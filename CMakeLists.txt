cmake_minimum_required(VERSION 3.0)
project(CompressionCourse)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(CompressionCourse main.cc)
target_include_directories(CompressionCourse PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/schemes)
