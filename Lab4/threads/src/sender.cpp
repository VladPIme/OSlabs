#include "../include/message_queue.h"
#include <iostream>
#include <string>
#include <windows.h>
#include <thread>
#include <chrono>

const std::wstring READY_SEMAPHORE_NAME = L"SenderReadySemaphore";

int main(int argc, char* argv[]) {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    if (argc < 2) {
        std::cerr << "Usage: sender_threads <file_name>" << std::endl;
        return 1;
    }

    std::string fileName = argv[1];

    // ƒобавл€ем паузу перед инициализацией
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    MessageQueue messageQueue(fileName, 0);
    if (!messageQueue.Initialize(false)) {
        std::cerr << "Failed to initialize message queue" << std::endl;
        return 1;
    }

    HANDLE readySemaphore = OpenSemaphoreW(SEMAPHORE_MODIFY_STATE, FALSE, READY_SEMAPHORE_NAME.c_str());
    if (!readySemaphore) {
        std::cerr << "Failed to open ready semaphore" << std::endl;
        return 1;
    }

    ReleaseSemaphore(readySemaphore, 1, NULL);
    CloseHandle(readySemaphore);

    std::cout << "Sender is ready." << std::endl;

    bool running = true;
    std::string command;

    while (running) {
        std::cout << "\nEnter command (s - send message, q - quit): ";
        std::getline(std::cin, command);

        if (command == "s" || command == "S") {
            std::string message;
            std::cout << "Enter message (max " << MAX_MESSAGE_LENGTH << " characters): ";
            std::getline(std::cin, message);

            if (message.length() > MAX_MESSAGE_LENGTH) {
                message = message.substr(0, MAX_MESSAGE_LENGTH);
                std::cout << "Message truncated to " << MAX_MESSAGE_LENGTH << " characters." << std::endl;
            }

            std::cout << "Sending message..." << std::endl;
            if (messageQueue.Write(message.c_str())) {
                std::cout << "Message sent successfully." << std::endl;
            }
            else {
                std::cerr << "Failed to send message" << std::endl;
            }
        }
        else if (command == "q" || command == "Q") {
            running = false;
        }
        else {
            std::cout << "Unknown command: " << command << std::endl;
        }
    }

    return 0;
}