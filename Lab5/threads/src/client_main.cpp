#include <iostream>
#include <string>
#include <Windows.h>
#include "client.h"
#include "server.h"

int main() {
    // Получаем имя файла из переменной окружения
    char filenameBuf[MAX_PATH];
    GetEnvironmentVariable("EMPLOYEE_FILE", filenameBuf, MAX_PATH);
    std::string filename = filenameBuf;

    std::cout << "Client process started. Working with file: " << filename << std::endl;

    // Создаем объект сервера и запускаем его
    Server server;
    server.startServer(filename, 1); // Указываем, что работаем только с одним клиентом

    // Создаем клиента и связываем его с сервером
    Client client(server);

    // Запускаем клиентский цикл
    client.run();

    std::cout << "Client process terminating." << std::endl;
    return 0;
}