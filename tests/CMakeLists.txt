# Minimal CMakeLists.txt for integration tests (T1.7)
cmake_minimum_required(VERSION 3.14)
project(LogosIaTests LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Network)
qt_standard_project_setup()

add_executable(test_minimal test_minimal.cpp ../src/search_client.cpp)
target_link_libraries(test_minimal PRIVATE Qt6::Network)
target_include_directories(test_minimal PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/.. ${CMAKE_CURRENT_SOURCE_DIR}/../src)
install(TARGETS test_minimal RUNTIME DESTINATION bin)
