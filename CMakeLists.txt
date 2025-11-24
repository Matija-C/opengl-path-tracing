cmake_minimum_required(VERSION 3.24)
project(pathTracer)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)


add_subdirectory(vendor)
add_subdirectory(src)

target_include_directories(${PROJECT_NAME} PRIVATE include)
