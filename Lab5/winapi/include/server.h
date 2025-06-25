#ifndef WINAPI_SERVER_H
#define WINAPI_SERVER_H

#include <windows.h>
#include <string>
#include <vector>
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

    // Для тестирования
    bool readEmployee(int id, Employee& emp);
    bool modifyEmployee(int id, const Employee& emp);
    bool releaseEmployee(int id);

private:
    std::string m_pipeName;
    HANDLE m_pipe;
    std::string m_filename;

    // Объекты синхронизации
    std::vector<HANDLE> m_readMutexes;    // Мьютексы для операций чтения
    std::vector<HANDLE> m_writeMutexes;   // Мьютексы для операций записи
    int m_numRecords;

    // Проверка валидности ID сотрудника
    bool isValidEmployeeId(int id);

    // Получить количество записей в файле
    int getNumRecords(const std::string& filename);
};

#endif // WINAPI_SERVER_H