#include "device_manager.hpp"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cstdlib>
#include <limits>
#include <array>
#include <memory>

DeviceManager::DeviceManager() {}

std::string DeviceManager::executeCommand(const std::string& command) {
    std::array<char, 128> buffer;
    std::string result;
    
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(command.c_str(), "r"), &pclose);
    if (!pipe) {
        std::cerr << "[ERROR] Failed to execute command: " << command << std::endl;
        return "";
    }
    
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    
    return result;
}

std::vector<Device> DeviceManager::parseLsblkOutput(const std::string& output) {
    std::vector<Device> devices;
    std::istringstream stream(output);
    std::string line;
    bool firstLine = true;
    
    while (std::getline(stream, line)) {
        // Skip empty lines and the header
        if (line.empty() || firstLine) {
            firstLine = false;
            continue;
        }
        
        // Parse each field from lsblk output
        std::istringstream lineStream(line);
        Device device;
        
        std::string name, size, type, fstype, model, serial, mountpoints;
        
        if (lineStream >> name >> size >> type >> fstype >> model >> serial) {
            // Read the rest of the line as mountpoints (may contain spaces)
            std::getline(lineStream, mountpoints);
            
            // Clean up mountpoints
            if (!mountpoints.empty() && mountpoints[0] == ' ') {
                mountpoints = mountpoints.substr(1);
            }
            
            device.name = name;
            device.path = "/dev/" + name;
            device.size = size;
            device.type = type;
            device.fstype = fstype;
            device.model = model;
            device.serial = serial;
            device.mountpoints = mountpoints;
            
            devices.push_back(device);
        }
    }
    
    return devices;
}

std::vector<Device> DeviceManager::discoverDevices() {
    std::cout << "[*] Scanning for block devices...\n" << std::flush;
    
    // Use lsblk to get device information
    // -l: list format
    // -o: columns to output
    // -d: only devices, not partitions (for initial discovery)
    std::string command = "lsblk -l -o NAME,SIZE,TYPE,FSTYPE,MODEL,SERIAL,MOUNTPOINTS 2>/dev/null";
    std::string output = executeCommand(command);
    
    if (output.empty()) {
        std::cerr << "[!] Warning: Could not enumerate devices. You may need root privileges.\n";
        return {};
    }
    
    return parseLsblkOutput(output);
}

void DeviceManager::displayDevices(const std::vector<Device>& devices) {
    if (devices.empty()) {
        std::cout << "\n[!] No block devices found.\n\n";
        return;
    }
    
    std::cout << "\n";
    std::cout << "╔════════════════════════════════════════════════════════════════════════╗\n";
    std::cout << "║                       AVAILABLE DEVICES                               ║\n";
    std::cout << "╠════════════════════════════════════════════════════════════════════════╣\n";
    
    for (size_t i = 0; i < devices.size(); ++i) {
        const auto& dev = devices[i];
        printf("║ %zu. %-8s  %-12s  %-10s  %-20s  │\n",
               i + 1,
               dev.path.c_str(),
               dev.size.c_str(),
               dev.type.c_str(),
               dev.model.c_str());
        printf("║    Filesystem: %-48s│\n", dev.fstype.c_str());
        if (!dev.mountpoints.empty()) {
            printf("║    Mounted: %-52s│\n", dev.mountpoints.c_str());
        }
        if (i < devices.size() - 1) {
            std::cout << "║────────────────────────────────────────────────────────────────────────║\n";
        }
    }
    
    std::cout << "╚════════════════════════════════════════════════════════════════════════╝\n\n";
}

Device DeviceManager::selectDevice(const std::vector<Device>& devices) {
    if (devices.empty()) {
        return Device();
    }
    
    displayDevices(devices);
    
    int choice = -1;
    std::cout << "Select device number (1-" << devices.size() << "), or 0 to cancel: ";
    
    if (std::cin >> choice) {
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        
        if (choice > 0 && choice <= static_cast<int>(devices.size())) {
            std::cout << "\n[✓] Selected: " << devices[choice - 1].path << std::endl;
            return devices[choice - 1];
        } else if (choice == 0) {
            std::cout << "\n[*] Cancelled.\n";
            return Device();
        }
    } else {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    
    std::cout << "\n[!] Invalid selection.\n";
    return Device();
}
