# Set the minimum required version of CMake
cmake_minimum_required(VERSION 3.0)

# Define your project and its name
project(task_one)

# Set the C++ standard (e.g., C++11, C++14, C++17, etc.)
set(CMAKE_CXX_STANDARD 11)

# List your source files
set(SOURCES
    task_one.cpp
    sqlite3.c
    # Add more source files as needed
)



#Add an executable target
add_executable(task ${SOURCES})

set(CMAKE_BUILD_TYPE Debug)

# Find SQLite3 package
#find_package(SQLite3 REQUIRED)

# Add the SQLite3 library to your project
#target_link_libraries(task_one SQLite::SQLite3)
# Include the directory containing sqlite3.h
target_include_directories(task PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Link your project with the SQLite library
target_link_libraries(task PRIVATE "E:/project/challenge/sqlite3.lib")

# Optionally, you can link libraries to your executable
# target_link_libraries(YourExecutableName YourLibrary)

# You can also set compiler flags if needed
# target_compile_options(YourExecutableName PRIVATE -Wall -Wextra)
