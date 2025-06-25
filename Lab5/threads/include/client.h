#ifndef THREADS_CLIENT_H
#define THREADS_CLIENT_H

#include <string>
#include "employee.h"

// Предварительное объявление класса Server
class Server;

class Client {
public:
    Client(Server& server);
    ~Client();

    // Запуск цикла клиента
    void run();

    // Чтение записи по ID
    bool readRecord(int id, Employee& employee);

    // Модификация записи по ID
    bool modifyRecord(int id, const Employee& employee);

    // Завершение доступа к записи
    bool releaseRecord(int id);

private:
    Server& m_server;
};

#endif // THREADS_CLIENT_H