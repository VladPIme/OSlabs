cmake_minimum_required(VERSION 3.10)

# Создаем исполняемый файл
add_executable(marker_winapi marker_winapi.cpp)

# Устанавливаем директорию для вывода
set_target_properties(marker_winapi PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)