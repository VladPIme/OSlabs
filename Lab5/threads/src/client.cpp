#include "client.h"
#include "server.h"
#include <iostream>
#include <string>
#include <thread>
#include <mutex>

Client::Client(Server& server) : m_server(server) {}

Client::~Client() {}

void Client::run() {
    std::cout << "Client thread started\n";

    bool running = true;
    while (running) {
        std::cout << "\nSelect operation:\n";
        std::cout << "1. Read employee record\n";
        std::cout << "2. Modify employee record\n";
        std::cout << "3. Exit\n";
        std::cout << "Choice: ";

        int choice;
        std::cin >> choice;

        switch (choice) {
        case 1: { // Чтение записи
            std::cout << "Enter employee ID to read: ";
            int id;
            std::cin >> id;

            Employee emp;
            if (readRecord(id, emp)) {
                std::cout << "Employee Data:\n";
                std::cout << "ID: " << emp.num << "\n";
                std::cout << "Name: " << emp.name << "\n";
                std::cout << "Hours: " << emp.hours << "\n";

                std::cout << "Press Enter to finish access...";
                std::cin.ignore();
                std::cin.get();

                releaseRecord(id);
            }
            else {
                std::cerr << "Failed to read employee record\n";
            }
            break;
        }

        case 2: { // Модификация записи
            std::cout << "Enter employee ID to modify: ";
            int id;
            std::cin >> id;

            Employee emp;
            if (readRecord(id, emp)) {
                std::cout << "Current Employee Data:\n";
                std::cout << "ID: " << emp.num << "\n";
                std::cout << "Name: " << emp.name << "\n";
                std::cout << "Hours: " << emp.hours << "\n";

                std::cout << "Enter new data:\n";

                std::cout << "  Name: ";
                std::cin >> emp.name;

                std::cout << "  Hours: ";
                std::cin >> emp.hours;

                if (modifyRecord(id, emp)) {
                    std::cout << "Record modified successfully\n";

                    std::cout << "Press Enter to finish access...";
                    std::cin.ignore();
                    std::cin.get();

                    releaseRecord(id);
                }
                else {
                    std::cerr << "Failed to modify record\n";
                }
            }
            else {
                std::cerr << "Failed to read employee record\n";
            }
            break;
        }

        case 3: // Выход
            std::cout << "Exiting client thread...\n";
            running = false;
            break;

        default:
            std::cout << "Invalid choice. Try again.\n";
        }
    }
}

bool Client::readRecord(int id, Employee& employee) {
    return m_server.readEmployee(id, employee);
}

bool Client::modifyRecord(int id, const Employee& employee) {
    return m_server.modifyEmployee(id, employee);
}

bool Client::releaseRecord(int id) {
    return m_server.releaseEmployee(id);
}