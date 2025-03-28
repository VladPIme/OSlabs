cmake_minimum_required(VERSION 3.10)

# Находим пакет для потоков
find_package(Threads REQUIRED)

# Получаем путь к Catch2
get_filename_component(CATCH_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../include" ABSOLUTE)

# Создаем исполняемый файл для тестов
add_executable(threads_tests threads_tests.cpp)

# Включаем директории с заголовочными файлами
target_include_directories(threads_tests PRIVATE ${CATCH_INCLUDE_DIR})

# Подключаем библиотеку потоков
target_link_libraries(threads_tests PRIVATE Threads::Threads)

# Устанавливаем директорию для вывода
set_target_properties(threads_tests PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# Добавляем тест
add_test(NAME ThreadsTests COMMAND threads_tests)