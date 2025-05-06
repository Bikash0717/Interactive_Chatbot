cmake_minimum_required(VERSION 3.10)
project(ChatBotusingDSA)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Find required packages
find_package(Threads REQUIRED)

# Add nlohmann/json
include(FetchContent)
FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz)
FetchContent_MakeAvailable(json)

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/src/headers
    ${CMAKE_SOURCE_DIR}/src
)

# Source files
set(SERVER_SOURCES
    src/cpp/DSAServer.cpp
    src/cpp/DecisionTree.cpp
    src/cpp/Graph.cpp
    src/cpp/HashTable.cpp
    src/cpp/PriorityQueue.cpp
    src/cpp/Trie.cpp
    src/cpp/DSAKnowledge.cpp
    src/cpp/main.cpp
)

# Server executable
add_executable(ChatBotServer ${SERVER_SOURCES})

# Link libraries
target_link_libraries(ChatBotServer
    PRIVATE
    Threads::Threads
    nlohmann_json::nlohmann_json
)

# Installation
install(TARGETS ChatBotServer
    RUNTIME DESTINATION bin
)
