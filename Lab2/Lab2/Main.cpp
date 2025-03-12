#include "ThreadProcessing.h"
#include <iostream>

int main() {
    int size;
    std::cout << "Enter array size: ";
    std::cin >> size;

    if (size <= 0) {
        std::cout << "Invalid array size\n";
        return 1;
    }

    int* array = new int[size];

    std::cout << "Enter " << size << " elements:\n";
    for (int i = 0; i < size; i++) {
        std::cin >> array[i];
    }

    ThreadData data;
    data.array = array;
    data.size = size;

    HANDLE hMinMaxThread;
    DWORD minMaxThreadId;

    hMinMaxThread = CreateThread(
        NULL,
        0,
        MinMaxThread,
        &data,
        0,
        &minMaxThreadId
    );

    if (hMinMaxThread == NULL) {
        std::cout << "Error creating min_max thread. Error code: " << GetLastError() << '\n';
        delete[] array;
        return 1;
    }

    HANDLE hAverageThread;
    DWORD averageThreadId;

    hAverageThread = CreateThread(
        NULL,
        0,
        AverageThread,
        &data,
        0,
        &averageThreadId
    );

    if (hAverageThread == NULL) {
        std::cout << "Error creating average thread. Error code: " << GetLastError() << '\n';
        CloseHandle(hMinMaxThread);
        delete[] array;
        return 1;
    }

    WaitForSingleObject(hMinMaxThread, INFINITE);
    WaitForSingleObject(hAverageThread, INFINITE);

    array[data.min_index] = data.average;
    if (data.min_index != data.max_index) {
        array[data.max_index] = data.average;
    }

    std::cout << "Array after replacing min and max elements with average value:" << '\n';
    for (int i = 0; i < size; i++) {
        std::cout << array[i] << " ";
    }
    std::cout << '\n';

    CloseHandle(hMinMaxThread);
    CloseHandle(hAverageThread);
    delete[] array;

    return 0;
}