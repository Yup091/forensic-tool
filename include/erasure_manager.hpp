#ifndef ERASURE_MANAGER_HPP
#define ERASURE_MANAGER_HPP

#include <string>
#include <vector>
#include "device_manager.hpp"

struct ErasureResult {
    bool success;
    std::string method;              // e.g., "shred", "nwipe", "secure_erase"
    std::string device;
    int exitCode;
    std::string timestamp;
    std::string details;
    std::string startTime;
    std::string endTime;
    double timeElapsedSeconds;
};

class ErasureManager {
public:
    ErasureManager();
    
    /**
     * Display available erasure methods
     */
    void displayErasureMethods();
    
    /**
     * Securely erase device using shred
     * @param device Device to erase
     * @param passes Number of overwrite passes (default 1)
     * @return ErasureResult with operation details
     */
    ErasureResult eraseWithShred(
        const Device& device,
        int passes = 1
    );
    
    /**
     * Securely erase device using nwipe
     * @param device Device to erase
     * @param method Erasure method (e.g., "dod5220", "gutmann")
     * @return ErasureResult with operation details
     */
    ErasureResult eraseWithNwipe(
        const Device& device,
        const std::string& method = "dod5220"
    );
    
    /**
     * Securely erase device using dd (basic method)
     * @param device Device to erase
     * @return ErasureResult with operation details
     */
    ErasureResult eraseWithDD(const Device& device);
    
    /**
     * Check if erasure tool is available
     * @param tool Tool name ("shred", "nwipe", "dd")
     * @return true if available, false otherwise
     */
    bool isToolAvailable(const std::string& tool);
    
private:
    /**
     * Execute system command and capture output
     * @param command Command to execute
     * @return Command output as string
     */
    std::string executeCommand(const std::string& command);
    
    /**
     * Get current timestamp
     * @return Timestamp string (YYYY-MM-DD HH:MM:SS)
     */
    std::string getCurrentTimestamp();
    
    /**
     * Confirm destructive operation with user
     * @param device Device to be erased
     * @return true if user confirms, false otherwise
     */
    bool confirmErasure(const Device& device);
};

#endif // ERASURE_MANAGER_HPP
