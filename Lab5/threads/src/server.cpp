#include "server.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <thread>
#include <mutex>
#include <memory>

Server::Server() {}

Server::~Server() {}

bool Server::createEmployeeFile(const std::string& filename, const std::vector<Employee>& employees) {
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to create file: " << filename << std::endl;
        return false;
    }

    for (const auto& emp : employees) {
        file.write(reinterpret_cast<const char*>(&emp), sizeof(Employee));
    }

    file.close();
    return true;
}

void Server::displayFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return;
    }

    Employee emp;
    std::cout << "File contents:\n";
    std::cout << "ID\tName\t\tHours\n";
    std::cout << "--------------------------------\n";

    while (file.read(reinterpret_cast<char*>(&emp), sizeof(Employee))) {
        std::cout << emp.num << "\t" << emp.name << "\t\t" << emp.hours << "\n";
    }

    file.close();
}

void Server::startServer(const std::string& filename, int numClients) {
    m_filename = filename;
    int numRecords = getNumRecords(filename);

    // Инициализация мьютексов для каждой записи
    m_mutexes.clear(); // Очищаем предыдущие мьютексы, если были
    m_mutexes.reserve(numRecords); // Резервируем память, чтобы избежать реаллокаций

    for (int i = 0; i < numRecords; ++i) {
        m_mutexes.push_back(std::make_unique<std::mutex>());
    }

    std::cout << "Server started with " << numClients << " clients.\n";

    // Обработка запросов клиентов
    handleClientRequests();
}

void Server::handleClientRequests() {
    // В реальной реализации здесь бы обрабатывались запросы через именованные каналы
    // Для данной реализации на потоках клиент напрямую вызывает методы сервера
    std::cout << "Server ready to handle client requests.\n";
}

bool Server::readEmployee(int id, Employee& emp) {
    if (!isValidEmployeeId(id)) {
        std::cerr << "Invalid employee ID: " << id << std::endl;
        return false;
    }

    // Получение блокировки для чтения
    std::lock_guard<std::mutex> lock(*m_mutexes[id]);

    std::ifstream file(m_filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to open file for reading: " << m_filename << std::endl;
        return false;
    }

    file.seekg(id * sizeof(Employee));
    file.read(reinterpret_cast<char*>(&emp), sizeof(Employee));
    file.close();

    std::cout << "Server: Client reading record " << id << std::endl;
    return true;
}

bool Server::modifyEmployee(int id, const Employee& emp) {
    if (!isValidEmployeeId(id)) {
        std::cerr << "Invalid employee ID: " << id << std::endl;
        return false;
    }

    // Получение блокировки для записи
    std::lock_guard<std::mutex> lock(*m_mutexes[id]);

    std::fstream file(m_filename, std::ios::binary | std::ios::in | std::ios::out);
    if (!file.is_open()) {
        std::cerr << "Failed to open file for writing: " << m_filename << std::endl;
        return false;
    }

    file.seekp(id * sizeof(Employee));
    file.write(reinterpret_cast<const char*>(&emp), sizeof(Employee));
    file.close();

    std::cout << "Server: Client modified record " << id << std::endl;
    return true;
}

bool Server::releaseEmployee(int id) {
    if (!isValidEmployeeId(id)) {
        return false;
    }

    std::cout << "Server: Client released record " << id << std::endl;
    return true;
}

bool Server::isValidEmployeeId(int id) {
    int numRecords = getNumRecords(m_filename);
    return id >= 0 && id < numRecords;
}

int Server::getNumRecords(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        return 0;
    }

    file.seekg(0, std::ios::end);
    int fileSize = static_cast<int>(file.tellg());
    file.close();

    return fileSize / sizeof(Employee);
}

void Server::displayModifiedFile(const std::string& filename) {
    displayFile(filename);
}