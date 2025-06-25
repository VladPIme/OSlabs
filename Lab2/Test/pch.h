#pragma once

#include "gtest/gtest.h"
#include <windows.h>
#include <iostream>

struct ThreadData {
    int* array;
    int size;
    double average;
    int min_index;
    int max_index;
};

extern "C" {
    DWORD WINAPI MinMaxThread(LPVOID lpParam);
    DWORD WINAPI AverageThread(LPVOID lpParam);
}