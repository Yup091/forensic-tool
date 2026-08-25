#include "audit_logger.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <chrono>
#include <iomanip>
#include <ctime>
#include <filesystem>
#include <array>
#include <memory>
#include <dirent.h>

namespace fs = std::filesystem;

AuditLogger::AuditLogger() : nextCaseNumber(1) {
    // Try to load the highest case number from existing files
    auto caseIDs = getAllCaseIDs();
    if (!caseIDs.empty()) {
        // Extract number from last case ID
        std::string lastCase = caseIDs.back();
        try {
            size_t pos = lastCase.find_last_of('-');
            if (pos != std::string::npos) {
                nextCaseNumber = std::stoi(lastCase.substr(pos + 1)) + 1;
            }
        } catch (...) {
            nextCaseNumber = caseIDs.size() + 1;
        }
    }
}

std::string AuditLogger::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

std::string AuditLogger::generateCaseID() {
    std::stringstream ss;
    ss << "CASE-" << std::setfill('0') << std::setw(4) << nextCaseNumber++;
    return ss.str();
}

std::string AuditLogger::calculateRecordHash(const AuditRecord& record) {
    // Simple hash calculation using existing details
    std::string toHash = record.caseID + record.operationType + 
                        record.device + record.status;
    
    // In production, use proper SHA256. For prototype, use simple hash
    std::hash<std::string> hasher;
    size_t hashValue = hasher(toHash);
    
    std::stringstream ss;
    ss << std::hex << hashValue;
    return ss.str().substr(0, 16);  // Return first 16 chars
}

std::string AuditLogger::formatAuditRecord(const AuditRecord& record) {
    std::stringstream ss;
    
    ss << "╔════════════════════════════════════════════════════════════════╗\n";
    ss << "║                    FORENSIC AUDIT RECORD                       ║\n";
    ss << "╚════════════════════════════════════════════════════════════════╝\n\n";
    
    ss << "CASE ID:           " << record.caseID << "\n";
    ss << "Operation Type:    " << record.operationType << "\n";
    ss << "Status:            " << record.status << "\n";
    ss << "Exit Code:         " << record.exitCode << "\n\n";
    
    ss << "────────── DEVICE INFORMATION ──────────\n";
    ss << "Device Path:       " << record.device << "\n";
    ss << "Device Model:      " << record.deviceModel << "\n";
    ss << "Device Serial:     " << record.deviceSerial << "\n";
    ss << "Device Size:       " << record.deviceSize << "\n\n";
    
    ss << "────────── OPERATION DETAILS ──────────\n";
    ss << "Method:            " << record.method << "\n";
    ss << "Start Time:        " << record.startTime << "\n";
    ss << "End Time:          " << record.endTime << "\n\n";
    
    if (record.operationType == "RECOVERY") {
        ss << "Files Recovered:   " << record.filesRecovered << "\n\n";
    } else if (record.operationType == "ERASURE") {
        ss << "Erasure %:         " << std::fixed << std::setprecision(2) 
           << record.erasurePercentage << "%\n\n";
    }
    
    ss << "────────── DETAILS ────────────────────\n";
    ss << record.details << "\n\n";
    
    ss << "────────── INTEGRITY ──────────────────\n";
    ss << "Record Hash:       " << record.sha256Hash << "\n\n";
    
    ss << "═══════════════════════════════════════════════════════════════════\n";
    
    return ss.str();
}

AuditRecord AuditLogger::logRecovery(
    const Device& device,
    const RecoveryResult& result) {
    
    AuditRecord record;
    record.caseID = generateCaseID();
    record.operationType = "RECOVERY";
    record.device = device.path;
    record.deviceModel = device.model;
    record.deviceSerial = device.serial;
    record.deviceSize = device.size;
    record.method = result.method;
    record.startTime = result.timestamp;  // Simplified for prototype
    record.endTime = getCurrentTimestamp();
    record.status = result.success ? "SUCCESS" : "FAILED";
    record.exitCode = result.exitCode;
    record.filesRecovered = result.filesRecovered;
    record.details = result.details;
    record.sha256Hash = calculateRecordHash(record);
    
    return record;
}

AuditRecord AuditLogger::logErasure(
    const Device& device,
    const ErasureResult& result) {
    
    AuditRecord record;
    record.caseID = generateCaseID();
    record.operationType = "ERASURE";
    record.device = device.path;
    record.deviceModel = device.model;
    record.deviceSerial = device.serial;
    record.deviceSize = device.size;
    record.method = result.method;
    record.startTime = result.startTime;
    record.endTime = result.endTime;
    record.status = result.success ? "SUCCESS" : "FAILED";
    record.exitCode = result.exitCode;
    record.details = result.details;
    record.sha256Hash = calculateRecordHash(record);
    
    return record;
}

AuditRecord AuditLogger::logVerification(
    const Device& device,
    const VerificationResult& result) {
    
    AuditRecord record;
    record.caseID = generateCaseID();
    record.operationType = "VERIFICATION";
    record.device = device.path;
    record.deviceModel = device.model;
    record.deviceSerial = device.serial;
    record.deviceSize = device.size;
    record.method = result.method;
    record.startTime = result.timestamp;
    record.endTime = getCurrentTimestamp();
    record.status = result.statusString;
    record.erasurePercentage = result.percentageCleared;
    record.details = result.details;
    record.sha256Hash = calculateRecordHash(record);
    
    return record;
}

std::string AuditLogger::saveAuditRecord(
    const AuditRecord& record,
    const std::string& directory) {
    
    // Create directory if it doesn't exist
    try {
        fs::create_directories(directory);
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] Failed to create directory: " << e.what() << std::endl;
        return "";
    }
    
    // Save as text file
    std::string filePath = directory + record.caseID + ".txt";
    
    std::ofstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "[ERROR] Failed to open file for writing: " << filePath << std::endl;
        return "";
    }
    
    file << formatAuditRecord(record);
    file.close();
    
    std::cout << "[✓] Audit record saved: " << filePath << "\n";
    
    return filePath;
}

std::vector<std::string> AuditLogger::getAllCaseIDs(const std::string& directory) {
    std::vector<std::string> caseIDs;
    
    DIR* dir = opendir(directory.c_str());
    if (dir == nullptr) {
        return caseIDs;  // Return empty vector if directory doesn't exist
    }
    
    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        std::string filename = entry->d_name;
        
        // Look for CASE-*.txt files
        if (filename.find("CASE-") == 0 && filename.find(".txt") != std::string::npos) {
            // Extract case ID (without .txt)
            std::string caseID = filename.substr(0, filename.find(".txt"));
            caseIDs.push_back(caseID);
        }
    }
    
    closedir(dir);
    
    // Sort case IDs
    std::sort(caseIDs.begin(), caseIDs.end());
    
    return caseIDs;
}

AuditRecord AuditLogger::loadAuditRecord(
    const std::string& caseID,
    const std::string& directory) {
    
    AuditRecord record;
    std::string filePath = directory + caseID + ".txt";
    
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "[ERROR] Could not open audit record: " << filePath << std::endl;
        return record;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        // Parse the file content and populate record
        if (line.find("CASE ID:") != std::string::npos) {
            record.caseID = line.substr(line.find_last_of(' ') + 1);
        } else if (line.find("Operation Type:") != std::string::npos) {
            record.operationType = line.substr(line.find_last_of(' ') + 1);
        } else if (line.find("Status:") != std::string::npos) {
            record.status = line.substr(line.find_last_of(' ') + 1);
        } else if (line.find("Device Path:") != std::string::npos) {
            record.device = line.substr(line.find_last_of(' ') + 1);
        }
    }
    
    file.close();
    return record;
}
