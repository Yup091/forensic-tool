#include "erasure_manager.hpp"
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <chrono>
#include <iomanip>
#include <limits>
#include <array>
#include <memory>
#include <ctime>

ErasureManager::ErasureManager() {}

std::string ErasureManager::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

std::string ErasureManager::executeCommand(const std::string& command) {
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

bool ErasureManager::isToolAvailable(const std::string& tool) {
    std::string command = "which " + tool + " 2>/dev/null";
    std::string result = executeCommand(command);
    return !result.empty();
}

void ErasureManager::displayErasureMethods() {
    std::cout << "\n[*] Available erasure methods:\n\n";
    
    if (isToolAvailable("shred")) {
        std::cout << "  1. Shred (Standard overwrite - 1 pass)\n";
        std::cout << "     Fast, NIST-approved method\n\n";
    }
    
    if (isToolAvailable("nwipe")) {
        std::cout << "  2. Nwipe (DoD 5220.22-M standard)\n";
        std::cout << "     Multiple passes, military-grade erasure\n\n";
    }
    
    if (isToolAvailable("dd")) {
        std::cout << "  3. DD (Basic zero-fill)\n";
        std::cout << "     Quick method, fills device with zeros\n\n";
    }
    
    std::cout << "  0. Cancel\n\n";
}

bool ErasureManager::confirmErasure(const Device& device) {
    std::cout << "\n"
              << "╔════════════════════════════════════════════════════════════╗\n"
              << "║                      ⚠️  WARNING ⚠️                        ║\n"
              << "║                                                            ║\n"
              << "║  This operation will PERMANENTLY ERASE all data on:       ║\n"
              << "║  " << device.path << "\n"
              << "║  " << device.model << " (" << device.size << ")\n"
              << "║                                                            ║\n"
              << "║  THIS ACTION CANNOT BE UNDONE.                            ║\n"
              << "║                                                            ║\n"
              << "╚════════════════════════════════════════════════════════════╝\n\n";
    
    std::cout << "To confirm erasure, type exactly: ERASE\n";
    std::cout << "(or type anything else to cancel)\n\n";
    std::cout << "Confirmation: ";
    
    std::string confirmation;
    std::getline(std::cin, confirmation);
    
    return confirmation == "ERASE";
}

ErasureResult ErasureManager::eraseWithShred(
    const Device& device,
    int passes) {
    
    ErasureResult result;
    result.method = "shred";
    result.device = device.path;
    result.startTime = getCurrentTimestamp();
    result.exitCode = -1;
    result.success = false;
    
    // Check if shred is available
    if (!isToolAvailable("shred")) {
        result.details = "Shred is not installed on this system.";
        result.exitCode = -2;
        std::cout << "\n[!] Shred is not installed.\n";
        std::cout << "[*] Install with: sudo apt install coreutils\n";
        return result;
    }
    
    std::cout << "\n"
              << "╔════════════════════════════════════════════════════════════╗\n"
              << "║              SECURE ERASURE - SHRED METHOD                ║\n"
              << "╚════════════════════════════════════════════════════════════╝\n";
    
    std::cout << "\n[*] Device: " << device.path << "\n";
    std::cout << "[*] Model: " << device.model << "\n";
    std::cout << "[*] Size: " << device.size << "\n";
    std::cout << "[*] Method: Shred (" << passes << " pass)\n\n";
    
    // Confirm before proceeding
    if (!confirmErasure(device)) {
        result.details = "User cancelled erasure operation.";
        result.exitCode = 0;
        std::cout << "\n[*] Erasure cancelled.\n\n";
        return result;
    }
    
    std::cout << "\n[*] Starting erasure process...\n";
    std::cout << "[*] This may take several minutes depending on device size.\n\n";
    
    // Build shred command
    // -f: force (ignore file permissions)
    // -v: verbose
    // -z: zero final overwrite
    // -n passes: number of passes
    // Note: In production, you'd use -f /dev/device, but this is dangerous
    // For prototype, we show the command but don't actually execute it destructively
    
    std::string command = "echo ";
    command += "'shred -vfz -n " + std::to_string(passes) + " " + device.path + "'";
    
    std::cout << "[*] Command: ";
    std::string output = executeCommand(command);
    std::cout << output;
    
    std::cout << "\n[*] In production, this would execute the full shred command.\n";
    std::cout << "[!] For SAFETY in testing, actual erasure is NOT performed.\n\n";
    
    result.endTime = getCurrentTimestamp();
    result.exitCode = 0;
    result.success = true;
    result.details = "Erasure simulation complete (not actually destructive in prototype).";
    result.timeElapsedSeconds = 0.0;
    
    std::cout << "[✓] Erasure operation simulation complete.\n\n";
    
    return result;
}

ErasureResult ErasureManager::eraseWithNwipe(
    const Device& device,
    const std::string& method) {
    
    ErasureResult result;
    result.method = "nwipe (" + method + ")";
    result.device = device.path;
    result.startTime = getCurrentTimestamp();
    result.exitCode = -1;
    result.success = false;
    
    // Check if nwipe is available
    if (!isToolAvailable("nwipe")) {
        result.details = "Nwipe is not installed on this system.";
        result.exitCode = -2;
        std::cout << "\n[!] Nwipe is not installed.\n";
        std::cout << "[*] Install with: sudo apt install nwipe\n";
        return result;
    }
    
    std::cout << "\n"
              << "╔════════════════════════════════════════════════════════════╗\n"
              << "║              SECURE ERASURE - NWIPE METHOD                ║\n"
              << "╚════════════════════════════════════════════════════════════╝\n";
    
    std::cout << "\n[*] Device: " << device.path << "\n";
    std::cout << "[*] Model: " << device.model << "\n";
    std::cout << "[*] Size: " << device.size << "\n";
    std::cout << "[*] Method: Nwipe (" << method << ")\n\n";
    
    // Confirm before proceeding
    if (!confirmErasure(device)) {
        result.details = "User cancelled erasure operation.";
        result.exitCode = 0;
        std::cout << "\n[*] Erasure cancelled.\n\n";
        return result;
    }
    
    std::cout << "\n[*] Starting erasure process...\n";
    std::cout << "[*] Nwipe will perform multiple passes.\n\n";
    
    result.endTime = getCurrentTimestamp();
    result.exitCode = 0;
    result.success = true;
    result.details = "Nwipe erasure method selected (interactive mode required).";
    result.timeElapsedSeconds = 0.0;
    
    std::cout << "[*] Nwipe requires interactive operation.\n";
    std::cout << "[✓] Erasure method configured.\n\n";
    
    return result;
}

ErasureResult ErasureManager::eraseWithDD(const Device& device) {
    
    ErasureResult result;
    result.method = "dd (zero-fill)";
    result.device = device.path;
    result.startTime = getCurrentTimestamp();
    result.exitCode = -1;
    result.success = false;
    
    // Check if dd is available
    if (!isToolAvailable("dd")) {
        result.details = "DD is not installed on this system.";
        result.exitCode = -2;
        std::cout << "\n[!] DD is not available.\n";
        return result;
    }
    
    std::cout << "\n"
              << "╔════════════════════════════════════════════════════════════╗\n"
              << "║               SECURE ERASURE - DD METHOD                  ║\n"
              << "╚════════════════════════════════════════════════════════════╝\n";
    
    std::cout << "\n[*] Device: " << device.path << "\n";
    std::cout << "[*] Model: " << device.model << "\n";
    std::cout << "[*] Size: " << device.size << "\n";
    std::cout << "[*] Method: DD (Basic zero-fill)\n\n";
    
    // Confirm before proceeding
    if (!confirmErasure(device)) {
        result.details = "User cancelled erasure operation.";
        result.exitCode = 0;
        std::cout << "\n[*] Erasure cancelled.\n\n";
        return result;
    }
    
    std::cout << "\n[*] Starting erasure process...\n";
    std::cout << "[*] This will fill the device with zeros.\n\n";
    
    result.endTime = getCurrentTimestamp();
    result.exitCode = 0;
    result.success = true;
    result.details = "DD zero-fill erasure simulation complete.";
    result.timeElapsedSeconds = 0.0;
    
    std::cout << "[✓] Erasure operation simulation complete.\n\n";
    
    return result;
}
