#include "menu.hpp"
#include <iostream>
#include <string>
#include <limits>

Menu::Menu() : running(true), deviceManager(), recoveryManager(), erasureManager(), verifier(), auditLogger() {}

void Menu::run() {
    while (running) {
        displayHeader();
        displayMenu();
        int choice = getUserChoice();
        handleChoice(choice);
    }
}

void Menu::displayHeader() {
    std::cout << "\n";
    std::cout << "╔═════════════════════════════════════╘\n";
    std::cout << "║       FORENSIC DATA TOOL             ║\n";
    std::cout << "╚═════════════════════════════════════╝\n";
    std::cout << "\n";
}

void Menu::displayMenu() {
    std::cout << "╔═════════════════════════════════════╘\n";
    std::cout << "║           MAIN MENU                  ║\n";
    std::cout << "╠═════════════════════════════════════╣\n";
    std::cout << "║ 1. Identify Device                   ║\n";
    std::cout << "║ 2. Recover Deleted Files             ║\n";
    std::cout << "║ 3. Securely Erase Device             ║\n";
    std::cout << "║ 4. Verify Erasure                    ║\n";
    std::cout << "║ 5. Generate Audit Report             ║\n";
    std::cout << "║ 6. View Previous Reports             ║\n";
    std::cout << "║ 7. Exit                              ║\n";
    std::cout << "╚═════════════════════════════════════╝\n";
    std::cout << "\n";
}

int Menu::getUserChoice() {
    int choice = -1;
    std::cout << "Select: ";
    
    if (std::cin >> choice) {
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return choice;
    }
    
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return -1;
}

void Menu::handleChoice(int choice) {
    switch (choice) {
        case 1:
            option1_IdentifyDevice();
            break;
        case 2:
            option2_RecoverFiles();
            break;
        case 3:
            option3_SecureErase();
            break;
        case 4:
            option4_VerifyErasure();
            break;
        case 5:
            option5_GenerateReport();
            break;
        case 6:
            option6_ViewReports();
            break;
        case 7:
            option7_Exit();
            break;
        default:
            std::cout << "\n[!] Invalid choice. Please select 1-7.\n";
            std::cout << "Press Enter to continue...";
            std::cin.ignore();
    }
}

void Menu::option1_IdentifyDevice() {
    std::cout << "\n╔═════════════════════════════════════╘\n";
    std::cout << "║  1. IDENTIFY DEVICE                  ║\n";
    std::cout << "╚═════════════════════════════════════╝\n";
    
    auto devices = deviceManager.discoverDevices();
    
    if (devices.empty()) {
        std::cout << "[!] No devices found or insufficient privileges.\n";
        std::cout << "[*] Try running with: sudo ./forensic-tool\n\n";
    } else {
        selectedDevice = deviceManager.selectDevice(devices);
        
        if (!selectedDevice.path.empty()) {
            std::cout << "\n[✓] Device Details:\n";
            std::cout << "    Path: " << selectedDevice.path << "\n";
            std::cout << "    Model: " << selectedDevice.model << "\n";
            std::cout << "    Size: " << selectedDevice.size << "\n";
            std::cout << "    Type: " << selectedDevice.type << "\n";
            std::cout << "    Filesystem: " << selectedDevice.fstype << "\n\n";
        }
    }
    
    std::cout << "Press Enter to continue...";
    std::cin.ignore();
}

void Menu::option2_RecoverFiles() {
    std::cout << "\n╔═════════════════════════════════════╘\n";
    std::cout << "║  2. RECOVER DELETED FILES            ║\n";
    std::cout << "╚═════════════════════════════════════╝\n";
    
    std::cout << "\n[*] Available recovery methods:\n";
    std::cout << "    1. PhotoRec (File carving - finds deleted files)\n";
    std::cout << "    2. TestDisk (Partition/filesystem recovery)\n";
    std::cout << "    0. Cancel\n\n";
    
    std::cout << "Select method: ";
    int method = -1;
    std::cin >> method;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    
    std::string outputDir = "recovery/";
    
    switch (method) {
        case 1: {
            if (selectedDevice.path.empty()) {
                std::cout << "\n[!] No device selected. Please run Option 1 first.\n\n";
                std::cout << "Press Enter to continue...";
                std::cin.ignore();
                return;
            }
            
            RecoveryResult result = recoveryManager.recoverWithPhotoRec(selectedDevice, outputDir);
            std::cout << "\n[*] Recovery Method: " << result.method << "\n";
            std::cout << "[*] Files Recovered: " << result.filesRecovered << "\n";
            std::cout << "[*] Status: " << result.details << "\n\n";
            
            // Log the operation
            AuditRecord record = auditLogger.logRecovery(selectedDevice, result);
            auditLogger.saveAuditRecord(record);
            
            std::cout << "[*] Case ID: " << record.caseID << "\n\n";
            break;
        }
        case 2: {
            if (selectedDevice.path.empty()) {
                std::cout << "\n[!] No device selected. Please run Option 1 first.\n\n";
                std::cout << "Press Enter to continue...";
                std::cin.ignore();
                return;
            }
            
            RecoveryResult result = recoveryManager.recoverWithTestDisk(selectedDevice, outputDir);
            std::cout << "\n[*] Recovery Method: " << result.method << "\n";
            std::cout << "[*] Status: " << result.details << "\n\n";
            
            // Log the operation
            AuditRecord record = auditLogger.logRecovery(selectedDevice, result);
            auditLogger.saveAuditRecord(record);
            
            std::cout << "[*] Case ID: " << record.caseID << "\n\n";
            break;
        }
        case 0:
            std::cout << "\n[*] Cancelled.\n\n";
            break;
        default:
            std::cout << "\n[!] Invalid selection.\n\n";
    }
    
    std::cout << "Press Enter to continue...";
    std::cin.ignore();
}

void Menu::option3_SecureErase() {
    std::cout << "\n╔═════════════════════════════════════╘\n";
    std::cout << "║  3. SECURELY ERASE DEVICE            ║\n";
    std::cout << "╚═════════════════════════════════════╝\n";
    
    if (selectedDevice.path.empty()) {
        std::cout << "\n[!] No device selected. Please run Option 1 first.\n\n";
        std::cout << "Press Enter to continue...";
        std::cin.ignore();
        return;
    }
    
    erasureManager.displayErasureMethods();
    
    std::cout << "Select method: ";
    int method = -1;
    std::cin >> method;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    
    ErasureResult result;
    
    switch (method) {
        case 1: {
            result = erasureManager.eraseWithShred(selectedDevice, 1);
            break;
        }
        case 2: {
            result = erasureManager.eraseWithNwipe(selectedDevice, "dod5220");
            break;
        }
        case 3: {
            result = erasureManager.eraseWithDD(selectedDevice);
            break;
        }
        case 0:
            std::cout << "\n[*] Cancelled.\n\n";
            std::cout << "Press Enter to continue...";
            std::cin.ignore();
            return;
        default:
            std::cout << "\n[!] Invalid selection.\n\n";
            std::cout << "Press Enter to continue...";
            std::cin.ignore();
            return;
    }
    
    std::cout << "[*] Erasure Method: " << result.method << "\n";
    std::cout << "[*] Status: " << (result.success ? "SUCCESS" : "FAILED") << "\n";
    std::cout << "[*] Details: " << result.details << "\n\n";
    
    // Log the operation
    AuditRecord record = auditLogger.logErasure(selectedDevice, result);
    auditLogger.saveAuditRecord(record);
    
    std::cout << "[*] Case ID: " << record.caseID << "\n\n";
    
    std::cout << "Press Enter to continue...";
    std::cin.ignore();
}

void Menu::option4_VerifyErasure() {
    std::cout << "\n╔═════════════════════════════════════╘\n";
    std::cout << "║  4. VERIFY ERASURE                   ║\n";
    std::cout << "╚═════════════════════════════════════╝\n";
    
    if (selectedDevice.path.empty()) {
        std::cout << "\n[!] No device selected. Please run Option 1 first.\n\n";
        std::cout << "Press Enter to continue...";
        std::cin.ignore();
        return;
    }
    
    VerificationResult result = verifier.verifyErasure(selectedDevice);
    
    std::cout << "[*] Verification Summary:\n";
    std::cout << "    Status: " << result.statusString << "\n";
    std::cout << "    Method: " << result.method << "\n";
    std::cout << "    Details: " << result.details << "\n\n";
    
    // Log the operation
    AuditRecord record = auditLogger.logVerification(selectedDevice, result);
    auditLogger.saveAuditRecord(record);
    
    std::cout << "[*] Case ID: " << record.caseID << "\n\n";
    
    std::cout << "Press Enter to continue...";
    std::cin.ignore();
}

void Menu::option5_GenerateReport() {
    std::cout << "\n[*] 5. Generate Audit Report\n";
    std::cout << "    Not implemented yet.\n\n";
    std::cout << "Press Enter to continue...";
    std::cin.ignore();
}

void Menu::option6_ViewReports() {
    std::cout << "\n[*] 6. View Previous Reports\n";
    std::cout << "    Not implemented yet.\n\n";
    std::cout << "Press Enter to continue...";
    std::cin.ignore();
}

void Menu::option7_Exit() {
    std::cout << "\n[*] Exiting Forensic Data Tool...\n\n";
    running = false;
}
