#ifndef WINAPI_CLIENT_H
#define WINAPI_CLIENT_H

#include <windows.h>
#include <string>
#include "employee.h"

class Client {
public:
    Client();
    ~Client();

    // ����������� � �������
    bool connectToServer();

    // ������ ������ �� ID
    bool readRecord(int id, Employee& employee);

    // ����������� ������ �� ID
    bool modifyRecord(int id, const Employee& employee);

    // ���������� ������� � ������
    bool releaseRecord(int id);

private:
    HANDLE m_pipe;
    std::string m_pipeName;

    // �������� �������
    bool sendRequest(int requestType, int id);

    // ��������� ������ � ����������
    bool receiveEmployee(Employee& employee);

    // �������� ������ � ����������
    bool sendEmployee(const Employee& employee);
};

#endif // WINAPI_CLIENT_H