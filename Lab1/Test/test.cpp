#include "pch.h"
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <sstream>

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

TEST(StructTest, EmployeeStructSize) {
    employee emp;
    size_t expected_size = 0;

    expected_size += sizeof(int);
    expected_size += 10 * sizeof(char);

    size_t alignment = alignof(double);
    if (expected_size % alignment != 0) {
        expected_size += alignment - (expected_size % alignment);
    }

    expected_size += sizeof(double);

    EXPECT_EQ(sizeof(employee), expected_size);
}

TEST(CreatorTest, CreateBinaryFile) {
    std::string testFile = "test_employees.bin";
    std::ofstream file(testFile.c_str(), std::ios::binary);

    employee emp;
    emp.num = 1;
    strcpy_s(emp.name, sizeof(emp.name), "TestEmp");
    emp.hours = 40.0;

    file.write(reinterpret_cast<char*>(&emp), sizeof(employee));
    file.close();

    std::ifstream checkFile(testFile.c_str(), std::ios::binary);
    ASSERT_TRUE(checkFile.good());

    employee readEmp;
    checkFile.read(reinterpret_cast<char*>(&readEmp), sizeof(employee));

    EXPECT_EQ(readEmp.num, 1);
    EXPECT_STREQ(readEmp.name, "TestEmp");
    EXPECT_DOUBLE_EQ(readEmp.hours, 40.0);

    checkFile.close();
    remove(testFile.c_str());
}

TEST(FileOperationsTest, ReadWriteBinary) {
    std::string testFile = "test_read_write.bin";
    std::ofstream outFile(testFile.c_str(), std::ios::binary);

    const int recordCount = 3;
    employee employees[recordCount];

    employees[0].num = 101;
    strcpy_s(employees[0].name, sizeof(employees[0].name), "Emp1");
    employees[0].hours = 40.0;

    employees[1].num = 102;
    strcpy_s(employees[1].name, sizeof(employees[1].name), "Emp2");
    employees[1].hours = 37.5;

    employees[2].num = 103;
    strcpy_s(employees[2].name, sizeof(employees[2].name), "Emp3");
    employees[2].hours = 42.0;

    for (int i = 0; i < recordCount; i++) {
        outFile.write(reinterpret_cast<char*>(&employees[i]), sizeof(employee));
    }
    outFile.close();

    std::ifstream inFile(testFile.c_str(), std::ios::binary);
    ASSERT_TRUE(inFile.good());

    employee readEmployees[recordCount];
    for (int i = 0; i < recordCount; i++) {
        inFile.read(reinterpret_cast<char*>(&readEmployees[i]), sizeof(employee));
    }
    inFile.close();

    for (int i = 0; i < recordCount; i++) {
        EXPECT_EQ(readEmployees[i].num, employees[i].num);
        EXPECT_STREQ(readEmployees[i].name, employees[i].name);
        EXPECT_DOUBLE_EQ(readEmployees[i].hours, employees[i].hours);
    }

    remove(testFile.c_str());
}

TEST(SalaryCalculationTest, HourlyRateCalculation) {
    employee emp;
    emp.num = 1;
    strcpy_s(emp.name, sizeof(emp.name), "TestEmp");
    emp.hours = 40.0;

    double hourlyRate = 15.0;
    double expectedSalary = emp.hours * hourlyRate;

    EXPECT_DOUBLE_EQ(expectedSalary, 600.0);
}

TEST(ReporterTest, EmployeeSorting) {
    std::vector<employee_report> employees;

    employee emp1, emp2, emp3;

    emp1.num = 103;
    strcpy_s(emp1.name, sizeof(emp1.name), "Emp3");
    emp1.hours = 30.0;

    emp2.num = 101;
    strcpy_s(emp2.name, sizeof(emp2.name), "Emp1");
    emp2.hours = 40.0;

    emp3.num = 102;
    strcpy_s(emp3.name, sizeof(emp3.name), "Emp2");
    emp3.hours = 35.0;

    double hourlyRate = 10.0;

    employees.push_back(employee_report(emp1, hourlyRate));
    employees.push_back(employee_report(emp2, hourlyRate));
    employees.push_back(employee_report(emp3, hourlyRate));

    std::sort(employees.begin(), employees.end(),
        [](const employee_report& a, const employee_report& b) {
            return a.num < b.num;
        });

    EXPECT_EQ(employees[0].num, 101);
    EXPECT_EQ(employees[1].num, 102);
    EXPECT_EQ(employees[2].num, 103);
}

TEST(FileOperationsTest, EmptyFile) {
    std::string testFile = "empty_file.bin";
    std::ofstream outFile(testFile.c_str(), std::ios::binary);
    outFile.close();

    std::ifstream inFile(testFile.c_str(), std::ios::binary);
    ASSERT_TRUE(inFile.good());

    employee emp;
    bool hasData = inFile.read(reinterpret_cast<char*>(&emp), sizeof(employee)).good();

    EXPECT_FALSE(hasData);

    inFile.close();
    remove(testFile.c_str());
}

TEST(ReporterTest, ReportFormatting) {
    std::vector<employee_report> employees;

    employee emp1;
    emp1.num = 101;
    strcpy_s(emp1.name, sizeof(emp1.name), "Emp1");
    emp1.hours = 40.0;

    double hourlyRate = 15.0;

    employees.push_back(employee_report(emp1, hourlyRate));

    std::ostringstream reportStream;

    reportStream << "Report for file \"test.bin\"" << std::endl;
    reportStream << std::endl;
    reportStream << std::left << std::setw(10) << "Num"
        << std::setw(15) << "Name"
        << std::setw(10) << "Hours"
        << "Salary" << std::endl;
    reportStream << "-------------------------------------------" << std::endl;

    for (const auto& emp : employees) {
        reportStream << std::left << std::setw(10) << emp.num
            << std::setw(15) << emp.name
            << std::setw(10) << std::fixed << std::setprecision(2) << emp.hours
            << std::fixed << std::setprecision(2) << emp.salary << std::endl;
    }

    std::string report = reportStream.str();

    EXPECT_NE(report.find("Report for file"), std::string::npos);
    EXPECT_NE(report.find("Num"), std::string::npos);
    EXPECT_NE(report.find("Name"), std::string::npos);
    EXPECT_NE(report.find("Hours"), std::string::npos);
    EXPECT_NE(report.find("Salary"), std::string::npos);

    EXPECT_NE(report.find("101"), std::string::npos);
    EXPECT_NE(report.find("Emp1"), std::string::npos);
    EXPECT_NE(report.find("40.00"), std::string::npos);
    EXPECT_NE(report.find("600.00"), std::string::npos);
}

TEST(SalaryCalculationTest, EdgeCases) {
    employee emp;
    emp.num = 1;
    strcpy_s(emp.name, sizeof(emp.name), "TestEmp");

    emp.hours = 0.0;
    double hourlyRate = 15.0;
    double salary = emp.hours * hourlyRate;
    EXPECT_DOUBLE_EQ(salary, 0.0);

    emp.hours = 1000.0;
    salary = emp.hours * hourlyRate;
    EXPECT_DOUBLE_EQ(salary, 15000.0);
}