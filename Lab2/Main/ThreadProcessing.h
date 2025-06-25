#pragma once

#include <windows.h>

// Структура данных для потоков
struct ThreadData {
    int* array;
    int size;
    int average;
    int min_index;
    int max_index;
};

// Объявления функций потоков
DWORD WINAPI MinMaxThread(LPVOID lpParam);
DWORD WINAPI AverageThread(LPVOID lpParam);