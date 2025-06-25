#include "../include/message_queue.h"
#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <windows.h>

const std::wstring READY_SEMAPHORE_NAME = L"SenderReadySemaphore";

int main() {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    std::string fileName;
    int queueSize;
    int senderCount;

    std::cout << "Enter binary file name: ";
    std::getline(std::cin, fileName);

    std::cout << "Enter number of records in the file: ";
    std::cin >> queueSize;
    std::cin.ignore();

    MessageQueue messageQueue(fileName, queueSize);
    if (!messageQueue.Initialize(true)) {
        std::cerr << "Failed to initialize message queue" << std::endl;
        return 1;
    }

    std::cout << "Enter number of Sender processes: ";
    std::cin >> senderCount;
    std::cin.ignore();

    if (senderCount <= 0 || senderCount > MAX_SENDERS) {
        std::cerr << "Invalid number of Sender processes" << std::endl;
        return 1;
    }

    HANDLE readySemaphore = CreateSemaphoreW(NULL, 0, senderCount, READY_SEMAPHORE_NAME.c_str());
    if (!readySemaphore) {
        std::cerr << "Failed to create ready semaphore" << std::endl;
        return 1;
    }

    // Запускаем процессы через CMD с title и правильным запуском в новой консоли
    std::vector<PROCESS_INFORMATION> senderProcesses(senderCount);
    for (int i = 0; i < senderCount; ++i) {
        STARTUPINFOW si = { sizeof(STARTUPINFOW) };
        ZeroMemory(&si, sizeof(si));
        ZeroMemory(&senderProcesses[i], sizeof(PROCESS_INFORMATION));

        // Преобразуем string в wstring
        std::wstring wFileName(fileName.begin(), fileName.end());

        // Создаем команду для CMD, которая запустит sender_threads с нужным заголовком
        std::wstring cmdCommand = L"cmd.exe /c start \"Sender Threads " + std::to_wstring(i) +
            L"\" /wait " + L"sender_threads.exe " + wFileName;

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

    std::cout << "Waiting for all Sender processes to be ready..." << std::endl;

    for (int i = 0; i < senderCount; ++i) {
        WaitForSingleObject(readySemaphore, INFINITE);
        std::cout << "Sender " << i + 1 << " of " << senderCount << " is ready." << std::endl;
    }

    std::cout << "All Sender processes are ready." << std::endl;

    CloseHandle(readySemaphore);

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

    std::cout << "Waiting for Sender processes to terminate..." << std::endl;

    for (auto& process : senderProcesses) {
        CloseHandle(process.hProcess);
        CloseHandle(process.hThread);
    }

    return 0;
}