#include <windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <iomanip>

struct employee {
    int num; 
    char name[10]; 
    double hours; 
};

struct employee_report {
    int num;
    std::string name;
    double hours;
    double salary;

    employee_report(const employee& emp, double hourlyRate)
        : num(emp.num), name(emp.name), hours(emp.hours), salary(emp.hours* hourlyRate) {}
};

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cout << "Usage: Reporter.exe <binary_file_name> <report_name> <hourly_rate>" << std::endl;
        return 1;
    }

    std::string binFilename = argv[1];
    std::string reportFilename = argv[2];
    double hourlyRate = std::stod(argv[3]);

    std::ifstream binFile(binFilename, std::ios::binary);
    if (!binFile) {
        std::cout << "Error opening file: " << binFilename << std::endl;
        return 1;
    }

    std::vector<employee_report> employees;
    employee emp;

    while (binFile.read(reinterpret_cast<char*>(&emp), sizeof(employee))) {
        employees.push_back(employee_report(emp, hourlyRate));
    }

    binFile.close();

    std::sort(employees.begin(), employees.end(),
        [](const employee_report& a, const employee_report& b) {
            return a.num < b.num;
        });

    std::ofstream reportFile(reportFilename);
    if (!reportFile) {
        std::cout << "Error creating report file: " << reportFilename << std::endl;
        return 1;
    }

    reportFile << "Report for file \"" << binFilename << "\"" << std::endl;
    reportFile << std::endl;
    reportFile << std::left << std::setw(10) << "Num"
        << std::setw(15) << "Name"
        << std::setw(10) << "Hours"
        << "Salary" << std::endl;
    reportFile << "-------------------------------------------" << std::endl;

    for (const auto& emp : employees) {
        reportFile << std::left << std::setw(10) << emp.num
            << std::setw(15) << emp.name
            << std::setw(10) << std::fixed << std::setprecision(2) << emp.hours
            << std::fixed << std::setprecision(2) << emp.salary << std::endl;
    }

    reportFile.close();
    std::cout << "Report successfully created in file: " << reportFilename << std::endl;

    return 0;
}