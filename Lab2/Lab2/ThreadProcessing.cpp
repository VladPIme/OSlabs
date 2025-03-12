#include "ThreadProcessing.h"
#include <iostream>

DWORD WINAPI MinMaxThread(LPVOID lpParam) {
    ThreadData* data = (ThreadData*)lpParam;

    data->min_index = 0;
    data->max_index = 0;

    std::cout << "Min_max thread started\n";

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

    std::cout << "Minimum element: " << data->array[data->min_index] << '\n';
    std::cout << "Maximum element: " << data->array[data->max_index] << '\n';

    std::cout << "Min_max thread finished\n";
    return 0;
}

DWORD WINAPI AverageThread(LPVOID lpParam) {
    ThreadData* data = (ThreadData*)lpParam;

    std::cout << "Average thread started\n";

    int sum = 0;
    for (int i = 0; i < data->size; i++) {
        sum += data->array[i];
        Sleep(12);
    }

    data->average = sum / data->size;

    std::cout << "Average value: " << data->average << '\n';

    std::cout << "Average thread finished\n";
    return 0;
}