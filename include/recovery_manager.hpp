#ifndef RECOVERY_MANAGER_HPP
#define RECOVERY_MANAGER_HPP

#include <string>
#include <vector>
#include "device_manager.hpp"

struct RecoveryResult {
    bool success;
    std::string method;              // e.g., "PhotoRec", "TestDisk"
    std::string outputDirectory;
    int filesRecovered;
    int filesFailed;
    int filesAttempted;
    int exitCode;
    std::string recoveryLocation;
    std::string timestamp;
    std::string details;
};

class RecoveryManager {
public:
    RecoveryManager();
    
    /**
     * Recover deleted files using PhotoRec
     * @param device Target device to recover from
     * @param outputDirectory Where to store recovered files
     * @return RecoveryResult with operation details
     */
    RecoveryResult recoverWithPhotoRec(
        const Device& device,
        const std::string& outputDirectory
    );
    
    /**
     * Recover files using TestDisk (filesystem/partition recovery)
     * @param device Target device to recover from
     * @param outputDirectory Where to store recovered files
     * @return RecoveryResult with operation details
     */
    RecoveryResult recoverWithTestDisk(
        const Device& device,
        const std::string& outputDirectory
    );
    
    /**
     * Display supported file types for recovery
     */
    void displaySupportedFileTypes();
    
    /**
     * Get user selection of file types to recover
     * @return Comma-separated list of extensions (e.g., "jpg,png,gif")
     */
    std::string selectFileTypes();
    
    /**
     * Check if PhotoRec is installed
     * @return true if available, false otherwise
     */
    bool isPhotoRecAvailable();
    
    /**
     * Check if TestDisk is installed
     * @return true if available, false otherwise
     */
    bool isTestDiskAvailable();
    
private:
    /**
     * Execute system command and capture output
     * @param command Command to execute
     * @return Command output as string
     */
    std::string executeCommand(const std::string& command);
    
    /**
     * Count recovered files in recovery directory
     * @param recoveryDir Directory to scan
     * @return Number of files found
     */
    int countRecoveredFiles(const std::string& recoveryDir);
    
    /**
     * Get current timestamp
     * @return Timestamp string (YYYY-MM-DD HH:MM:SS)
     */
    std::string getCurrentTimestamp();
};

#endif // RECOVERY_MANAGER_HPP
