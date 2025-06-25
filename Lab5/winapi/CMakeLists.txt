# Создаем сначала статические библиотеки
add_library(winapi_server_lib STATIC
    src/server.cpp
)

add_library(winapi_client_lib STATIC
    src/client.cpp
)

# Указание директорий включения для библиотек
target_include_directories(winapi_server_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_include_directories(winapi_client_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Затем создаем исполняемые файлы
add_executable(winapi_server_exe
    src/server_main.cpp
)

add_executable(winapi_client_exe
    src/client_main.cpp
)

# Линковка библиотек
target_link_libraries(winapi_server_exe PRIVATE winapi_server_lib winapi_client_lib)
target_link_libraries(winapi_client_exe PRIVATE winapi_client_lib)