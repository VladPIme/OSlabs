#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include "message_queue.h"

TEST_CASE("MessageQueue initialization", "[winapi]") {
    const std::wstring testFileName = L"test_queue.bin";
    const int queueSize = 10;

    MessageQueue queue(testFileName, queueSize);
    REQUIRE(queue.Initialize(true));

    REQUIRE(queue.GetCount() == 0);
}

TEST_CASE("MessageQueue write and read", "[winapi]") {
    const std::wstring testFileName = L"test_queue.bin";
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

TEST_CASE("MessageQueue full and empty behavior", "[winapi]") {
    const std::wstring testFileName = L"test_queue.bin";
    const int queueSize = 2;

    MessageQueue queue(testFileName, queueSize);
    REQUIRE(queue.Initialize(true));

    REQUIRE(queue.Write("Message 1"));
    REQUIRE(queue.Write("Message 2"));

    REQUIRE(queue.GetCount() == 2);

    char readMessage[MAX_MESSAGE_LENGTH + 1] = { 0 };
    REQUIRE(queue.Read(readMessage, MAX_MESSAGE_LENGTH));
    REQUIRE(strcmp(readMessage, "Message 1") == 0);

    REQUIRE(queue.Read(readMessage, MAX_MESSAGE_LENGTH));
    REQUIRE(strcmp(readMessage, "Message 2") == 0);

    REQUIRE(queue.GetCount() == 0);
}