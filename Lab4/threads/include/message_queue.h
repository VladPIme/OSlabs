#pragma once

#include "../src/common.h"
#include <fstream>
#include <thread>
#include <condition_variable>
#include <mutex>

class MessageQueue {
public:
    MessageQueue(const std::string& fileName, int size);
    ~MessageQueue();

    bool Initialize(bool isCreator);
    bool Write(const char* message);
    bool Read(char* message, int maxSize);
    int GetCount() const;

private:
    std::string fileName_;
    int size_;

    mutable std::mutex mutex_;
    std::condition_variable notEmptyCV_;
    std::condition_variable notFullCV_;

    mutable std::fstream file_;
    mutable QueueHeader header_;

    void UpdateHeader();
    void ReadHeader() const;
    void WriteTo(int index, const Message& msg);
    void ReadFrom(int index, Message& msg);
};