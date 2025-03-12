#include "pch.h"
#include "../Lab2/ThreadProcessing.h"
#include <Windows.h>
#include <vector>
#include <algorithm>

TEST(ThreadDataTest, BasicInitializationTest) {
    const int size = 5;
    int testArray[size] = { 3, 1, 5, 2, 4 };

    ThreadData data;
    data.array = testArray;
    data.size = size;
    data.average = 0;
    data.min_index = 0;
    data.max_index = 0;

    EXPECT_EQ(data.size, size);
    EXPECT_EQ(data.array[0], 3);
    EXPECT_EQ(data.array[1], 1);
    EXPECT_EQ(data.average, 0);
}

TEST(ThreadTest, MinMaxThreadTest) {
    const int size = 5;
    int* testArray = new int[size] { 3, 1, 5, 2, 4 };

    ThreadData data;
    data.array = testArray;
    data.size = size;
    data.min_index = 0;
    data.max_index = 0;

    HANDLE hMinMaxThread = CreateThread(
        NULL,
        0,
        MinMaxThread,
        &data,
        0,
        NULL
    );

    ASSERT_NE(hMinMaxThread, nullptr) << "Failed to create MinMax thread";

    WaitForSingleObject(hMinMaxThread, INFINITE);
    CloseHandle(hMinMaxThread);

    EXPECT_EQ(data.min_index, 1) << "Incorrect minimum index";
    EXPECT_EQ(data.max_index, 2) << "Incorrect maximum index";
    EXPECT_EQ(testArray[data.min_index], 1) << "Incorrect minimum value";
    EXPECT_EQ(testArray[data.max_index], 5) << "Incorrect maximum value";

    delete[] testArray;
}

TEST(ThreadTest, AverageThreadTest) {
    const int size = 5;
    int* testArray = new int[size] { 3, 1, 5, 2, 4 };

    ThreadData data;
    data.array = testArray;
    data.size = size;
    data.average = 0;

    HANDLE hAverageThread = CreateThread(
        NULL,
        0,
        AverageThread,
        &data,
        0,
        NULL
    );

    ASSERT_NE(hAverageThread, nullptr) << "Failed to create Average thread";

    WaitForSingleObject(hAverageThread, INFINITE);
    CloseHandle(hAverageThread);

    int expectedAverage = (3 + 1 + 5 + 2 + 4) / size;
    EXPECT_EQ(data.average, expectedAverage) << "Incorrect average value";

    delete[] testArray;
}

TEST(ThreadTest, IntegrationTest) {
    const int size = 5;
    int* testArray = new int[size] { 3, 1, 5, 2, 4 };
    std::vector<int> expectedArray = { 3, 3, 3, 2, 4 };

    ThreadData data;
    data.array = testArray;
    data.size = size;

    HANDLE hMinMaxThread = CreateThread(
        NULL,
        0,
        MinMaxThread,
        &data,
        0,
        NULL
    );

    ASSERT_NE(hMinMaxThread, nullptr) << "Failed to create MinMax thread";

    HANDLE hAverageThread = CreateThread(
        NULL,
        0,
        AverageThread,
        &data,
        0,
        NULL
    );

    ASSERT_NE(hAverageThread, nullptr) << "Failed to create Average thread";

    HANDLE handles[2] = { hMinMaxThread, hAverageThread };
    WaitForMultipleObjects(2, handles, TRUE, INFINITE);

    testArray[data.min_index] = data.average;
    if (data.min_index != data.max_index) {
        testArray[data.max_index] = data.average;
    }

    for (int i = 0; i < size; i++) {
        EXPECT_EQ(testArray[i], expectedArray[i]) << "Array mismatch at index " << i;
    }

    CloseHandle(hMinMaxThread);
    CloseHandle(hAverageThread);
    delete[] testArray;
}

TEST(ThreadTest, AllEqualElementsTest) {
    const int size = 3;
    int* testArray = new int[size] { 5, 5, 5 };

    ThreadData data;
    data.array = testArray;
    data.size = size;

    HANDLE hMinMaxThread = CreateThread(NULL, 0, MinMaxThread, &data, 0, NULL);
    HANDLE hAverageThread = CreateThread(NULL, 0, AverageThread, &data, 0, NULL);

    ASSERT_NE(hMinMaxThread, nullptr);
    ASSERT_NE(hAverageThread, nullptr);

    HANDLE handles[2] = { hMinMaxThread, hAverageThread };
    WaitForMultipleObjects(2, handles, TRUE, INFINITE);

    EXPECT_EQ(data.min_index, 0);
    EXPECT_EQ(data.max_index, 0);
    EXPECT_EQ(data.average, 5);

    testArray[data.min_index] = data.average;
    if (data.min_index != data.max_index) {
        testArray[data.max_index] = data.average;
    }

    for (int i = 0; i < size; i++) {
        EXPECT_EQ(testArray[i], 5);
    }

    CloseHandle(hMinMaxThread);
    CloseHandle(hAverageThread);
    delete[] testArray;
}

TEST(ThreadTest, LargeArrayTest) {
    const int size = 100;
    int* testArray = new int[size];

    for (int i = 0; i < size; i++) {
        testArray[i] = size - i;
    }

    ThreadData data;
    data.array = testArray;
    data.size = size;

    HANDLE hMinMaxThread = CreateThread(NULL, 0, MinMaxThread, &data, 0, NULL);
    HANDLE hAverageThread = CreateThread(NULL, 0, AverageThread, &data, 0, NULL);

    ASSERT_NE(hMinMaxThread, nullptr);
    ASSERT_NE(hAverageThread, nullptr);

    HANDLE handles[2] = { hMinMaxThread, hAverageThread };
    WaitForMultipleObjects(2, handles, TRUE, INFINITE);

    EXPECT_EQ(data.min_index, size - 1) << "Min index should be " << (size - 1);
    EXPECT_EQ(data.max_index, 0) << "Max index should be 0";
    EXPECT_EQ(data.array[data.min_index], 1) << "Min value should be 1";
    EXPECT_EQ(data.array[data.max_index], size) << "Max value should be " << size;

    int expectedAverage = (size * (size + 1)) / (2 * size);
    EXPECT_EQ(data.average, expectedAverage);

    CloseHandle(hMinMaxThread);
    CloseHandle(hAverageThread);
    delete[] testArray;
}

TEST(ThreadTest, NegativeNumbersTest) {
    const int size = 5;
    int* testArray = new int[size] { -3, -1, -5, -2, -4 };

    ThreadData data;
    data.array = testArray;
    data.size = size;

    HANDLE hMinMaxThread = CreateThread(NULL, 0, MinMaxThread, &data, 0, NULL);
    HANDLE hAverageThread = CreateThread(NULL, 0, AverageThread, &data, 0, NULL);

    HANDLE handles[2] = { hMinMaxThread, hAverageThread };
    WaitForMultipleObjects(2, handles, TRUE, INFINITE);

    EXPECT_EQ(data.min_index, 2) << "Min index should be 2";
    EXPECT_EQ(data.max_index, 1) << "Max index should be 1";
    EXPECT_EQ(testArray[data.min_index], -5) << "Min value should be -5";
    EXPECT_EQ(testArray[data.max_index], -1) << "Max value should be -1";

    int expectedAverage = (-3 + (-1) + (-5) + (-2) + (-4)) / size;
    EXPECT_EQ(data.average, expectedAverage);

    CloseHandle(hMinMaxThread);
    CloseHandle(hAverageThread);
    delete[] testArray;
}