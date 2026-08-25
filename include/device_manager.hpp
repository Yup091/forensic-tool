#ifndef DEVICE_MANAGER_HPP
#define DEVICE_MANAGER_HPP

#include <string>
#include <vector>

struct Device {
    std::string path;          // e.g., /dev/sda
    std::string name;          // e.g., sda
    std::string size;          // e.g., 931G
    std::string type;          // e.g., disk, part
    std::string fstype;        // e.g., ext4, ntfs
    std::string model;         // e.g., Samsung SSD
    std::string serial;        // e.g., S1234567
    std::string mountpoints;   // e.g., /home
};

class DeviceManager {
public:
    DeviceManager();
    
    /**
     * Discover all block devices on the system
     * @return Vector of Device structures
     */
    std::vector<Device> discoverDevices();
    
    /**
     * Display devices in a formatted table
     * @param devices Vector of devices to display
     */
    void displayDevices(const std::vector<Device>& devices);
    
    /**
     * Get device by user selection
     * @param devices Vector of available devices
     * @return Selected Device, or empty Device if invalid
     */
    Device selectDevice(const std::vector<Device>& devices);
    
private:
    /**
     * Parse lsblk output into Device structures
     * @param output Raw lsblk output
     * @return Vector of parsed devices
     */
    std::vector<Device> parseLsblkOutput(const std::string& output);
    
    /**
     * Execute system command and capture output
     * @param command Command to execute
     * @return Command output as string
     */
    std::string executeCommand(const std::string& command);
};

#endif // DEVICE_MANAGER_HPP
