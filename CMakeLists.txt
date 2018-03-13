cmake_minimum_required(VERSION 3.9)
project(ccenerate)

set(CMAKE_CXX_STANDARD 14)

add_subdirectory(vnd/cppast)
include_directories(vnd/cppast/include)

add_executable(ccenerate main.cpp)
target_link_libraries(ccenerate PRIVATE cppast)
