#include <windows.h>
#include <iostream>
#include <string>
#include <fstream>

struct employee {
    int num;        
    char name[10];   
    double hours;  
};

void DisplayBinaryFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cout << "Error opening file: " << filename << std::endl;
        return;
    }

    employee emp;
    int count = 0;
    std::cout << "\nBinary file contents:\n";
    std::cout << "ID\tName\t\tHours\n";
    std::cout << "------------------------\n";

    while (file.read(reinterpret_cast<char*>(&emp), sizeof(employee))) {
        std::cout << emp.num << "\t" << emp.name << "\t\t" << emp.hours << "\n";
        count++;
    }

    file.close();
    std::cout << "Total records: " << count << std::endl;
}

void DisplayTextFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file) {
        std::cout << "Error opening file: " << filename << std::endl;
        return;
    }

    std::string line;
    std::cout << "\nReport contents:\n";
    while (std::getline(file, line)) {
        std::cout << line << std::endl;
    }

    file.close();
}

int main() {
    std::string binFilename;
    int recordCount;
    std::string reportFilename;
    double hourlyRate;

    std::cout << "Enter binary file name: ";
    std::cin >> binFilename;
    std::cout << "Enter number of records: ";
    std::cin >> recordCount;

    std::string creatorCmdLine = "Creator.exe " + binFilename + " " + std::to_string(recordCount);

    STARTUPINFO siCreator;
    PROCESS_INFORMATION piCreator;

    ZeroMemory(&siCreator, sizeof(STARTUPINFO));
    siCreator.cb = sizeof(siCreator);

    std::cout << "Launching Creator with parameters: " << creatorCmdLine << std::endl;

    if (!CreateProcess(
        NULL,
        &creatorCmdLine[0],   
        NULL,     
        NULL,        
        FALSE, 
        0,             
        NULL,        
        NULL,            
        &siCreator,         
        &piCreator      
    )) {
        std::cout << "Error launching Creator. Error code: " << GetLastError() << std::endl;
        return 1;
    }

    WaitForSingleObject(piCreator.hProcess, INFINITE);

    CloseHandle(piCreator.hProcess);
    CloseHandle(piCreator.hThread);

    DisplayBinaryFile(binFilename);

    std::cout << "\nEnter report file name: ";
    std::cin >> reportFilename;
    std::cout << "Enter hourly rate: ";
    std::cin >> hourlyRate;

    std::string reporterCmdLine = "Reporter.exe " + binFilename + " " + reportFilename + " " + std::to_string(hourlyRate);

    STARTUPINFO siReporter;
    PROCESS_INFORMATION piReporter;

    ZeroMemory(&siReporter, sizeof(STARTUPINFO));
    siReporter.cb = sizeof(siReporter);

    std::cout << "Launching Reporter with parameters: " << reporterCmdLine << std::endl;

    if (!CreateProcess(
        NULL,                   
        &reporterCmdLine[0],      
        NULL,               
        NULL,   
        FALSE,    
        0,    
        NULL, 
        NULL,        
        &siReporter,  
        &piReporter     
    )) {
        std::cout << "Error launching Reporter. Error code: " << GetLastError() << std::endl;
        return 1;
    }

    WaitForSingleObject(piReporter.hProcess, INFINITE);

    CloseHandle(piReporter.hProcess);
    CloseHandle(piReporter.hThread);

    DisplayTextFile(reportFilename);

    std::cout << "\nOperation completed. Press Enter to exit...";
    std::cin.ignore();
    std::cin.get();

    return 0;
}