cmake_minimum_required(VERSION 3.14)
project(NamedPipesLab VERSION 1.0)

# Стандарт C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Директории включения
include_directories(${CMAKE_SOURCE_DIR}/include)

# Для Windows нужны определения для потоков
if(WIN32)
    add_definitions(-D_WIN32_WINNT=0x0601)
endif()

# Добавление сначала библиотек, потом исполняемых файлов
# Модифицируем порядок, чтобы избежать циклических зависимостей
add_subdirectory(winapi)
add_subdirectory(threads)
add_subdirectory(winapi-tests)
add_subdirectory(threads-tests)