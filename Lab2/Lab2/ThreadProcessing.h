#pragma once

#include <windows.h>

struct ThreadData {
    int* array;
    int size;
    int average;
    int min_index;
    int max_index;
};

DWORD WINAPI MinMaxThread(LPVOID lpParam);
DWORD WINAPI AverageThread(LPVOID lpParam);