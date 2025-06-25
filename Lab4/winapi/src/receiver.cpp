#include "../include/message_queue.h"
#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <chrono>

int main() {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    std::wstring fileName;
    int queueSize;
    int senderCount;

    std::wcout << L"Enter binary file name: ";
    std::getline(std::wcin, fileName);

    std::wcout << L"Enter number of records in the file: ";
    std::wcin >> queueSize;
    std::wcin.ignore();

    MessageQueue messageQueue(fileName, queueSize);
    if (!messageQueue.Initialize(true)) {
        std::cerr << "Failed to initialize message queue" << std::endl;
        return 1;
    }

    std::wcout << L"Enter number of Sender processes: ";
    std::wcin >> senderCount;
    std::wcin.ignore();

    if (senderCount <= 0 || senderCount > MAX_SENDERS) {
        std::cerr << "Invalid number of Sender processes" << std::endl;
        return 1;
    }

    std::vector<HANDLE> readyEvents(senderCount);
    for (int i = 0; i < senderCount; ++i) {
        std::wstring eventName = READY_EVENT_PREFIX + std::to_wstring(i);
        readyEvents[i] = CreateEventW(NULL, TRUE, FALSE, eventName.c_str());

        if (!readyEvents[i]) {
            std::cerr << "Failed to create ready event " << i << std::endl;
            return 1;
        }
    }

    // Запускаем процессы через CMD с title и правильным запуском в новой консоли
    std::vector<PROCESS_INFORMATION> senderProcesses(senderCount);
    for (int i = 0; i < senderCount; ++i) {
        STARTUPINFOW si = { sizeof(STARTUPINFOW) };
        ZeroMemory(&si, sizeof(si));
        ZeroMemory(&senderProcesses[i], sizeof(PROCESS_INFORMATION));

        // Создаем команду для CMD, которая запустит sender с нужным заголовком
        std::wstring cmdCommand = L"cmd.exe /c start \"Sender " + std::to_wstring(i) +
            L"\" /wait " + L"sender.exe " + fileName + L" " + std::to_wstring(i);

        wchar_t* cmdCommandCopy = new wchar_t[cmdCommand.length() + 1];
        wcscpy_s(cmdCommandCopy, cmdCommand.length() + 1, cmdCommand.c_str());

        if (!CreateProcessW(
            NULL,
            cmdCommandCopy,
            NULL,
            NULL,
            FALSE,
            0, // Не нужен CREATE_NEW_CONSOLE, так как cmd /c start создаст новую консоль
            NULL,
            NULL,
            &si,
            &senderProcesses[i]
        )) {
            std::cerr << "Failed to create Sender process " << i << std::endl;
            delete[] cmdCommandCopy;
            return 1;
        }

        delete[] cmdCommandCopy;

        // Пауза между запусками процессов
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    std::wcout << L"Waiting for all Sender processes to be ready..." << std::endl;
    WaitForMultipleObjects(static_cast<DWORD>(readyEvents.size()), readyEvents.data(), TRUE, INFINITE);
    std::wcout << L"All Sender processes are ready." << std::endl;

    for (auto& event : readyEvents) {
        CloseHandle(event);
    }

    bool running = true;
    std::string command;

    while (running) {
        std::cout << "\nEnter command (r - read message, q - quit): ";
        std::getline(std::cin, command);

        if (command == "r" || command == "R") {
            char message[MAX_MESSAGE_LENGTH + 1] = { 0 };

            std::cout << "Waiting for message..." << std::endl;
            if (messageQueue.Read(message, MAX_MESSAGE_LENGTH)) {
                std::cout << "Received message: " << message << std::endl;
            }
            else {
                std::cerr << "Failed to read message" << std::endl;
            }
        }
        else if (command == "q" || command == "Q") {
            running = false;
        }
        else {
            std::cout << "Unknown command: " << command << std::endl;
        }
    }

    std::wcout << L"Waiting for Sender processes to terminate..." << std::endl;

    for (auto& process : senderProcesses) {
        CloseHandle(process.hProcess);
        CloseHandle(process.hThread);
    }

    return 0;
}