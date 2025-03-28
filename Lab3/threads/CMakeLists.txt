cmake_minimum_required(VERSION 3.10)

# Находим пакет для потоков
find_package(Threads REQUIRED)

# Создаем исполняемый файл
add_executable(marker_threads marker_threads.cpp)

# Подключаем библиотеку потоков
target_link_libraries(marker_threads PRIVATE Threads::Threads)

# Устанавливаем директорию для вывода
set_target_properties(marker_threads PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)