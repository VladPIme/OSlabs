#include "../include/message_queue.h"
#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <chrono>

int main(int argc, char* argv[]) {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    if (argc < 3) {
        std::cerr << "Usage: sender.exe <file_name> <sender_id>" << std::endl;
        return 1;
    }

    std::wstring fileName;
    int senderId;

    {
        int wideSize = MultiByteToWideChar(CP_UTF8, 0, argv[1], -1, NULL, 0);
        if (wideSize > 0) {
            std::vector<wchar_t> buffer(wideSize);
            MultiByteToWideChar(CP_UTF8, 0, argv[1], -1, buffer.data(), wideSize);
            fileName = buffer.data();
        }
    }

    senderId = std::stoi(argv[2]);

    // ƒобавл€ем паузу перед инициализацией
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    MessageQueue messageQueue(fileName, 0);
    if (!messageQueue.Initialize(false)) {
        std::cerr << "Failed to initialize message queue" << std::endl;
        return 1;
    }

    std::wstring readyEventName = READY_EVENT_PREFIX + std::to_wstring(senderId);
    HANDLE readyEvent = OpenEventW(EVENT_MODIFY_STATE, FALSE, readyEventName.c_str());

    if (!readyEvent) {
        std::cerr << "Failed to open ready event" << std::endl;
        return 1;
    }

    SetEvent(readyEvent);
    CloseHandle(readyEvent);

    std::cout << "Sender " << senderId << " is ready." << std::endl;

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