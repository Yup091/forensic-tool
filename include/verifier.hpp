#ifndef VERIFIER_HPP
#define VERIFIER_HPP

#include <string>
#include "device_manager.hpp"

enum class VerificationStatus {
    PASSED,
    FAILED,
    INCONCLUSIVE,
    ERROR
};

struct VerificationResult {
    VerificationStatus status;
    std::string statusString;
    std::string method;
    std::string device;
    std::string details;
    int bytesChecked;
    int bytesZero;
    int bytesFailed;
    double percentageCleared;
    std::string timestamp;
};

class Verifier {
public:
    Verifier();
    
    /**
     * Verify that device has been properly erased
     * @param device Device to verify
     * @return VerificationResult with detailed status
     */
    VerificationResult verifyErasure(const Device& device);
    
    /**
     * Verify recovered files integrity using checksums
     * @param recoveryDirectory Directory containing recovered files
     * @return VerificationResult for recovery integrity
     */
    VerificationResult verifyRecoveryIntegrity(const std::string& recoveryDirectory);
    
    /**
     * Check if file has been properly recovered (readable, not corrupted)
     * @param filePath Path to recovered file
     * @return true if file appears valid, false otherwise
     */
    bool isFileValid(const std::string& filePath);
    
private:
    /**
     * Execute system command and capture output
     * @param command Command to execute
     * @return Command output as string
     */
    std::string executeCommand(const std::string& command);
    
    /**
     * Calculate SHA256 hash of a file
     * @param filePath Path to file
     * @return SHA256 hash string
     */
    std::string calculateSHA256(const std::string& filePath);
    
    /**
     * Get current timestamp
     * @return Timestamp string (YYYY-MM-DD HH:MM:SS)
     */
    std::string getCurrentTimestamp();
    
    /**
     * Read sample bytes from device for verification
     * @param device Device to sample
     * @return Number of zero bytes in sample
     */
    int sampleDeviceBytes(const Device& device);
};

#endif // VERIFIER_HPP
