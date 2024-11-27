cmake_minimum_required(VERSION 3.10)

project(matrix C)

set(CMAKE_C_FLAGS "-std=c11")
set(CMAKE_C_FLAGS "-Wall")

find_package(PythonLibs 3 REQUIRED)

python_add_module(matrix matrix.c)

message(STATUS ${PYTHON_INCLUDE_DIRS})

include_directories(${PYTHON_INCLUDE_DIRS})
target_link_libraries(matrix ${PYTHON_LIBRARIES})
