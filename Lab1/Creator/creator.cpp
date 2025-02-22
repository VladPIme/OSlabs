#include <windows.h>
#include <iostream>
#include <fstream>
#include <string>

struct employee {
    char name[10];
    int num;
    double hours;
};

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cout << "Usage: Creator.exe <file_name> <record_count>" << std::endl;
        return 1;
    }

    std::string filename = argv[1];
    int recordCount = std::stoi(argv[2]);

    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        std::cout << "Error creating file: " << filename << std::endl;
        return 1;
    }

    std::cout << "Creating binary file: " << filename << std::endl;
    std::cout << "Record count: " << recordCount << std::endl;

    // Input records
    employee emp;
    for (int i = 0; i < recordCount; i++) {
        std::cout << "\nRecord #" << (i + 1) << ":" << std::endl;

        std::cout << "Number: ";
        std::cin >> emp.num;

        std::cout << "Employee name: ";
        std::cin >> emp.name;

        std::cout << "Hours worked: ";
        std::cin >> emp.hours;

        // Write structure to file
        file.write(reinterpret_cast<char*>(&emp), sizeof(employee));
    }

    file.close();
    std::cout << "\nFile " << filename << " successfully created with " << recordCount << " records." << std::endl;

    return 0;
}