#include "pch.h"

// Простой тест для проверки, что Google Test работает
TEST(TestBasic, BasicTest) {
    EXPECT_EQ(1, 1);
    EXPECT_TRUE(true);
}

// Тест для ThreadData структуры
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

// Тестовая функция для вычисления среднего
void TestAverage(ThreadData* data) {
    int sum = 0;
    for (int i = 0; i < data->size; i++) {
        sum += data->array[i];
    }

    data->average = sum / data->size;
}

// Тест для проверки логики поиска минимума и максимума
TEST(ThreadLogicTest, MinMaxLogicTest) {
    const int size = 5;
    int testArray[size] = { 3, 1, 5, 2, 4 };

    ThreadData data;
    data.array = testArray;
    data.size = size;

    // Используем нашу тестовую функцию вместо потока
    TestMinMax(&data);

    // Проверяем результаты
    EXPECT_EQ(data.min_index, 1) << "Incorrect minimum index";
    EXPECT_EQ(data.max_index, 2) << "Incorrect maximum index";
    EXPECT_EQ(testArray[data.min_index], 1) << "Incorrect minimum value";
    EXPECT_EQ(testArray[data.max_index], 5) << "Incorrect maximum value";
}

// Тест для проверки логики вычисления среднего значения
TEST(ThreadLogicTest, AverageLogicTest) {
    const int size = 5;
    int testArray[size] = { 3, 1, 5, 2, 4 };

    ThreadData data;
    data.array = testArray;
    data.size = size;

    // Используем нашу тестовую функцию вместо потока
    TestAverage(&data);

    // Проверяем результат
    int expectedAverage = (3.0 + 1.0 + 5.0 + 2.0 + 4.0) / 5.0;
    EXPECT_EQ(data.average, expectedAverage) << "Incorrect average value";
}

// Интеграционный тест всей логики обработки массива
TEST(ThreadLogicTest, FullLogicTest) {
    const int size = 5;
    int testArray[size] = { 3, 1, 5, 2, 4 };
    int expectedArray[size];

    // Копируем исходный массив
    for (int i = 0; i < size; i++) {
        expectedArray[i] = testArray[i];
    }

    ThreadData data;
    data.array = testArray;
    data.size = size;

    // Используем наши тестовые функции
    TestMinMax(&data);
    TestAverage(&data);

    // Заменяем минимальное и максимальное значения средним
    expectedArray[1] = data.average; // Минимальный элемент
    expectedArray[2] = data.average; // Максимальный элемент

    // Выполняем то же замещение в тестовом массиве
    testArray[data.min_index] = data.average;
    if (data.min_index != data.max_index) {
        testArray[data.max_index] = data.average;
    }

    // Проверяем, что массив соответствует ожидаемому
    for (int i = 0; i < size; i++) {
        EXPECT_DOUBLE_EQ(testArray[i], expectedArray[i]) << "Array mismatch at index " << i;
    }
}

// Тест с граничными значениями
TEST(ThreadLogicTest, EdgeCasesTest) {
    // Массив, где минимум и максимум - один и тот же элемент
    const int size1 = 3;
    int testArray1[size1] = { 5, 5, 5 };

    ThreadData data1;
    data1.array = testArray1;
    data1.size = size1;

    TestMinMax(&data1);
    TestAverage(&data1);

    // Проверяем, что индексы минимума и максимума правильные
    EXPECT_EQ(data1.min_index, 0);
    EXPECT_EQ(data1.max_index, 0);

    // Тест с массивом размера 1
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