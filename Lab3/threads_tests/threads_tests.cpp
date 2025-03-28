#define CATCH_CONFIG_MAIN
#include "../include/catch.hpp"

#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <atomic>
#include <iostream>

std::vector<int> CreateAndInitializeArray(int size) {
    return std::vector<int>(size, 0);
}

bool MarkArrayElement(std::vector<int>& array, int index, int threadId) {
    if (index < 0 || index >= static_cast<int>(array.size())) {
        return false;
    }

    if (array[index] == 0) {
        array[index] = threadId;
        return true;
    }

    return false;
}

int ClearThreadMarks(std::vector<int>& array, int threadId) {
    int clearedCount = 0;
    for (size_t i = 0; i < array.size(); i++) {
        if (array[i] == threadId) {
            array[i] = 0;
            clearedCount++;
        }
    }
    return clearedCount;
}

bool TestMutex() {
    std::mutex mtx;
    std::atomic<int> sharedCounter(0);
    const int numThreads = 5;
    const int iterationsPerThread = 1000;

    std::vector<std::thread> threads;

    for (int i = 0; i < numThreads; i++) {
        threads.emplace_back([&]() {
            for (int j = 0; j < iterationsPerThread; j++) {
                std::lock_guard<std::mutex> lock(mtx);
                sharedCounter++;
            }
            });
    }

    for (auto& thread : threads) {
        thread.join();
    }

    return sharedCounter == numThreads * iterationsPerThread;
}


bool TestConditionVariable() {
    std::mutex mtx;
    std::condition_variable cv;
    bool ready = false;
    bool processed = false;

    std::thread worker([&]() {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [&] { return ready; });

        processed = true;

        lock.unlock();
        cv.notify_one();
        });

    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        std::lock_guard<std::mutex> lock(mtx);
        ready = true;
    }
    cv.notify_one();

    {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait_for(lock, std::chrono::seconds(1), [&] { return processed; });
    }

    worker.join();
    return processed;
}

TEST_CASE("Array Initialization", "[threads]") {
    const int size = 10;
    auto array = CreateAndInitializeArray(size);

    for (int i = 0; i < size; i++) {
        REQUIRE(array[i] == 0);
    }
}

TEST_CASE("Array Marking", "[threads]") {
    const int size = 10;
    auto array = CreateAndInitializeArray(size);

    REQUIRE(MarkArrayElement(array, 2, 1));
    REQUIRE(MarkArrayElement(array, 5, 1));
    REQUIRE(MarkArrayElement(array, 8, 1));

    REQUIRE(array[2] == 1);
    REQUIRE(array[5] == 1);
    REQUIRE(array[8] == 1);

    REQUIRE_FALSE(MarkArrayElement(array, 2, 2));
    REQUIRE(array[2] == 1);

    REQUIRE_FALSE(MarkArrayElement(array, -1, 1));
    REQUIRE_FALSE(MarkArrayElement(array, size, 1));
}

TEST_CASE("Clearing Thread Marks", "[threads]") {
    const int size = 10;
    auto array = CreateAndInitializeArray(size);

    MarkArrayElement(array, 0, 1);
    MarkArrayElement(array, 2, 1);
    MarkArrayElement(array, 4, 2);
    MarkArrayElement(array, 6, 1);
    MarkArrayElement(array, 8, 2);

    int cleared = ClearThreadMarks(array, 1);
    REQUIRE(cleared == 3);

    REQUIRE(array[0] == 0);
    REQUIRE(array[2] == 0);
    REQUIRE(array[6] == 0);

    REQUIRE(array[4] == 2);
    REQUIRE(array[8] == 2);
}

TEST_CASE("Mutex Synchronization", "[threads]") {
    REQUIRE(TestMutex());
}

TEST_CASE("Condition Variable Signaling", "[threads]") {
    REQUIRE(TestConditionVariable());
}

TEST_CASE("Thread Safety with Multiple Threads", "[threads]") {
    const int size = 100;
    auto array = CreateAndInitializeArray(size);
    std::mutex arrayMutex;

    const int numThreads = 5;
    std::vector<std::thread> threads;
    std::atomic<int> totalMarked(0);

    for (int i = 0; i < numThreads; i++) {
        threads.emplace_back([&, threadId = i + 1]() {
            int markedByThisThread = 0;
            for (int attempt = 0; attempt < size; attempt++) {
                int index = rand() % size;

                std::lock_guard<std::mutex> lock(arrayMutex);
                if (array[index] == 0) {
                    array[index] = threadId;
                    markedByThisThread++;
                }
            }
            totalMarked += markedByThisThread;
            });
    }

    for (auto& thread : threads) {
        thread.join();
    }

    int markedInArray = 0;
    for (int element : array) {
        if (element != 0) {
            markedInArray++;
        }
    }

    REQUIRE(markedInArray == totalMarked);
}