#include <iostream>
#include <string>
#include <Windows.h>
#include "client.h"
#include "server.h"

int main() {
    // �������� ��� ����� �� ���������� ���������
    char filenameBuf[MAX_PATH];
    GetEnvironmentVariable("EMPLOYEE_FILE", filenameBuf, MAX_PATH);
    std::string filename = filenameBuf;

    std::cout << "Client process started. Working with file: " << filename << std::endl;

    // ������� ������ ������� � ��������� ���
    Server server;
    server.startServer(filename, 1); // ���������, ��� �������� ������ � ����� ��������

    // ������� ������� � ��������� ��� � ��������
    Client client(server);

    // ��������� ���������� ����
    client.run();

    std::cout << "Client process terminating." << std::endl;
    return 0;
}