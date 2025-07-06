cmake_minimum_required(VERSION 3.10)
project(MicroHedger)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(INCLUDE_DIR 
   "/opt/homebrew/include"
)
include_directories(${INCLUDE_DIR})
find_package(Boost CONFIG REQUIRED)

set(CMAKE_PREFIX_PATH 
    "../matplotlib-cpp"
    "/opt/homebrew/include"
)
find_package(matplotlib_cpp REQUIRED)

add_subdirectory(libs)
add_subdirectory(tests)

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} matplotlib_cpp::matplotlib_cpp)

# target_link_libraries(${PROJECT_NAME} utils_lib)
# target_link_libraries(${PROJECT_NAME} lob_lib) # contains boost
# target_link_libraries(${PROJECT_NAME} option_lib) # contains utils_lib
# target_link_libraries(${PROJECT_NAME} players_lib) # contains lob_lib, option_lib
target_link_libraries(${PROJECT_NAME} path_lib) # contains players_lib