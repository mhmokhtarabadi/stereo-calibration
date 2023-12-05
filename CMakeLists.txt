cmake_minimum_required(VERSION 3.10)

project(camera_calibration)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

add_compile_definitions(DIRECTORY_PATH="${CMAKE_SOURCE_DIR}")

find_package(OpenCV REQUIRED)

add_subdirectory(src)