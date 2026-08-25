#include "menu.hpp"
#include <iostream>
#include <string>
#include <limits>

Menu::Menu() : running(true) {}

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
    std::cout << "╔══════════════════════════════════════╗\n";
    std::cout << "║       FORENSIC DATA TOOL             ║\n";
    std::cout << "╚══════════════════════════════════════╝\n";
    std::cout << "\n";
}

void Menu::displayMenu() {
    std::cout << "╔══════════════════════════════════════╗\n";
    std::cout << "║           MAIN MENU                  ║\n";
    std::cout << "╠══════════════════════════════════════╣\n";
    std::cout << "║ 1. Identify Device                   ║\n";
    std::cout << "║ 2. Recover Deleted Files             ║\n";
    std::cout << "║ 3. Securely Erase Device             ║\n";
    std::cout << "║ 4. Verify Erasure                    ║\n";
    std::cout << "║ 5. Generate Audit Report             ║\n";
    std::cout << "║ 6. View Previous Reports             ║\n";
    std::cout << "║ 7. Exit                              ║\n";
    std::cout << "╚══════════════════════════════════════╝\n";
    std::cout << "\n";
}

int Menu::getUserChoice() {
    int choice = -1;
    std::cout << "Select: ";
    
    if (std::cin >> choice) {
        // Clear the input buffer
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return choice;
    }
    
    // Clear the input buffer if invalid input
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
    std::cout << "\n[*] 1. Identify Device\n";
    std::cout << "    Not implemented yet.\n\n";
    std::cout << "Press Enter to continue...";
    std::cin.ignore();
}

void Menu::option2_RecoverFiles() {
    std::cout << "\n[*] 2. Recover Deleted Files\n";
    std::cout << "    Not implemented yet.\n\n";
    std::cout << "Press Enter to continue...";
    std::cin.ignore();
}

void Menu::option3_SecureErase() {
    std::cout << "\n[*] 3. Securely Erase Device\n";
    std::cout << "    Not implemented yet.\n\n";
    std::cout << "Press Enter to continue...";
    std::cin.ignore();
}

void Menu::option4_VerifyErasure() {
    std::cout << "\n[*] 4. Verify Erasure\n";
    std::cout << "    Not implemented yet.\n\n";
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
