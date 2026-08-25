#ifndef AUDIT_LOGGER_HPP
#define AUDIT_LOGGER_HPP

#include <string>
#include <vector>
#include "device_manager.hpp"
#include "recovery_manager.hpp"
#include "erasure_manager.hpp"
#include "verifier.hpp"

struct AuditRecord {
    std::string caseID;              // e.g., CASE-0001 or CASE-20260825-143210
    std::string operationType;       // RECOVERY, ERASURE, VERIFICATION
    std::string device;              // /dev/sda
    std::string deviceModel;         // Samsung SSD
    std::string deviceSerial;        // S1234567
    std::string deviceSize;          // 931G
    std::string method;              // PhotoRec, shred, etc.
    std::string startTime;           // YYYY-MM-DD HH:MM:SS
    std::string endTime;             // YYYY-MM-DD HH:MM:SS
    std::string status;              // SUCCESS, FAILED, INCONCLUSIVE
    int exitCode;
    std::string details;
    int filesRecovered;              // For recovery operations
    double erasurePercentage;        // For erasure verification
    std::string sha256Hash;          // Hash of report for integrity
};

class AuditLogger {
public:
    AuditLogger();
    
    /**
     * Generate next case ID
     * @return New case ID (CASE-NNNN format)
     */
    std::string generateCaseID();
    
    /**
     * Log a recovery operation
     * @param device Device used
     * @param result Recovery result
     * @return AuditRecord with case ID
     */
    AuditRecord logRecovery(
        const Device& device,
        const RecoveryResult& result
    );
    
    /**
     * Log an erasure operation
     * @param device Device used
     * @param result Erasure result
     * @return AuditRecord with case ID
     */
    AuditRecord logErasure(
        const Device& device,
        const ErasureResult& result
    );
    
    /**
     * Log a verification operation
     * @param device Device verified
     * @param result Verification result
     * @return AuditRecord with case ID
     */
    AuditRecord logVerification(
        const Device& device,
        const VerificationResult& result
    );
    
    /**
     * Save audit record to file
     * @param record Audit record to save
     * @param directory Directory to save in
     * @return Path to saved file
     */
    std::string saveAuditRecord(
        const AuditRecord& record,
        const std::string& directory = "logs/"
    );
    
    /**
     * Get all audit records from directory
     * @param directory Directory to search
     * @return Vector of case IDs found
     */
    std::vector<std::string> getAllCaseIDs(const std::string& directory = "logs/");
    
    /**
     * Load audit record from file
     * @param caseID Case ID to load
     * @param directory Directory to search in
     * @return AuditRecord, or empty record if not found
     */
    AuditRecord loadAuditRecord(
        const std::string& caseID,
        const std::string& directory = "logs/"
    );
    
private:
    /**
     * Get current timestamp
     * @return Timestamp string (YYYY-MM-DD HH:MM:SS)
     */
    std::string getCurrentTimestamp();
    
    /**
     * Calculate SHA256 hash of audit record
     * @param record Record to hash
     * @return SHA256 hash string
     */
    std::string calculateRecordHash(const AuditRecord& record);
    
    /**
     * Format audit record for file storage
     * @param record Record to format
     * @return Formatted string
     */
    std::string formatAuditRecord(const AuditRecord& record);
    
    int nextCaseNumber;
};

#endif // AUDIT_LOGGER_HPP
