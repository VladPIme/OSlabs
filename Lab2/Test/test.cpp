#include "pch.h"

// ������� ���� ��� ��������, ��� Google Test ��������
TEST(TestBasic, BasicTest) {
    EXPECT_EQ(1, 1);
    EXPECT_TRUE(true);
}

// ���� ��� ThreadData ���������
TEST(ThreadDataTest, InitializationTest) {
    const int size = 5;
    int testArray[size] = { 3, 1, 5, 2, 4 };

    ThreadData data;
    data.array = testArray;
    data.size = size;
    data.average = 0.0;
    data.min_index = 0;
    data.max_index = 0;

    EXPECT_EQ(data.size, size);
    EXPECT_EQ(data.array[0], 3);
    EXPECT_EQ(data.array[1], 1);
    EXPECT_EQ(data.array[2], 5);
}

void TestMinMax(ThreadData* data) {
    data->min_index = 0;
    data->max_index = 0;

    for (int i = 1; i < data->size; i++) {
        if (data->array[i] < data->array[data->min_index]) {
            data->min_index = i;
        }

        if (data->array[i] > data->array[data->max_index]) {
            data->max_index = i;
        }
    }
}

// �������� ������� ��� ���������� ��������
void TestAverage(ThreadData* data) {
    int sum = 0;
    for (int i = 0; i < data->size; i++) {
        sum += data->array[i];
    }

    data->average = sum / data->size;
}

// ���� ��� �������� ������ ������ �������� � ���������
TEST(ThreadLogicTest, MinMaxLogicTest) {
    const int size = 5;
    int testArray[size] = { 3, 1, 5, 2, 4 };

    ThreadData data;
    data.array = testArray;
    data.size = size;

    // ���������� ���� �������� ������� ������ ������
    TestMinMax(&data);

    // ��������� ����������
    EXPECT_EQ(data.min_index, 1) << "Incorrect minimum index";
    EXPECT_EQ(data.max_index, 2) << "Incorrect maximum index";
    EXPECT_EQ(testArray[data.min_index], 1) << "Incorrect minimum value";
    EXPECT_EQ(testArray[data.max_index], 5) << "Incorrect maximum value";
}

// ���� ��� �������� ������ ���������� �������� ��������
TEST(ThreadLogicTest, AverageLogicTest) {
    const int size = 5;
    int testArray[size] = { 3, 1, 5, 2, 4 };

    ThreadData data;
    data.array = testArray;
    data.size = size;

    // ���������� ���� �������� ������� ������ ������
    TestAverage(&data);

    // ��������� ���������
    int expectedAverage = (3.0 + 1.0 + 5.0 + 2.0 + 4.0) / 5.0;
    EXPECT_EQ(data.average, expectedAverage) << "Incorrect average value";
}

// �������������� ���� ���� ������ ��������� �������
TEST(ThreadLogicTest, FullLogicTest) {
    const int size = 5;
    int testArray[size] = { 3, 1, 5, 2, 4 };
    int expectedArray[size];

    // �������� �������� ������
    for (int i = 0; i < size; i++) {
        expectedArray[i] = testArray[i];
    }

    ThreadData data;
    data.array = testArray;
    data.size = size;

    // ���������� ���� �������� �������
    TestMinMax(&data);
    TestAverage(&data);

    // �������� ����������� � ������������ �������� �������
    expectedArray[1] = data.average; // ����������� �������
    expectedArray[2] = data.average; // ������������ �������

    // ��������� �� �� ��������� � �������� �������
    testArray[data.min_index] = data.average;
    if (data.min_index != data.max_index) {
        testArray[data.max_index] = data.average;
    }

    // ���������, ��� ������ ������������� ����������
    for (int i = 0; i < size; i++) {
        EXPECT_DOUBLE_EQ(testArray[i], expectedArray[i]) << "Array mismatch at index " << i;
    }
}

// ���� � ���������� ����������
TEST(ThreadLogicTest, EdgeCasesTest) {
    // ������, ��� ������� � �������� - ���� � ��� �� �������
    const int size1 = 3;
    int testArray1[size1] = { 5, 5, 5 };

    ThreadData data1;
    data1.array = testArray1;
    data1.size = size1;

    TestMinMax(&data1);
    TestAverage(&data1);

    // ���������, ��� ������� �������� � ��������� ����������
    EXPECT_EQ(data1.min_index, 0);
    EXPECT_EQ(data1.max_index, 0);

    // ���� � �������� ������� 1
    const int size2 = 1;
    int testArray2[size2] = { 42 };

    ThreadData data2;
    data2.array = testArray2;
    data2.size = size2;

    TestMinMax(&data2);
    TestAverage(&data2);

    EXPECT_EQ(data2.min_index, 0);
    EXPECT_EQ(data2.max_index, 0);
    EXPECT_DOUBLE_EQ(data2.average, 42.0);
}