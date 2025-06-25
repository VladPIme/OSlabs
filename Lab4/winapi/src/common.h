#pragma once

#include <windows.h>
#include <string>
#include <vector>

constexpr int MAX_MESSAGE_LENGTH = 20;
constexpr int MAX_SENDERS = 10;

struct Message {
    char data[MAX_MESSAGE_LENGTH];
    bool isEmpty;
};

struct QueueHeader {
    int head;
    int tail;
    int size;
    int count;
};

const std::wstring MUTEX_NAME = L"MessageQueueMutex";
const std::wstring EMPTY_EVENT_NAME = L"EmptyEvent";
const std::wstring FULL_EVENT_NAME = L"FullEvent";
const std::wstring READY_EVENT_PREFIX = L"SenderReadyEvent_";