# IA Client Module CMakeLists.txt

cmake_minimum_required(VERSION 3.14)
project(LogosIaPlugin LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include the Logos Module CMake helper
if(DEFINED ENV{LOGOS_MODULE_BUILDER_ROOT})
    include($ENV{LOGOS_MODULE_BUILDER_ROOT}/cmake/LogosModule.cmake)
elseif(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/cmake/LogosModule.cmake")
    include(cmake/LogosModule.cmake)
else()
    message(FATAL_ERROR "LogosModule.cmake not found")
endif()

# Define the module
logos_module(
    NAME logos_ia
    SOURCES 
        src/ia_interface.h
        src/ia_interface.rep
        src/ia_plugin.h
        src/ia_plugin.cpp
)
