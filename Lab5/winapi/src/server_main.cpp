#include <iostream>
#include <Windows.h>
#include <string>
#include <vector>
#include <process.h>
#include "server.h"
#include "employee.h"

int main() {
    Server server;
    std::string filename;
    int numEmployees;
    std::vector<Employee> employees;

    std::cout << "Enter the filename to create: ";
    std::cin >> filename;

    std::cout << "Enter the number of employees: ";
    std::cin >> numEmployees;

    std::cout << "Enter employee data (ID, Name, Hours):\n";
    for (int i = 0; i < numEmployees; i++) {
        Employee emp;
        std::cout << "Employee " << i + 1 << ":\n";

        std::cout << "  ID: ";
        std::cin >> emp.num;

        std::cout << "  Name: ";
        std::cin >> emp.name;

        std::cout << "  Hours: ";
        std::cin >> emp.hours;

        employees.push_back(emp);
    }

    // �������� � ����������� �����
    if (!server.createEmployeeFile(filename, employees)) {
        std::cerr << "Failed to create employee file\n";
        return 1;
    }

    server.displayFile(filename);

    int numClients;
    std::cout << "Enter the number of client processes to start: ";
    std::cin >> numClients;

    // ������ �������
    server.startServer(filename, numClients);

    // �������� ������� ���������� � ���� � ������������ �����
    char currentDir[MAX_PATH];
    GetCurrentDirectory(MAX_PATH, currentDir);
    std::string clientExe = std::string(currentDir) + "\\winapi_client_exe.exe";

    // ������ ���������� ��������� � ��������� �����
    std::vector<PROCESS_INFORMATION> clientProcesses;

    for (int i = 0; i < numClients; i++) {
        // ������ ������� ��� �������
        std::string commandLine = "cmd.exe /c start cmd.exe /k \"" + clientExe + "\"";

        STARTUPINFO si;
        PROCESS_INFORMATION pi;

        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        ZeroMemory(&pi, sizeof(pi));

        if (CreateProcess(
            NULL,                           // ��� ����������
            const_cast<char*>(commandLine.c_str()), // ��������� ������
            NULL,                           // ���������� ������������ ��������
            NULL,                           // ���������� ������������ ������
            FALSE,                          // ����������� �����������
            0,                              // ����� ��������
            NULL,                           // ���� ���������� ���������
            NULL,                           // ������� �������
            &si,                            // ���������� � �������
            &pi                             // ���������� � ��������
        )) {
            clientProcesses.push_back(pi);
            std::cout << "Started client process " << i + 1 << std::endl;
        }
        else {
            std::cerr << "Failed to start client process " << i + 1 << ". Error: " << GetLastError() << std::endl;
        }
    }

    std::cout << "All client processes started. Server is running..." << std::endl;

    // ������������ ������� �������� � ��������� �������
    server.handleClientRequests();

    std::cout << "Press Enter to terminate client processes...";
    std::cin.ignore();
    std::cin.get();

    // ��������� ��� ���������� ��������
    for (auto& pi : clientProcesses) {
        TerminateProcess(pi.hProcess, 0);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }

    // ����� ���������� ���� ��������, ����������� ����������������� �����
    server.displayModifiedFile(filename);

    std::cout << "Press Enter to exit...";
    std::cin.get();

    return 0;
}