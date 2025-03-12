#include "pch.h"
#include "../Lab2a/ThreadProcessing.h"
#include <thread>
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

    std::thread minMaxThread(MinMaxThread, &data);
    minMaxThread.join();

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

    std::thread averageThread(AverageThread, &data);
    averageThread.join();

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

    std::thread minMaxThread(MinMaxThread, &data);
    std::thread averageThread(AverageThread, &data);

    minMaxThread.join();
    averageThread.join();

    testArray[data.min_index] = data.average;
    if (data.min_index != data.max_index) {
        testArray[data.max_index] = data.average;
    }

    for (int i = 0; i < size; i++) {
        EXPECT_EQ(testArray[i], expectedArray[i]) << "Array mismatch at index " << i;
    }

    delete[] testArray;
}

TEST(ThreadTest, AllEqualElementsTest) {
    const int size = 3;
    int* testArray = new int[size] { 5, 5, 5 };

    ThreadData data;
    data.array = testArray;
    data.size = size;

    std::thread minMaxThread(MinMaxThread, &data);
    std::thread averageThread(AverageThread, &data);

    minMaxThread.join();
    averageThread.join();

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

    std::thread minMaxThread(MinMaxThread, &data);
    std::thread averageThread(AverageThread, &data);

    minMaxThread.join();
    averageThread.join();

    EXPECT_EQ(data.min_index, size - 1) << "Min index should be " << (size - 1);
    EXPECT_EQ(data.max_index, 0) << "Max index should be 0";
    EXPECT_EQ(data.array[data.min_index], 1) << "Min value should be 1";
    EXPECT_EQ(data.array[data.max_index], size) << "Max value should be " << size;

    int expectedAverage = (size * (size + 1)) / (2 * size);
    EXPECT_EQ(data.average, expectedAverage);

    delete[] testArray;
}

TEST(ThreadTest, NegativeNumbersTest) {
    const int size = 5;
    int* testArray = new int[size] { -3, -1, -5, -2, -4 };

    ThreadData data;
    data.array = testArray;
    data.size = size;

    std::thread minMaxThread(MinMaxThread, &data);
    std::thread averageThread(AverageThread, &data);

    minMaxThread.join();
    averageThread.join();

    EXPECT_EQ(data.min_index, 2) << "Min index should be 2";
    EXPECT_EQ(data.max_index, 1) << "Max index should be 1";
    EXPECT_EQ(testArray[data.min_index], -5) << "Min value should be -5";
    EXPECT_EQ(testArray[data.max_index], -1) << "Max value should be -1";

    int expectedAverage = (-3 + (-1) + (-5) + (-2) + (-4)) / size;
    EXPECT_EQ(data.average, expectedAverage);

    delete[] testArray;
}