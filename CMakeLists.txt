cmake_minimum_required(VERSION 3.10)
project(DirectoryMonitor)

# Set the C++ standard to use
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add the executable
add_executable(DirectoryMonitor main.cpp)

# Link with the Windows libraries if building on Windows
if(WIN32)
    target_link_libraries(DirectoryMonitor PRIVATE kernel32 user32)
endif()
