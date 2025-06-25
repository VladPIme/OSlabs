#include <iostream>
#include <string>
#include <vector>
#include <Windows.h>
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

    // Создание и отображение файла
    if (!server.createEmployeeFile(filename, employees)) {
        std::cerr << "Failed to create employee file\n";
        return 1;
    }

    server.displayFile(filename);

    int numClients;
    std::cout << "Enter the number of client processes to start: ";
    std::cin >> numClients;

    // Сохраняем имя файла в переменную окружения для передачи клиенту
    SetEnvironmentVariable("EMPLOYEE_FILE", filename.c_str());

    // Запуск клиентских процессов в отдельных терминалах
    std::vector<PROCESS_INFORMATION> clientProcesses;

    // Получаем текущую директорию
    char currentDir[MAX_PATH];
    GetCurrentDirectory(MAX_PATH, currentDir);

    // Запускаем клиентов
    for (int i = 0; i < numClients; i++) {
        // Используем полный путь к клиентскому приложению
        std::string clientPath = std::string(currentDir) + "\\threads_client_exe.exe";
        std::string commandLine = "cmd.exe /c start cmd.exe /k \"" + clientPath + "\"";

        STARTUPINFO si;
        PROCESS_INFORMATION pi;

        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        ZeroMemory(&pi, sizeof(pi));

        // Создаем новый процесс
        if (CreateProcess(
            NULL,                           // Имя приложения
            const_cast<char*>(commandLine.c_str()), // Командная строка
            NULL,                           // Дескриптор безопасности процесса
            NULL,                           // Дескриптор безопасности потока
            FALSE,                          // Наследовать дескрипторы
            0,                              // Флаги создания
            NULL,                           // Блок переменных окружения
            NULL,                           // Текущий каталог
            &si,                            // Информация о запуске
            &pi                             // Информация о процессе
        )) {
            clientProcesses.push_back(pi);
            std::cout << "Started client process " << i + 1 << std::endl;
        }
        else {
            std::cerr << "Failed to start client process " << i + 1 << ". Error: " << GetLastError() << std::endl;
        }
    }

    // Запуск сервера
    server.startServer(filename, numClients);

    std::cout << "All client processes started. Server is running..." << std::endl;

    // Обрабатываем запросы клиентов
    server.handleClientRequests();

    // Ожидаем нажатия Enter для выхода
    std::cout << "Press Enter to terminate client processes and exit...";
    std::cin.ignore();
    std::cin.get();

    // Завершаем все клиентские процессы
    for (auto& pi : clientProcesses) {
        TerminateProcess(pi.hProcess, 0);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }

    // После завершения всех клиентов, отображение модифицированного файла
    server.displayModifiedFile(filename);

    return 0;
}