project(MicroHedger)
cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(INCLUDE_DIR "/opt/homebrew/include")
include_directories(${INCLUDE_DIR})
find_package(Boost REQUIRED)

add_subdirectory(utils)
add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} lob_lib)
