cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/debug)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/release)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/release)

include_directories(${CMAKE_SOURCE_DIR}/include)

project(ajson)

add_executable(ajson ${CMAKE_SOURCE_DIR}/test/main.cpp)
target_link_libraries(ajson PUBLIC ${CMAKE_SOURCE_DIR}/libs/liblexer.a)