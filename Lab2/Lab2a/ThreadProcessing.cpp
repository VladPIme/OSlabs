#include "ThreadProcessing.h"
#include <iostream>
#include <chrono>
#include <thread>

void MinMaxThread(ThreadData* data) {
    data->min_index = 0;
    data->max_index = 0;

    std::cout << "Min_max thread started\n";

    for (int i = 1; i < data->size; i++) {
        if (data->array[i] < data->array[data->min_index]) {
            data->min_index = i;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(7));

        if (data->array[i] > data->array[data->max_index]) {
            data->max_index = i;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(7));
    }

    std::cout << "Minimum element: " << data->array[data->min_index] << '\n';
    std::cout << "Maximum element: " << data->array[data->max_index] << '\n';

    std::cout << "Min_max thread finished\n";
}

void AverageThread(ThreadData* data) {
    std::cout << "Average thread started\n";

    int sum = 0;
    for (int i = 0; i < data->size; i++) {
        sum += data->array[i];
        std::this_thread::sleep_for(std::chrono::milliseconds(12));
    }

    data->average = sum / data->size;

    std::cout << "Average value: " << data->average << '\n';

    std::cout << "Average thread finished\n";
}