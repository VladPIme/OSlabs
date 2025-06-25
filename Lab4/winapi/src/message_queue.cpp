#include "../include/message_queue.h"
#include <cstring>

MessageQueue::MessageQueue(const std::wstring& fileName, int size)
    : fileName_(fileName), size_(size),
    fileHandle_(INVALID_HANDLE_VALUE), fileMappingHandle_(NULL), mappedView_(NULL),
    mutex_(NULL), emptyEvent_(NULL), fullEvent_(NULL),
    header_(nullptr), messages_(nullptr) {
}

MessageQueue::~MessageQueue() {
    if (mappedView_) {
        UnmapViewOfFile(mappedView_);
    }

    if (fileMappingHandle_) {
        CloseHandle(fileMappingHandle_);
    }

    if (fileHandle_ != INVALID_HANDLE_VALUE) {
        CloseHandle(fileHandle_);
    }

    if (mutex_) {
        CloseHandle(mutex_);
    }

    if (emptyEvent_) {
        CloseHandle(emptyEvent_);
    }

    if (fullEvent_) {
        CloseHandle(fullEvent_);
    }
}

bool MessageQueue::Initialize(bool isCreator) {
    mutex_ = isCreator
        ? CreateMutexW(NULL, FALSE, MUTEX_NAME.c_str())
        : OpenMutexW(SYNCHRONIZE, FALSE, MUTEX_NAME.c_str());

    if (!mutex_) {
        return false;
    }

    if (isCreator) {
        emptyEvent_ = CreateEventW(NULL, TRUE, TRUE, EMPTY_EVENT_NAME.c_str());
        fullEvent_ = CreateEventW(NULL, TRUE, FALSE, FULL_EVENT_NAME.c_str());
    }
    else {
        emptyEvent_ = OpenEventW(SYNCHRONIZE, FALSE, EMPTY_EVENT_NAME.c_str());
        fullEvent_ = OpenEventW(SYNCHRONIZE | EVENT_MODIFY_STATE, FALSE, FULL_EVENT_NAME.c_str());
    }

    if (!emptyEvent_ || !fullEvent_) {
        return false;
    }

    if (isCreator) {
        fileHandle_ = CreateFileW(
            fileName_.c_str(),
            GENERIC_READ | GENERIC_WRITE,
            FILE_SHARE_READ | FILE_SHARE_WRITE,
            NULL,
            CREATE_ALWAYS,
            FILE_ATTRIBUTE_NORMAL,
            NULL
        );

        if (fileHandle_ == INVALID_HANDLE_VALUE) {
            return false;
        }

        DWORD fileSize = sizeof(QueueHeader) + size_ * sizeof(Message);
        SetFilePointer(fileHandle_, fileSize, NULL, FILE_BEGIN);
        SetEndOfFile(fileHandle_);

        fileMappingHandle_ = CreateFileMappingW(
            fileHandle_,
            NULL,
            PAGE_READWRITE,
            0,
            0,
            NULL
        );
    }
    else {
        fileHandle_ = CreateFileW(
            fileName_.c_str(),
            GENERIC_READ | GENERIC_WRITE,
            FILE_SHARE_READ | FILE_SHARE_WRITE,
            NULL,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            NULL
        );

        if (fileHandle_ == INVALID_HANDLE_VALUE) {
            return false;
        }

        fileMappingHandle_ = CreateFileMappingW(
            fileHandle_,
            NULL,
            PAGE_READWRITE,
            0,
            0,
            NULL
        );
    }

    if (!fileMappingHandle_) {
        return false;
    }

    mappedView_ = MapViewOfFile(
        fileMappingHandle_,
        FILE_MAP_ALL_ACCESS,
        0,
        0,
        0
    );

    if (!mappedView_) {
        return false;
    }

    header_ = static_cast<QueueHeader*>(mappedView_);
    messages_ = reinterpret_cast<Message*>(static_cast<char*>(mappedView_) + sizeof(QueueHeader));

    if (isCreator) {
        header_->head = 0;
        header_->tail = 0;
        header_->size = size_;
        header_->count = 0;

        for (int i = 0; i < size_; ++i) {
            messages_[i].isEmpty = true;
        }
    }

    return true;
}

bool MessageQueue::Write(const char* message) {
    if (!mutex_ || !emptyEvent_ || !fullEvent_ || !header_ || !messages_) {
        return false;
    }

    WaitForSingleObject(emptyEvent_, INFINITE);
    WaitForSingleObject(mutex_, INFINITE);

    bool result = false;

    if (header_->count < header_->size) {
        strncpy_s(messages_[header_->tail].data, MAX_MESSAGE_LENGTH, message, _TRUNCATE);
        messages_[header_->tail].isEmpty = false;

        header_->tail = (header_->tail + 1) % header_->size;
        header_->count++;

        if (header_->count == header_->size) {
            ResetEvent(emptyEvent_);
        }

        SetEvent(fullEvent_);

        result = true;
    }

    ReleaseMutex(mutex_);

    return result;
}

bool MessageQueue::Read(char* message, int maxSize) {
    if (!mutex_ || !emptyEvent_ || !fullEvent_ || !header_ || !messages_) {
        return false;
    }

    WaitForSingleObject(fullEvent_, INFINITE);
    WaitForSingleObject(mutex_, INFINITE);

    bool result = false;

    if (header_->count > 0) {
        strncpy_s(message, maxSize, messages_[header_->head].data, _TRUNCATE);
        messages_[header_->head].isEmpty = true;

        header_->head = (header_->head + 1) % header_->size;
        header_->count--;

        if (header_->count == 0) {
            ResetEvent(fullEvent_);
        }

        SetEvent(emptyEvent_);

        result = true;
    }

    ReleaseMutex(mutex_);

    return result;
}

int MessageQueue::GetCount() const {
    if (!header_) {
        return 0;
    }

    return header_->count;
}