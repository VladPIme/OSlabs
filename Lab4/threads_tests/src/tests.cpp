#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include "message_queue.h"
#include <thread>
#include <vector>

TEST_CASE("MessageQueue initialization", "[threads]") {
    const std::string testFileName = "test_queue_threads.bin";
    const int queueSize = 10;

    MessageQueue queue(testFileName, queueSize);
    REQUIRE(queue.Initialize(true));

    REQUIRE(queue.GetCount() == 0);
}

TEST_CASE("MessageQueue write and read", "[threads]") {
    const std::string testFileName = "test_queue_threads.bin";
    const int queueSize = 10;

    MessageQueue queue(testFileName, queueSize);
    REQUIRE(queue.Initialize(true));

    const char* testMessage = "Hello, World!";
    REQUIRE(queue.Write(testMessage));

    REQUIRE(queue.GetCount() == 1);

    char readMessage[MAX_MESSAGE_LENGTH + 1] = { 0 };
    REQUIRE(queue.Read(readMessage, MAX_MESSAGE_LENGTH));

    REQUIRE(strcmp(readMessage, testMessage) == 0);

    REQUIRE(queue.GetCount() == 0);
}

TEST_CASE("MessageQueue concurrent operation", "[threads]") {
    const std::string testFileName = "test_queue_threads.bin";
    const int queueSize = 10;

    MessageQueue queue(testFileName, queueSize);
    REQUIRE(queue.Initialize(true));

    const int numMessages = 5;
    std::vector<std::string> messages = {
        "Message 1",
        "Message 2",
        "Message 3",
        "Message 4",
        "Message 5"
    };

    std::thread writer([&queue, &messages]() {
        for (const auto& msg : messages) {
            REQUIRE(queue.Write(msg.c_str()));
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        });

    std::thread reader([&queue, &messages]() {
        for (size_t i = 0; i < messages.size(); ++i) {
            char readMessage[MAX_MESSAGE_LENGTH + 1] = { 0 };
            REQUIRE(queue.Read(readMessage, MAX_MESSAGE_LENGTH));
            REQUIRE(strcmp(readMessage, messages[i].c_str()) == 0);
        }
        });

    writer.join();
    reader.join();

    REQUIRE(queue.GetCount() == 0);
}

TEST_CASE("MessageQueue full queue behavior", "[threads]") {
    const std::string testFileName = "test_queue_threads.bin";
    const int queueSize = 3;

    MessageQueue queue(testFileName, queueSize);
    REQUIRE(queue.Initialize(true));

    std::vector<std::string> messages = {
        "Message 1",
        "Message 2",
        "Message 3",
        "Message 4",
        "Message 5"
    };

    std::atomic<int> writtenCount(0);
    std::atomic<int> readCount(0);

    std::thread writer([&queue, &messages, &writtenCount]() {
        for (const auto& msg : messages) {
            REQUIRE(queue.Write(msg.c_str()));
            writtenCount++;
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        });

    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    std::thread reader([&queue, &messages, &readCount]() {
        for (size_t i = 0; i < messages.size(); ++i) {
            char readMessage[MAX_MESSAGE_LENGTH + 1] = { 0 };
            REQUIRE(queue.Read(readMessage, MAX_MESSAGE_LENGTH));
            REQUIRE(strcmp(readMessage, messages[i].c_str()) == 0);
            readCount++;
            std::this_thread::sleep_for(std::chrono::milliseconds(15));
        }
        });

    writer.join();
    reader.join();

    REQUIRE(writtenCount.load() == 5);
    REQUIRE(readCount.load() == 5);
    REQUIRE(queue.GetCount() == 0);
}