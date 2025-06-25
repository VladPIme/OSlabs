#ifndef THREADS_SERVER_H
#define THREADS_SERVER_H

#include <string>
#include <vector>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <memory> // для std::unique_ptr
#include "employee.h"

class Server {
public:
    Server();
    ~Server();

    // Удаляем копирующий конструктор и оператор присваивания
    Server(const Server&) = delete;
    Server& operator=(const Server&) = delete;

    // Создать файл с данными о сотрудниках
    bool createEmployeeFile(const std::string& filename, const std::vector<Employee>& employees);

    // Вывести содержимое файла на консоль
    void displayFile(const std::string& filename);

    // Запустить сервер с указанным количеством клиентов
    void startServer(const std::string& filename, int numClients);

    // Обработать запросы клиентов
    void handleClientRequests();

    // Вывести модифицированный файл
    void displayModifiedFile(const std::string& filename);

    // Методы для работы с клиентами
    bool readEmployee(int id, Employee& emp);
    bool modifyEmployee(int id, const Employee& emp);
    bool releaseEmployee(int id);

private:
    std::string m_filename;
    std::vector<std::unique_ptr<std::mutex>> m_mutexes;  // Храним указатели на мьютексы

    // Проверка валидности ID сотрудника
    bool isValidEmployeeId(int id);

    // Получить количество записей в файле
    int getNumRecords(const std::string& filename);
};

#endif // THREADS_SERVER_H