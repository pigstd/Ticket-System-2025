cmake_minimum_required(VERSION 3.22.1)
project(Ticket-System-2025)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)

include_directories(src)

# Ofast 优化
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Ofast")

add_executable(code
    main.cpp
)

# 输出可执行文件到项目根目录
set_target_properties(code PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}
)

message(STATUS "CMAKE_SOURCE_DIR: ${CMAKE_SOURCE_DIR}")

# 调试时可以打开，用来测指针和越界的问题
target_compile_options(code PRIVATE -fsanitize=address,leak,undefined)
target_link_libraries(code PRIVATE -fsanitize=address,leak,undefined)