#include <windows.h>
#include <iostream>
#include <vector>
#include <string>

int* array = nullptr;
int arraySize = 0;
int markerCount = 0;
std::vector<HANDLE> markerThreads;
std::vector<DWORD> markerThreadIds;
std::vector<int> threadMarks;
std::vector<bool> threadActive;

CRITICAL_SECTION arrayCriticalSection;
HANDLE startAllEvent;
std::vector<HANDLE> cannotContinueEvents;
std::vector<HANDLE> continueEvents;
std::vector<HANDLE> terminateEvents;


void PrintArray() {
    EnterCriticalSection(&arrayCriticalSection);
    std::cout << "Array content: ";
    for (int i = 0; i < arraySize; i++) {
        std::cout << array[i] << " ";
    }
    std::cout << std::endl;
    LeaveCriticalSection(&arrayCriticalSection);
}


DWORD WINAPI MarkerThreadProc(LPVOID lpParam) {
    int threadIdx = *((int*)lpParam);
    int threadNum = threadIdx + 1;

    WaitForSingleObject(startAllEvent, INFINITE);

    srand(threadNum);

    int markedElements = 0;

    while (true) {
        int randomIdx = rand() % arraySize;

        EnterCriticalSection(&arrayCriticalSection);

        if (array[randomIdx] == 0) {
            Sleep(5);
            array[randomIdx] = threadNum;
            threadMarks[threadIdx]++;
            markedElements++;
            Sleep(5);
            LeaveCriticalSection(&arrayCriticalSection);
        }
        else {
            LeaveCriticalSection(&arrayCriticalSection);

            std::cout << "Thread " << threadNum << " info:" << std::endl;
            std::cout << "- Thread number: " << threadNum << std::endl;
            std::cout << "- Marked elements: " << markedElements << std::endl;
            std::cout << "- Can't mark element at index: " << randomIdx << std::endl;

            SetEvent(cannotContinueEvents[threadIdx]);

            HANDLE waitHandles[2] = { continueEvents[threadIdx], terminateEvents[threadIdx] };
            DWORD waitResult = WaitForMultipleObjects(2, waitHandles, FALSE, INFINITE);

            if (waitResult == WAIT_OBJECT_0 + 1) {
                EnterCriticalSection(&arrayCriticalSection);
                for (int i = 0; i < arraySize; i++) {
                    if (array[i] == threadNum) {
                        array[i] = 0;
                    }
                }
                LeaveCriticalSection(&arrayCriticalSection);

                return 0;
            }
        }
    }

    return 0;
}

int main() {
    InitializeCriticalSection(&arrayCriticalSection);

    std::cout << "Enter array size: ";
    std::cin >> arraySize;

    array = new int[arraySize]();

    std::cout << "Enter number of marker threads: ";
    std::cin >> markerCount;

    markerThreads.resize(markerCount);
    markerThreadIds.resize(markerCount);
    threadMarks.resize(markerCount, 0);
    threadActive.resize(markerCount, true);

    startAllEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    cannotContinueEvents.resize(markerCount);
    continueEvents.resize(markerCount);
    terminateEvents.resize(markerCount);

    std::vector<int> threadIndices(markerCount);

    for (int i = 0; i < markerCount; i++) {
        threadIndices[i] = i;
        cannotContinueEvents[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
        continueEvents[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
        terminateEvents[i] = CreateEvent(NULL, FALSE, FALSE, NULL);

        markerThreads[i] = CreateThread(
            NULL,
            0,
            MarkerThreadProc,
            &threadIndices[i],
            0,
            &markerThreadIds[i]
        );

        if (markerThreads[i] == NULL) {
            std::cerr << "Error creating thread " << (i + 1) << std::endl;
            return 1;
        }
    }

    SetEvent(startAllEvent);

    int activeThreads = markerCount;
    while (activeThreads > 0) {
        std::vector<HANDLE> waitEvents;
        for (int i = 0; i < markerCount; i++) {
            if (threadActive[i]) {
                waitEvents.push_back(cannotContinueEvents[i]);
            }
        }

        WaitForMultipleObjects(waitEvents.size(), waitEvents.data(), TRUE, INFINITE);

        PrintArray();

        int threadToTerminate;
        std::cout << "Enter thread number to terminate (1-" << markerCount << "): ";
        std::cin >> threadToTerminate;

        threadToTerminate--;
        if (threadToTerminate >= 0 && threadToTerminate < markerCount && threadActive[threadToTerminate]) {
            SetEvent(terminateEvents[threadToTerminate]);

            WaitForSingleObject(markerThreads[threadToTerminate], INFINITE);

            threadActive[threadToTerminate] = false;
            activeThreads--;

            PrintArray();

            for (int i = 0; i < markerCount; i++) {
                if (threadActive[i]) {
                    SetEvent(continueEvents[i]);
                }
            }
        }
        else {
            std::cout << "Invalid thread number or thread already terminated" << std::endl;

            for (int i = 0; i < markerCount; i++) {
                if (threadActive[i]) {
                    SetEvent(continueEvents[i]);
                }
            }
        }
    }

    DeleteCriticalSection(&arrayCriticalSection);
    CloseHandle(startAllEvent);

    for (int i = 0; i < markerCount; i++) {
        CloseHandle(cannotContinueEvents[i]);
        CloseHandle(continueEvents[i]);
        CloseHandle(terminateEvents[i]);
        CloseHandle(markerThreads[i]);
    }

    delete[] array;

    std::cout << "All threads terminated. Program completed." << std::endl;
    return 0;
}