#ifndef THREADS_SERVER_H
#define THREADS_SERVER_H

#include <string>
#include <vector>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <memory> // ��� std::unique_ptr
#include "employee.h"

class Server {
public:
    Server();
    ~Server();

    // ������� ���������� ����������� � �������� ������������
    Server(const Server&) = delete;
    Server& operator=(const Server&) = delete;

    // ������� ���� � ������� � �����������
    bool createEmployeeFile(const std::string& filename, const std::vector<Employee>& employees);

    // ������� ���������� ����� �� �������
    void displayFile(const std::string& filename);

    // ��������� ������ � ��������� ����������� ��������
    void startServer(const std::string& filename, int numClients);

    // ���������� ������� ��������
    void handleClientRequests();

    // ������� ���������������� ����
    void displayModifiedFile(const std::string& filename);

    // ������ ��� ������ � ���������
    bool readEmployee(int id, Employee& emp);
    bool modifyEmployee(int id, const Employee& emp);
    bool releaseEmployee(int id);

private:
    std::string m_filename;
    std::vector<std::unique_ptr<std::mutex>> m_mutexes;  // ������ ��������� �� ��������

    // �������� ���������� ID ����������
    bool isValidEmployeeId(int id);

    // �������� ���������� ������� � �����
    int getNumRecords(const std::string& filename);
};

#endif // THREADS_SERVER_H