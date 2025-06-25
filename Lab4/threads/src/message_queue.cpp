#include "../include/message_queue.h"
#include <cstring>

MessageQueue::MessageQueue(const std::string& fileName, int size)
    : fileName_(fileName), size_(size) {
    header_.head = 0;
    header_.tail = 0;
    header_.size = size;
    header_.count = 0;
}

MessageQueue::~MessageQueue() {
    if (file_.is_open()) {
        file_.close();
    }
}

bool MessageQueue::Initialize(bool isCreator) {
    if (isCreator) {
        file_.open(fileName_, std::ios::binary | std::ios::out | std::ios::trunc);
        if (!file_.is_open()) {
            return false;
        }

        file_.write(reinterpret_cast<const char*>(&header_), sizeof(QueueHeader));

        Message emptyMsg = {};
        for (int i = 0; i < size_; ++i) {
            file_.write(reinterpret_cast<const char*>(&emptyMsg), sizeof(Message));
        }

        file_.close();
    }

    file_.open(fileName_, std::ios::binary | std::ios::in | std::ios::out);
    if (!file_.is_open()) {
        return false;
    }

    if (!isCreator) {
        ReadHeader();
    }

    return true;
}

bool MessageQueue::Write(const char* message) {
    std::unique_lock<std::mutex> lock(mutex_);

    ReadHeader();

    notFullCV_.wait(lock, [this]() { return header_.count < header_.size; });

    Message msg = {};
    strncpy_s(msg.data, MAX_MESSAGE_LENGTH, message, _TRUNCATE);

    WriteTo(header_.tail, msg);

    header_.tail = (header_.tail + 1) % header_.size;
    header_.count++;

    UpdateHeader();

    notEmptyCV_.notify_one();

    return true;
}

bool MessageQueue::Read(char* message, int maxSize) {
    std::unique_lock<std::mutex> lock(mutex_);

    ReadHeader();

    notEmptyCV_.wait(lock, [this]() { return header_.count > 0; });

    Message msg = {};
    ReadFrom(header_.head, msg);

    strncpy_s(message, maxSize, msg.data, _TRUNCATE);

    header_.head = (header_.head + 1) % header_.size;
    header_.count--;

    UpdateHeader();

    notFullCV_.notify_one();

    return true;
}

int MessageQueue::GetCount() const {
    std::lock_guard<std::mutex> lock(mutex_);
    ReadHeader();
    return header_.count;
}

void MessageQueue::UpdateHeader() {
    file_.seekp(0);
    file_.write(reinterpret_cast<const char*>(&header_), sizeof(QueueHeader));
    file_.flush();
}

void MessageQueue::ReadHeader() const {
    file_.seekg(0);
    file_.read(reinterpret_cast<char*>(&const_cast<QueueHeader&>(header_)), sizeof(QueueHeader));
}

void MessageQueue::WriteTo(int index, const Message& msg) {
    file_.seekp(sizeof(QueueHeader) + index * sizeof(Message));
    file_.write(reinterpret_cast<const char*>(&msg), sizeof(Message));
    file_.flush();
}

void MessageQueue::ReadFrom(int index, Message& msg) {
    file_.seekg(sizeof(QueueHeader) + index * sizeof(Message));
    file_.read(reinterpret_cast<char*>(&msg), sizeof(Message));
}