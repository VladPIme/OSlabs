#include "ThreadProcessing.h"
#include <iostream>
#include <thread>

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

    std::thread minMaxThread(MinMaxThread, &data);
    std::thread averageThread(AverageThread, &data);

    minMaxThread.join();
    averageThread.join();

    array[data.min_index] = data.average;
    if (data.min_index != data.max_index) {
        array[data.max_index] = data.average;
    }

    std::cout << "Array after replacing min and max elements with average value:" << '\n';
    for (int i = 0; i < size; i++) {
        std::cout << array[i] << " ";
    }
    std::cout << '\n';

    delete[] array;

    return 0;
}