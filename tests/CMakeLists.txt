# Minimal CMakeLists.txt for integration tests
cmake_minimum_required(VERSION 3.14)
project(LogosIaTests LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Network Test)
qt_standard_project_setup()

add_executable(test_search_client test_search_client.cpp)
target_link_libraries(test_search_client PRIVATE Qt6::Network Qt6::Test)
target_include_directories(test_search_client PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/..)
