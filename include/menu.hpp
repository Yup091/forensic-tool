#ifndef MENU_HPP
#define MENU_HPP

#include <iostream>
#include <string>
#include "device_manager.hpp"

class Menu {
public:
    Menu();
    void run();

private:
    void displayHeader();
    void displayMenu();
    int getUserChoice();
    void handleChoice(int choice);
    
    void option1_IdentifyDevice();
    void option2_RecoverFiles();
    void option3_SecureErase();
    void option4_VerifyErasure();
    void option5_GenerateReport();
    void option6_ViewReports();
    void option7_Exit();
    
    bool running;
    DeviceManager deviceManager;
    Device selectedDevice;
};

#endif // MENU_HPP
