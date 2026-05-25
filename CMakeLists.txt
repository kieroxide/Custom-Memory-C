cmake_minimum_required(VERSION 3.10)
project(MemoryManagement)

set(CMAKE_CXX_STANDARD 17)

add_executable(${PROJECT_NAME} src/main.cpp)
target_include_directories(${PROJECT_NAME} PRIVATE include/)

# Building testing with gtests:
enable_testing()

# Fetch GoogleTest automatically
include(FetchContent)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/heads/main.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

# Add the memory library
add_library(personalMemory
    src/personalMemory.cpp
    src/Heap.cpp
    src/Region.cpp
    src/Segment.cpp
)


target_include_directories(personalMemory PUBLIC include/)
target_link_libraries(${PROJECT_NAME} PRIVATE personalMemory)

# Test executable
add_executable(
    memoryTests
    tests/testMalloc.cpp
)

# Link against your code + gtest
target_link_libraries(
    memoryTests
    personalMemory
    GTest::gtest_main
)

# Register tests
include(GoogleTest)
gtest_discover_tests(memoryTests)
