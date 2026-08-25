#ifndef REPORT_GENERATOR_HPP
#define REPORT_GENERATOR_HPP

#include <string>
#include <vector>
#include "audit_logger.hpp"

class ReportGenerator {
public:
    ReportGenerator();
    
    /**
     * Generate text report from audit record
     * @param record Audit record to report on
     * @param outputPath Path to save report
     * @return Path to generated report
     */
    std::string generateTextReport(
        const AuditRecord& record,
        const std::string& outputPath = "reports/"
    );
    
    /**
     * Generate HTML report from audit record
     * @param record Audit record to report on
     * @param outputPath Path to save report
     * @return Path to generated report
     */
    std::string generateHTMLReport(
        const AuditRecord& record,
        const std::string& outputPath = "reports/"
    );
    
    /**
     * Generate comprehensive audit summary
     * @param caseIDs Vector of case IDs to include
     * @param outputPath Path to save report
     * @return Path to generated report
     */
    std::string generateAuditSummary(
        const std::vector<std::string>& caseIDs,
        const std::string& outputPath = "reports/"
    );
    
    /**
     * Generate chain of custody report
     * @param caseID Case ID for report
     * @param outputPath Path to save report
     * @return Path to generated report
     */
    std::string generateChainOfCustodyReport(
        const std::string& caseID,
        const std::string& outputPath = "reports/"
    );
    
private:
    /**
     * Get current timestamp
     * @return Timestamp string (YYYY-MM-DD HH:MM:SS)
     */
    std::string getCurrentTimestamp();
    
    /**
     * Generate HTML header
     * @param title Report title
     * @return HTML header string
     */
    std::string generateHTMLHeader(const std::string& title);
    
    /**
     * Generate HTML footer
     * @return HTML footer string
     */
    std::string generateHTMLFooter();
    
    /**
     * Format status with color for HTML
     * @param status Status string
     * @return HTML-formatted status
     */
    std::string formatStatusHTML(const std::string& status);
};

#endif // REPORT_GENERATOR_HPP
