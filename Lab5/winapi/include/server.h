#ifndef WINAPI_SERVER_H
#define WINAPI_SERVER_H

#include <windows.h>
#include <string>
#include <vector>
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

    // ��� ������������
    bool readEmployee(int id, Employee& emp);
    bool modifyEmployee(int id, const Employee& emp);
    bool releaseEmployee(int id);

private:
    std::string m_pipeName;
    HANDLE m_pipe;
    std::string m_filename;

    // ������� �������������
    std::vector<HANDLE> m_readMutexes;    // �������� ��� �������� ������
    std::vector<HANDLE> m_writeMutexes;   // �������� ��� �������� ������
    int m_numRecords;

    // �������� ���������� ID ����������
    bool isValidEmployeeId(int id);

    // �������� ���������� ������� � �����
    int getNumRecords(const std::string& filename);
};

#endif // WINAPI_SERVER_H