#include <catch2/catch_test_macros.hpp>
#include <fstream>
#include <string>
#include <vector>
#include <cstring>
#include <thread>
#include <mutex>
#include "employee.h"
#include "server.h"
#include "client.h"

// ������������ filesystem ������ ���� ���������� ������������
#if ((defined(_MSVC_LANG) && _MSVC_LANG >= 201703L) || __cplusplus >= 201703L)
#if defined(__has_include)
#if __has_include(<filesystem>)
#include <filesystem>
namespace fs = std::filesystem;
#define HAS_FILESYSTEM 1
#elif __has_include(<experimental/filesystem>)
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#define HAS_FILESYSTEM 1
#endif
#endif
#endif

#ifndef HAS_FILESYSTEM
#include <cstdio> // ��� remove
#define HAS_FILESYSTEM 0
#endif

// ��������������� ������� ��� �������� ���������� ��������� �����
std::string createTestFile(const std::vector<Employee>& employees) {
    std::string testFilename = "test_employees_threads.dat";
    std::ofstream file(testFilename, std::ios::binary);

    for (const auto& emp : employees) {
        file.write(reinterpret_cast<const char*>(&emp), sizeof(Employee));
    }

    file.close();
    return testFilename;
}

// ��������������� ������� ��� �������� �����
void removeFile(const std::string& filename) {
#if HAS_FILESYSTEM
    fs::remove(filename);
#else
    std::remove(filename.c_str());
#endif
}

// ��������������� ������� ��� �������� ������������� �����
bool fileExists(const std::string& filename) {
#if HAS_FILESYSTEM
    return fs::exists(filename);
#else
    std::ifstream file(filename);
    return file.good();
#endif
}

// ��������������� ������� ��� ������ ���������� �� �����
Employee readEmployeeFromFile(const std::string& filename, int id) {
    std::ifstream file(filename, std::ios::binary);
    Employee emp;

    file.seekg(id * sizeof(Employee));
    file.read(reinterpret_cast<char*>(&emp), sizeof(Employee));
    file.close();

    return emp;
}

TEST_CASE("Threads Server file operations", "[threads_server]") {
    Server server;
    std::vector<Employee> testEmployees = {
        {1, "John", 40.0},
        {2, "Jane", 35.5},
        {3, "Bob", 42.5}
    };

    SECTION("Create employee file") {
        std::string testFile = "test_create_threads.dat";
        bool result = server.createEmployeeFile(testFile, testEmployees);

        REQUIRE(result);
        REQUIRE(fileExists(testFile));

        // �������� ����������� �����
        std::ifstream file(testFile, std::ios::binary);
        Employee emp;
        int count = 0;

        while (file.read(reinterpret_cast<char*>(&emp), sizeof(Employee))) {
            REQUIRE(emp.num == testEmployees[count].num);
            REQUIRE(std::strcmp(emp.name, testEmployees[count].name) == 0);
            REQUIRE(emp.hours == testEmployees[count].hours);
            count++;
        }

        REQUIRE(count == testEmployees.size());
        file.close();

        // ������� ��������� �����
        removeFile(testFile);
    }
}

TEST_CASE("Threads Server read/write operations", "[threads_server]") {
    Server server;
    std::vector<Employee> testEmployees = {
        {1, "John", 40.0},
        {2, "Jane", 35.5},
        {3, "Bob", 42.5}
    };

    std::string testFile = createTestFile(testEmployees);

    // ������ �������
    server.startServer(testFile, 1);

    SECTION("Read employee record") {
        Employee emp;
        bool result = server.readEmployee(1, emp);

        REQUIRE(result);
        REQUIRE(emp.num == testEmployees[1].num);
        REQUIRE(std::strcmp(emp.name, testEmployees[1].name) == 0);
        REQUIRE(emp.hours == testEmployees[1].hours);

        // ������������ ������
        server.releaseEmployee(1);
    }

    SECTION("Modify employee record") {
        // ����������� ������
        Employee newEmp = { 2, "Janet", 45.0 };
        bool result = server.modifyEmployee(1, newEmp);

        REQUIRE(result);

        // ������������ ������
        server.releaseEmployee(1);

        // �������� ����������� ����� ������ �����
        Employee modifiedEmp = readEmployeeFromFile(testFile, 1);
        REQUIRE(modifiedEmp.num == newEmp.num);
        REQUIRE(std::strcmp(modifiedEmp.name, newEmp.name) == 0);
        REQUIRE(modifiedEmp.hours == newEmp.hours);
    }

    // ������� ��������� �����
    removeFile(testFile);
}

TEST_CASE("Threads Client operations", "[threads_client]") {
    Server server;
    std::vector<Employee> testEmployees = {
        {1, "John", 40.0},
        {2, "Jane", 35.5},
        {3, "Bob", 42.5}
    };

    std::string testFile = createTestFile(testEmployees);

    // ������ �������
    server.startServer(testFile, 1);

    // �������� �������
    Client client(server);

    SECTION("Client read operation") {
        Employee emp;
        bool result = client.readRecord(1, emp);

        REQUIRE(result);
        REQUIRE(emp.num == testEmployees[1].num);
        REQUIRE(std::strcmp(emp.name, testEmployees[1].name) == 0);
        REQUIRE(emp.hours == testEmployees[1].hours);

        // ������������ ������
        client.releaseRecord(1);
    }

    SECTION("Client modify operation") {
        // ����������� ������
        Employee newEmp = { 2, "Janet", 45.0 };
        bool result = client.modifyRecord(1, newEmp);

        REQUIRE(result);

        // ������������ ������
        client.releaseRecord(1);

        // �������� ����������� ����� ������ �����
        Employee modifiedEmp = readEmployeeFromFile(testFile, 1);
        REQUIRE(modifiedEmp.num == newEmp.num);
        REQUIRE(std::strcmp(modifiedEmp.name, newEmp.name) == 0);
        REQUIRE(modifiedEmp.hours == newEmp.hours);
    }

    // ������� ��������� �����
    removeFile(testFile);
}

// � ���� ����� ������� ������������ ������������� � ��������
TEST_CASE("Thread basic concurrency test", "[threads_server]") {
    Server server;
    std::vector<Employee> testEmployees = {
        {1, "John", 40.0},
        {2, "Jane", 35.5},
        {3, "Bob", 42.5}
    };

    std::string testFile = createTestFile(testEmployees);

    // ������ �������
    server.startServer(testFile, 3);

    SECTION("Multiple operations test") {
        std::mutex resultMutex;
        std::vector<Employee> results(3);

        // ������ ������� ������
        std::thread t1([&server, &results, &resultMutex]() {
            Employee emp;
            server.readEmployee(0, emp);

            {
                std::lock_guard<std::mutex> lock(resultMutex);
                results[0] = emp;
            }
            });

        // ������ ������ �����������
        std::thread t2([&server]() {
            Employee newEmp = { 2, "Modified", 99.9 };
            server.modifyEmployee(1, newEmp);
            });

        // ������ ��� ������ ������ ������
        std::thread t3([&server, &results, &resultMutex]() {
            Employee emp;
            // ��������� �������� ��� ��������, ��� ������ ����� ��� ������� ������
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            server.readEmployee(1, emp);

            {
                std::lock_guard<std::mutex> lock(resultMutex);
                results[1] = emp;
            }
            });

        // ������� ���������� �������
        t1.join();
        t2.join();
        t3.join();

        // �������� ����������� ������� ������
        REQUIRE(results[0].num == testEmployees[0].num);

        // �������� ����������� ������� ������ (����� �����������)
        REQUIRE(results[1].num == 2); // ����� ������������� ����� �����������
        REQUIRE(std::strcmp(results[1].name, "Modified") == 0);
        REQUIRE(results[1].hours == 99.9);
    }

    // ������� ��������� �����
    removeFile(testFile);
}