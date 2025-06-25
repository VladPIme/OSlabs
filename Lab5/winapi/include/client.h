#ifndef WINAPI_CLIENT_H
#define WINAPI_CLIENT_H

#include <windows.h>
#include <string>
#include "employee.h"

class Client {
public:
    Client();
    ~Client();

    // Подключение к серверу
    bool connectToServer();

    // Чтение записи по ID
    bool readRecord(int id, Employee& employee);

    // Модификация записи по ID
    bool modifyRecord(int id, const Employee& employee);

    // Завершение доступа к записи
    bool releaseRecord(int id);

private:
    HANDLE m_pipe;
    std::string m_pipeName;

    // Отправка запроса
    bool sendRequest(int requestType, int id);

    // Получение данных о сотруднике
    bool receiveEmployee(Employee& employee);

    // Отправка данных о сотруднике
    bool sendEmployee(const Employee& employee);
};

#endif // WINAPI_CLIENT_H