#include "ThreadProcessing.h"
#include <iostream>
using namespace std;

DWORD WINAPI MinMaxThread(LPVOID lpParam) {
    ThreadData* data = (ThreadData*)lpParam;

    data->min_index = 0;
    data->max_index = 0;

    cout << "Min_max thread started\n";

    for (int i = 1; i < data->size; i++) {
        if (data->array[i] < data->array[data->min_index]) {
            data->min_index = i;
        }
        Sleep(7);

        if (data->array[i] > data->array[data->max_index]) {
            data->max_index = i;
        }
        Sleep(7);
    }

    cout << "Minimum element: " << data->array[data->min_index] << '\n';
    cout << "Maximum element: " << data->array[data->max_index] << '\n';

    cout << "Min_max thread finished\n";
    return 0;
}

DWORD WINAPI AverageThread(LPVOID lpParam) {
    ThreadData* data = (ThreadData*)lpParam;

    cout << "Average thread started\n";

    int sum = 0;
    for (int i = 0; i < data->size; i++) {
        sum += data->array[i];
        Sleep(12);
    }

    data->average = sum / data->size;

    cout << "Average value: " << data->average << '\n';

    cout << "Average thread finished\n";
    return 0;
}

int main() {
    int size;
    cout << "Enter array size: ";
    cin >> size;

    if (size <= 0) {
        cout << "Invalid array size\n";
        return 1;
    }

    int* array = new int[size];

    cout << "Enter " << size << " elements:\n";
    for (int i = 0; i < size; i++) {
        cin >> array[i];
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
        cout << "Error creating min_max thread. Error code: " << GetLastError() << '\n';
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
        cout << "Error creating average thread. Error code: " << GetLastError() << '\n';
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

    cout << "Array after replacing min and max elements with average value:" << '\n';
    for (int i = 0; i < size; i++) {
        cout << array[i] << " ";
    }
    cout << '\n';

    CloseHandle(hMinMaxThread);
    CloseHandle(hAverageThread);
    delete[] array;

    return 0;
}