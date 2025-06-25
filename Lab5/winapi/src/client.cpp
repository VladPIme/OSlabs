#include "client.h"
#include <iostream>
#include <Windows.h>

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

Client::Client() : m_pipe(INVALID_HANDLE_VALUE) {
    m_pipeName = "\\\\.\\pipe\\employee_pipe";
}

Client::~Client() {
    if (m_pipe != INVALID_HANDLE_VALUE) {
        CloseHandle(m_pipe);
    }
}

bool Client::connectToServer() {
    // Подключение к именованному каналу
    while (true) {
        m_pipe = CreateFile(
            m_pipeName.c_str(),
            GENERIC_READ | GENERIC_WRITE,
            0,
            NULL,
            OPEN_EXISTING,
            0,
            NULL
        );

        if (m_pipe != INVALID_HANDLE_VALUE) {
            break;
        }

        if (GetLastError() != ERROR_PIPE_BUSY) {
            std::cerr << "Could not open pipe. Error: " << GetLastError() << std::endl;
            return false;
        }

        // Все экземпляры канала заняты, ждем 20 секунд
        if (!WaitNamedPipe(m_pipeName.c_str(), 20000)) {
            std::cerr << "Could not open pipe: 20 second wait timed out." << std::endl;
            return false;
        }
    }

    // Изменение режима канала на режим сообщений
    DWORD mode = PIPE_READMODE_MESSAGE;
    BOOL success = SetNamedPipeHandleState(
        m_pipe,
        &mode,
        NULL,
        NULL
    );

    if (!success) {
        std::cerr << "SetNamedPipeHandleState failed. Error: " << GetLastError() << std::endl;
        CloseHandle(m_pipe);
        m_pipe = INVALID_HANDLE_VALUE;
        return false;
    }

    return true;
}

bool Client::readRecord(int id, Employee& employee) {
    // Отправляем запрос на чтение и получаем данные
    return sendRequest(MSG_READ, id) && receiveEmployee(employee);
}

bool Client::modifyRecord(int id, const Employee& employee) {
    Employee original;

    // Отправляем запрос на модификацию, получаем текущие данные
    if (!sendRequest(MSG_MODIFY, id) || !receiveEmployee(original)) {
        return false;
    }

    // Отправляем модифицированные данные о сотруднике
    return sendEmployee(employee);
}

bool Client::releaseRecord(int id) {
    // Отправляем запрос на освобождение блокировки
    return sendRequest(MSG_RELEASE, id);
}

bool Client::sendRequest(int requestType, int id) {
    Message msg;
    msg.type = requestType;
    msg.id = id;

    DWORD bytesWritten;
    BOOL success = WriteFile(
        m_pipe,
        &msg,
        sizeof(Message),
        &bytesWritten,
        NULL
    );

    if (!success || bytesWritten != sizeof(Message)) {
        std::cerr << "WriteFile failed. Error: " << GetLastError() << std::endl;
        return false;
    }

    return true;
}

bool Client::receiveEmployee(Employee& employee) {
    DWORD bytesRead;
    int status;

    // Чтение статуса
    BOOL success = ReadFile(
        m_pipe,
        &status,
        sizeof(int),
        &bytesRead,
        NULL
    );

    if (!success || bytesRead != sizeof(int)) {
        std::cerr << "Failed to read status. Error: " << GetLastError() << std::endl;
        return false;
    }

    if (status != STATUS_SUCCESS) {
        std::cerr << "Server returned error status\n";
        return false;
    }

    // Чтение данных о сотруднике
    success = ReadFile(
        m_pipe,
        &employee,
        sizeof(Employee),
        &bytesRead,
        NULL
    );

    if (!success || bytesRead != sizeof(Employee)) {
        std::cerr << "Failed to read employee data. Error: " << GetLastError() << std::endl;
        return false;
    }

    return true;
}

bool Client::sendEmployee(const Employee& employee) {
    DWORD bytesWritten;
    BOOL success = WriteFile(
        m_pipe,
        &employee,
        sizeof(Employee),
        &bytesWritten,
        NULL
    );

    if (!success || bytesWritten != sizeof(Employee)) {
        std::cerr << "Failed to send employee data. Error: " << GetLastError() << std::endl;
        return false;
    }

    return true;
}