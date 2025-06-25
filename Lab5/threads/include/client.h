#ifndef THREADS_CLIENT_H
#define THREADS_CLIENT_H

#include <string>
#include "employee.h"

// ��������������� ���������� ������ Server
class Server;

class Client {
public:
    Client(Server& server);
    ~Client();

    // ������ ����� �������
    void run();

    // ������ ������ �� ID
    bool readRecord(int id, Employee& employee);

    // ����������� ������ �� ID
    bool modifyRecord(int id, const Employee& employee);

    // ���������� ������� � ������
    bool releaseRecord(int id);

private:
    Server& m_server;
};

#endif // THREADS_CLIENT_H