cmake_minimum_required(VERSION 3.16)
project(signpost LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Sources
set(SOURCES
    src/main.cpp
    src/config.cpp
    src/network/server.cpp
    src/network/client.cpp
    src/protocol/buffer.cpp
    src/protocol/packet.cpp
    src/handler/handshake.cpp
    src/handler/status.cpp
    src/handler/login.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include)

# Link pthread on Unix
if(UNIX)
    target_link_libraries(${PROJECT_NAME} PRIVATE pthread)
endif()

# Optimize for release
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(${PROJECT_NAME} PRIVATE -O2)
endif()
