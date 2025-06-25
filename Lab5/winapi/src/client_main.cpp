#include <iostream>
#include "client.h"
#include "employee.h"

// Отображение данных о сотруднике
void displayEmployee(const Employee& emp) {
    std::cout << "Employee Data:\n";
    std::cout << "ID: " << emp.num << "\n";
    std::cout << "Name: " << emp.name << "\n";
    std::cout << "Hours: " << emp.hours << "\n";
}

int main() {
    Client client;

    if (!client.connectToServer()) {
        std::cerr << "Failed to connect to server\n";
        return 1;
    }

    std::cout << "Connected to server\n";

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
            if (client.readRecord(id, emp)) {
                displayEmployee(emp);

                std::cout << "Press Enter to finish access...";
                std::cin.ignore();
                std::cin.get();

                client.releaseRecord(id);
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
            if (client.readRecord(id, emp)) {
                displayEmployee(emp);

                std::cout << "Enter new data:\n";

                std::cout << "  Name: ";
                std::cin >> emp.name;

                std::cout << "  Hours: ";
                std::cin >> emp.hours;

                if (client.modifyRecord(id, emp)) {
                    std::cout << "Record modified successfully\n";

                    std::cout << "Press Enter to finish access...";
                    std::cin.ignore();
                    std::cin.get();

                    client.releaseRecord(id);
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
            running = false;
            break;

        default:
            std::cout << "Invalid choice. Try again.\n";
        }
    }

    return 0;
}