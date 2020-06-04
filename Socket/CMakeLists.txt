cmake_minimum_required(VERSION 3.5)

project(Socket-cpp)

add_compile_options(${CMAKE_CXX_FLAGS} -Wall)

# Settings
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/build/bin)

# Constant name
set(TARGET_SERVER server)
set(TARGET_CLIENT client)

# Include dynamic libraries
find_package(Threads REQUIRED)

# Bring header file into project
include_directories(
    ${CMAKE_SOURCE_DIR}
)

# Create base objects
set(BASE_OBJECTS CSocket.cpp utils.cpp)

# Executable for server
add_executable(${TARGET_SERVER} ${BASE_OBJECTS} server.cpp)

target_link_libraries(${TARGET_SERVER} 
    Threads::Threads
)

# Executable for client
add_executable(${TARGET_CLIENT} ${BASE_OBJECTS} client.cpp)

target_link_libraries(${TARGET_CLIENT} 
    Threads::Threads
)

