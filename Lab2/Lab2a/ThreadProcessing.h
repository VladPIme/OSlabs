#pragma once

#include <thread>

struct ThreadData {
    int* array;
    int size;
    int average;
    int min_index;
    int max_index;
};

void MinMaxThread(ThreadData* data);
void AverageThread(ThreadData* data);