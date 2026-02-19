cmake_minimum_required(VERSION 3.10)
project(ConsoleGame)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "include/*.hpp")

include_directories(${CMAKE_SOURCE_DIR}/include)

add_executable(ConsoleGame ${SOURCES} ${HEADERS})

if(WIN32)
    target_link_libraries(ConsoleGame PRIVATE user32)
endif()