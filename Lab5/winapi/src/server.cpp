#include "server.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <Windows.h>
#include <vector>
#include <thread>

// Константы
const int MAX_INSTANCES = 10;
const int BUFFER_SIZE = 1024;

// Типы сообщений
const int MSG_READ = 1;
const int MSG_MODIFY = 2;
const int MSG_RELEASE = 3;

// Коды состояния
const int STATUS_SUCCESS = 0;
const int STATUS_ERROR = 1;

// Структура сообщения
struct Message {
    int type;     // Тип запроса
    int id;       // ID сотрудника
    Employee emp; // Данные о сотруднике
};

// Структура данных для потока обработки клиента
struct ClientHandlerData {
    HANDLE pipe;
    std::string filename;
    std::vector<HANDLE>& readMutexes;
    std::vector<HANDLE>& writeMutexes;
    int numRecords;

    ClientHandlerData(HANDLE p, const std::string& fn, std::vector<HANDLE>& rm, std::vector<HANDLE>& wm, int nr)
        : pipe(p), filename(fn), readMutexes(rm), writeMutexes(wm), numRecords(nr) {}
};

// Функция потока для обработки клиентских запросов
DWORD WINAPI ClientHandlerThread(LPVOID lpParam) {
    ClientHandlerData* data = static_cast<ClientHandlerData*>(lpParam);
    HANDLE pipe = data->pipe;
    std::string filename = data->filename;
    std::vector<HANDLE>& readMutexes = data->readMutexes;
    std::vector<HANDLE>& writeMutexes = data->writeMutexes;
    int numRecords = data->numRecords;

    BOOL connected = ConnectNamedPipe(pipe, NULL) ? TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);

    if (connected) {
        std::cout << "Client connected to pipe: " << pipe << std::endl;

        DWORD bytesRead, bytesWritten;
        Message msg;

        // Обработка запросов клиента
        while (ReadFile(pipe, &msg, sizeof(Message), &bytesRead, NULL) && bytesRead > 0) {
            int status = STATUS_SUCCESS;

            // Проверка валидности ID сотрудника
            if (msg.id < 0 || msg.id >= numRecords) {
                status = STATUS_ERROR;
                WriteFile(pipe, &status, sizeof(int), &bytesWritten, NULL);
                continue;
            }

            Employee emp;
            std::fstream file(filename, std::ios::binary | std::ios::in | std::ios::out);

            switch (msg.type) {
            case MSG_READ: {
                // Получение блокировки на чтение
                WaitForSingleObject(readMutexes[msg.id], INFINITE);

                // Чтение записи сотрудника
                file.seekg(msg.id * sizeof(Employee));
                file.read(reinterpret_cast<char*>(&emp), sizeof(Employee));

                // Отправка статуса и данных о сотруднике клиенту
                WriteFile(pipe, &status, sizeof(int), &bytesWritten, NULL);
                WriteFile(pipe, &emp, sizeof(Employee), &bytesWritten, NULL);

                std::cout << "Client reading record: " << msg.id << std::endl;
                break;
            }

            case MSG_MODIFY: {
                // Получение блокировки на запись
                WaitForSingleObject(writeMutexes[msg.id], INFINITE);

                // Чтение записи сотрудника
                file.seekg(msg.id * sizeof(Employee));
                file.read(reinterpret_cast<char*>(&emp), sizeof(Employee));

                // Отправка статуса и данных о сотруднике клиенту
                WriteFile(pipe, &status, sizeof(int), &bytesWritten, NULL);
                WriteFile(pipe, &emp, sizeof(Employee), &bytesWritten, NULL);

                // Получение модифицированных данных о сотруднике
                if (ReadFile(pipe, &emp, sizeof(Employee), &bytesRead, NULL)) {
                    // Запись модифицированных данных в файл
                    file.seekp(msg.id * sizeof(Employee));
                    file.write(reinterpret_cast<char*>(&emp), sizeof(Employee));
                    std::cout << "Client modified record: " << msg.id << std::endl;
                }

                break;
            }

            case MSG_RELEASE: {
                // Освобождение блокировок
                ReleaseMutex(readMutexes[msg.id]);
                ReleaseMutex(writeMutexes[msg.id]);
                std::cout << "Client released record: " << msg.id << std::endl;
                break;
            }
            }

            file.close();
        }

        DisconnectNamedPipe(pipe);
        std::cout << "Client disconnected from pipe: " << pipe << std::endl;
    }

    // Закрываем канал
    CloseHandle(pipe);

    // Освобождаем память для структуры данных
    delete data;

    return 0;
}

Server::Server() : m_pipe(INVALID_HANDLE_VALUE), m_numRecords(0) {
    m_pipeName = "\\\\.\\pipe\\employee_pipe";
}

Server::~Server() {
    // Очистка мьютексов
    for (auto& handle : m_readMutexes) {
        if (handle != NULL) {
            CloseHandle(handle);
        }
    }

    for (auto& handle : m_writeMutexes) {
        if (handle != NULL) {
            CloseHandle(handle);
        }
    }
}

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
    m_numRecords = getNumRecords(filename);

    // Очистка и инициализация мьютексов
    m_readMutexes.clear();
    m_writeMutexes.clear();
    m_readMutexes.reserve(m_numRecords);
    m_writeMutexes.reserve(m_numRecords);

    for (int i = 0; i < m_numRecords; i++) {
        std::string mutexName = "ReadMutex_" + std::to_string(i);
        HANDLE readMutex = CreateMutex(NULL, FALSE, mutexName.c_str());

        mutexName = "WriteMutex_" + std::to_string(i);
        HANDLE writeMutex = CreateMutex(NULL, FALSE, mutexName.c_str());

        if (readMutex == NULL || writeMutex == NULL) {
            std::cerr << "Failed to create mutexes\n";
            return;
        }

        m_readMutexes.push_back(readMutex);
        m_writeMutexes.push_back(writeMutex);
    }

    std::cout << "Server started. Waiting for client connections...\n";
}

void Server::handleClientRequests() {
    std::vector<HANDLE> threadHandles;

    // Создаем и запускаем отдельные потоки для обслуживания клиентов
    for (int i = 0; i < MAX_INSTANCES; i++) {
        // Создание именованного канала
        HANDLE pipe = CreateNamedPipe(
            m_pipeName.c_str(),
            PIPE_ACCESS_DUPLEX,
            PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
            MAX_INSTANCES,
            BUFFER_SIZE,
            BUFFER_SIZE,
            0,
            NULL
        );

        if (pipe == INVALID_HANDLE_VALUE) {
            std::cerr << "Failed to create named pipe instance " << i << ". Error: " << GetLastError() << std::endl;
            continue;
        }

        // Создаем структуру данных для передачи потоку
        ClientHandlerData* data = new ClientHandlerData(pipe, m_filename, m_readMutexes, m_writeMutexes, m_numRecords);

        // Создаем поток для обработки клиентских запросов
        HANDLE thread = CreateThread(
            NULL,
            0,
            ClientHandlerThread,
            data,
            0,
            NULL
        );

        if (thread == NULL) {
            std::cerr << "Failed to create thread for pipe instance " << i << ". Error: " << GetLastError() << std::endl;
            delete data;
            CloseHandle(pipe);
            continue;
        }

        threadHandles.push_back(thread);
    }

    // Дожидаемся завершения всех потоков
    if (!threadHandles.empty()) {
        WaitForMultipleObjects(static_cast<DWORD>(threadHandles.size()), threadHandles.data(), TRUE, INFINITE);
    }

    // Закрываем дескрипторы потоков
    for (auto& handle : threadHandles) {
        CloseHandle(handle);
    }
}

void Server::displayModifiedFile(const std::string& filename) {
    displayFile(filename);
}

bool Server::isValidEmployeeId(int id) {
    return id >= 0 && id < m_numRecords;
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

// Методы для тестирования
bool Server::readEmployee(int id, Employee& emp) {
    if (!isValidEmployeeId(id)) {
        return false;
    }

    std::ifstream file(m_filename, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }

    file.seekg(id * sizeof(Employee));
    file.read(reinterpret_cast<char*>(&emp), sizeof(Employee));
    file.close();

    return true;
}

bool Server::modifyEmployee(int id, const Employee& emp) {
    if (!isValidEmployeeId(id)) {
        return false;
    }

    std::fstream file(m_filename, std::ios::binary | std::ios::in | std::ios::out);
    if (!file.is_open()) {
        return false;
    }

    file.seekp(id * sizeof(Employee));
    file.write(reinterpret_cast<const char*>(&emp), sizeof(Employee));
    file.close();

    return true;
}

bool Server::releaseEmployee(int id) {
    return isValidEmployeeId(id);
}