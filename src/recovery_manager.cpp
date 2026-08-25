#include "recovery_manager.hpp"
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <iomanip>
#include <limits>
#include <array>
#include <memory>
#include <dirent.h>
#include <sys/types.h>

RecoveryManager::RecoveryManager() {}

std::string RecoveryManager::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

std::string RecoveryManager::executeCommand(const std::string& command) {
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

bool RecoveryManager::isPhotoRecAvailable() {
    std::string result = executeCommand("which photorec 2>/dev/null");
    return !result.empty();
}

bool RecoveryManager::isTestDiskAvailable() {
    std::string result = executeCommand("which testdisk 2>/dev/null");
    return !result.empty();
}

void RecoveryManager::displaySupportedFileTypes() {
    std::cout << "\n[*] Supported file types for recovery:\n\n";
    std::cout << "  1. JPG/JPEG Images       (jpg, jpeg)\n";
    std::cout << "  2. PNG Images            (png)\n";
    std::cout << "  3. GIF Images            (gif)\n";
    std::cout << "  4. BMP Images            (bmp)\n";
    std::cout << "  5. PDF Documents         (pdf)\n";
    std::cout << "  6. ZIP Archives          (zip)\n";
    std::cout << "  7. Office Documents      (doc, docx, xls, xlsx, ppt)\n";
    std::cout << "  8. Text Files            (txt)\n";
    std::cout << "  9. Videos                (mp4, avi, mov, mkv)\n";
    std::cout << "  10. Audio                (mp3, wav, flac, m4a)\n\n";
}

std::string RecoveryManager::selectFileTypes() {
    displaySupportedFileTypes();
    
    std::cout << "Enter file type numbers (comma-separated, or press Enter for default JPG/PNG): ";
    std::string input;
    std::getline(std::cin, input);
    
    if (input.empty()) {
        return "jpg,jpeg,png";  // Default
    }
    
    // Parse input - for prototype, just return comma-separated extensions
    // Full implementation would map numbers to extensions
    return input;
}

int RecoveryManager::countRecoveredFiles(const std::string& recoveryDir) {
    int fileCount = 0;
    
    DIR* dir = opendir(recoveryDir.c_str());
    if (dir != nullptr) {
        struct dirent* entry;
        while ((entry = readdir(dir)) != nullptr) {
            // Count regular files (skip . and ..)
            if (entry->d_type == DT_REG) {
                fileCount++;
            }
        }
        closedir(dir);
    }
    
    return fileCount;
}

RecoveryResult RecoveryManager::recoverWithPhotoRec(
    const Device& device,
    const std::string& outputDirectory) {
    
    RecoveryResult result;
    result.method = "PhotoRec";
    result.outputDirectory = outputDirectory;
    result.timestamp = getCurrentTimestamp();
    result.filesRecovered = 0;
    result.filesFailed = 0;
    result.filesAttempted = 0;
    result.success = false;
    result.exitCode = -1;
    
    // Check if PhotoRec is available
    if (!isPhotoRecAvailable()) {
        result.details = "PhotoRec is not installed on this system.";
        result.exitCode = -2;
        std::cout << "\n[!] PhotoRec is not installed.\n";
        std::cout << "[*] Install with: sudo apt install testdisk\n";
        return result;
    }
    
    // Verify device path
    if (device.path.empty()) {
        result.details = "No device selected.";
        result.exitCode = -1;
        std::cout << "\n[!] No device selected. Please select a device first (Option 1).\n";
        return result;
    }
    
    std::cout << "\n╔══════════════════════════════════════╗\n";
    std::cout << "║  PHOTOREC RECOVERY                   ║\n";
    std::cout << "╚══════════════════════════════════════╝\n";
    
    std::cout << "\n[*] Device: " << device.path << "\n";
    std::cout << "[*] Model: " << device.model << "\n";
    std::cout << "[*] Size: " << device.size << "\n";
    std::cout << "[*] Output: " << outputDirectory << "\n\n";
    
    std::cout << "[⚠]  WARNING: This will attempt to recover deleted files.\n";
    std::cout << "[*] The device will NOT be modified.\n\n";
    
    std::cout << "Continue? (yes/no): ";
    std::string confirm;
    std::cin >> confirm;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    
    if (confirm != "yes" && confirm != "y") {
        std::cout << "\n[*] Recovery cancelled.\n";
        result.details = "User cancelled operation.";
        result.exitCode = 0;
        return result;
    }
    
    // Create output directory
    std::string mkdirCmd = "mkdir -p \"" + outputDirectory + "\"";
    executeCommand(mkdirCmd);
    
    // Execute PhotoRec
    // PhotoRec requires -c to disable interactive mode
    // For prototype, we'll use it in interactive mode and let it run
    std::cout << "\n[*] Launching PhotoRec...\n";
    std::cout << "[*] This may take several minutes depending on device size.\n\n";
    
    // Build PhotoRec command
    std::string command = "photorec /log /d \"" + outputDirectory + "\" /cmd \"" + 
                         device.path + "\" estimate 2>&1";
    
    std::string output = executeCommand(command);
    
    std::cout << output;
    
    // Count recovered files
    result.filesRecovered = countRecoveredFiles(outputDirectory);
    
    if (result.filesRecovered > 0) {
        result.success = true;
        result.exitCode = 0;
        result.details = "Recovery completed successfully.";
        std::cout << "\n[✓] Recovery complete: " << result.filesRecovered << " files recovered.\n\n";
    } else {
        result.success = false;
        result.exitCode = 1;
        result.details = "No files recovered.";
        std::cout << "\n[!] No files were recovered. Device may not contain deleted files.\n\n";
    }
    
    return result;
}

RecoveryResult RecoveryManager::recoverWithTestDisk(
    const Device& device,
    const std::string& outputDirectory) {
    
    RecoveryResult result;
    result.method = "TestDisk";
    result.outputDirectory = outputDirectory;
    result.timestamp = getCurrentTimestamp();
    result.filesRecovered = 0;
    result.filesFailed = 0;
    result.filesAttempted = 0;
    result.success = false;
    result.exitCode = -1;
    
    // Check if TestDisk is available
    if (!isTestDiskAvailable()) {
        result.details = "TestDisk is not installed on this system.";
        result.exitCode = -2;
        std::cout << "\n[!] TestDisk is not installed.\n";
        std::cout << "[*] Install with: sudo apt install testdisk\n";
        return result;
    }
    
    std::cout << "\n╔══════════════════════════════════════╗\n";
    std::cout << "║  TESTDISK RECOVERY                   ║\n";
    std::cout << "╚══════════════════════════════════════╝\n";
    
    std::cout << "\n[*] TestDisk is specialized for partition/filesystem recovery.\n";
    std::cout << "[*] Device: " << device.path << "\n";
    std::cout << "[*] This tool requires interactive use.\n\n";
    
    result.details = "TestDisk recovery interface not automated in prototype.";
    result.success = false;
    result.exitCode = 1;
    
    return result;
}
