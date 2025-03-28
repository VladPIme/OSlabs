#define CATCH_CONFIG_MAIN
#include "../include/catch.hpp"

#include <windows.h>
#include <vector>
#include <iostream>

int* CreateAndInitializeArray(int size) {
    int* array = new int[size]();
    return array;
}

bool MarkArrayElement(int* array, int size, int index, int threadId) {
    if (index < 0 || index >= size) {
        return false;
    }

    if (array[index] == 0) {
        array[index] = threadId;
        return true;
    }

    return false;
}

int ClearThreadMarks(int* array, int size, int threadId) {
    int clearedCount = 0;
    for (int i = 0; i < size; i++) {
        if (array[i] == threadId) {
            array[i] = 0;
            clearedCount++;
        }
    }
    return clearedCount;
}

struct ThreadArgs {
    int* counter;
    CRITICAL_SECTION* cs;
    int iterations;
};


DWORD WINAPI IncrementCounter(LPVOID param) {
    ThreadArgs* args = static_cast<ThreadArgs*>(param);

    for (int i = 0; i < args->iterations; i++) {
        EnterCriticalSection(args->cs);
        (*args->counter)++;
        LeaveCriticalSection(args->cs);
    }

    return 0;
}


bool TestCriticalSection() {
    CRITICAL_SECTION cs;
    InitializeCriticalSection(&cs);

    int sharedCounter = 0;
    const int numThreads = 5;
    const int iterationsPerThread = 1000;

    std::vector<HANDLE> threads(numThreads);
    std::vector<ThreadArgs> threadArgs(numThreads);

    for (int i = 0; i < numThreads; i++) {
        threadArgs[i].counter = &sharedCounter;
        threadArgs[i].cs = &cs;
        threadArgs[i].iterations = iterationsPerThread;

        threads[i] = CreateThread(NULL, 0, IncrementCounter, &threadArgs[i], 0, NULL);
        if (threads[i] == NULL) {
            DeleteCriticalSection(&cs);
            return false;
        }
    }

    WaitForMultipleObjects(numThreads, threads.data(), TRUE, INFINITE);


    for (int i = 0; i < numThreads; i++) {
        CloseHandle(threads[i]);
    }

    DeleteCriticalSection(&cs);

    return sharedCounter == numThreads * iterationsPerThread;
}

bool TestEvents() {
    HANDLE event = CreateEvent(NULL, TRUE, FALSE, NULL); 

    if (event == NULL) {
        return false;
    }

    DWORD result = WaitForSingleObject(event, 0);
    if (result != WAIT_TIMEOUT) {
        CloseHandle(event);
        return false;
    }

    if (!SetEvent(event)) {
        CloseHandle(event);
        return false;
    }

    result = WaitForSingleObject(event, 0);
    if (result != WAIT_OBJECT_0) {
        CloseHandle(event);
        return false;
    }

    if (!ResetEvent(event)) {
        CloseHandle(event);
        return false;
    }

    result = WaitForSingleObject(event, 0);
    if (result != WAIT_TIMEOUT) {
        CloseHandle(event);
        return false;
    }

    CloseHandle(event);
    return true;
}

TEST_CASE("Array Initialization", "[winapi]") {
    const int size = 10;
    int* array = CreateAndInitializeArray(size);

    for (int i = 0; i < size; i++) {
        REQUIRE(array[i] == 0);
    }

    delete[] array;
}

TEST_CASE("Array Marking", "[winapi]") {
    const int size = 10;
    int* array = CreateAndInitializeArray(size);

    REQUIRE(MarkArrayElement(array, size, 2, 1));
    REQUIRE(MarkArrayElement(array, size, 5, 1));
    REQUIRE(MarkArrayElement(array, size, 8, 1));

    REQUIRE(array[2] == 1);
    REQUIRE(array[5] == 1);
    REQUIRE(array[8] == 1);

    REQUIRE_FALSE(MarkArrayElement(array, size, 2, 2));
    REQUIRE(array[2] == 1); 

    REQUIRE_FALSE(MarkArrayElement(array, size, -1, 1));
    REQUIRE_FALSE(MarkArrayElement(array, size, size, 1));

    delete[] array;
}

TEST_CASE("Clearing Thread Marks", "[winapi]") {
    const int size = 10;
    int* array = CreateAndInitializeArray(size);

    MarkArrayElement(array, size, 0, 1);
    MarkArrayElement(array, size, 2, 1);
    MarkArrayElement(array, size, 4, 2);
    MarkArrayElement(array, size, 6, 1);
    MarkArrayElement(array, size, 8, 2);

    int cleared = ClearThreadMarks(array, size, 1);
    REQUIRE(cleared == 3);

    REQUIRE(array[0] == 0);
    REQUIRE(array[2] == 0);
    REQUIRE(array[6] == 0);

    REQUIRE(array[4] == 2);
    REQUIRE(array[8] == 2);

    delete[] array;
}

TEST_CASE("Critical Section Synchronization", "[winapi]") {
    REQUIRE(TestCriticalSection());
}

TEST_CASE("Event Signaling", "[winapi]") {
    REQUIRE(TestEvents());
}

TEST_CASE("Multiple Events with WaitForMultipleObjects", "[winapi]") {
    const int numEvents = 3;
    HANDLE events[numEvents];


    for (int i = 0; i < numEvents; i++) {
        events[i] = CreateEvent(NULL, TRUE, FALSE, NULL);
        REQUIRE(events[i] != NULL);
    }

    SetEvent(events[1]);

    DWORD result = WaitForMultipleObjects(numEvents, events, FALSE, 0);
    REQUIRE(result == (WAIT_OBJECT_0 + 1));

    for (int i = 0; i < numEvents; i++) {
        SetEvent(events[i]);
    }

    result = WaitForMultipleObjects(numEvents, events, TRUE, 0);
    REQUIRE(result == WAIT_OBJECT_0);

    for (int i = 0; i < numEvents; i++) {
        CloseHandle(events[i]);
    }
}