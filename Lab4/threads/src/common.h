#pragma once

#include <string>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <windows.h>

constexpr int MAX_MESSAGE_LENGTH = 20;
constexpr int MAX_SENDERS = 10;

struct Message {
    char data[MAX_MESSAGE_LENGTH];
};

struct QueueHeader {
    int head;
    int tail;
    int size;
    int count;
};