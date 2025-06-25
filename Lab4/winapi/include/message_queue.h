#pragma once

#include "../src/common.h"

class MessageQueue {
public:
    MessageQueue(const std::wstring& fileName, int size);
    ~MessageQueue();

    bool Initialize(bool isCreator);
    bool Write(const char* message);
    bool Read(char* message, int maxSize);
    int GetCount() const;

private:
    std::wstring fileName_;
    int size_;

    HANDLE fileHandle_;
    HANDLE fileMappingHandle_;
    LPVOID mappedView_;

    HANDLE mutex_;
    HANDLE emptyEvent_;
    HANDLE fullEvent_;

    QueueHeader* header_;
    Message* messages_;
};