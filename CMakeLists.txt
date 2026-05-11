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

# Find Qt with RemoteObjects support
logos_find_qt()

# Define the module
logos_module(
    NAME logos_ia
    SOURCES 
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ia_plugin.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ia_plugin.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/search_client.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/search_client.cpp
)

# Find repc tool for Qt Remote Objects code generation
find_program(REPC_TOOL NAMES repc PATHS /usr/libexec /nix/store/*/libexec NO_DEFAULT_PATH)
if(NOT REPC_TOOL)
    find_program(REPC_TOOL NAMES repc)
endif()

if(REPC_TOOL AND EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/src/ia_backend.rep)
    message(STATUS "Found repc: ${REPC_TOOL}")
    
    # Generate both source and replica headers
    set(REPC_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated_code")
    set(REPC_SOURCE_HEADER "${REPC_OUTPUT_DIR}/ia_backend_source.h")
    set(REPC_REPLICA_HEADER "${REPC_OUTPUT_DIR}/ia_backend_replica.h")
    
    add_custom_target(repc_ia_backend ALL
        COMMAND ${CMAKE_COMMAND} -E make_directory ${REPC_OUTPUT_DIR}
        COMMAND ${REPC_TOOL} -o source ${CMAKE_CURRENT_SOURCE_DIR}/src/ia_backend.rep ${REPC_SOURCE_HEADER}
        COMMAND ${REPC_TOOL} -o replica ${CMAKE_CURRENT_SOURCE_DIR}/src/ia_backend.rep ${REPC_REPLICA_HEADER}
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/ia_backend.rep
        COMMENT "Generating Qt Remote Objects code for ia_backend.rep"
        VERBATIM
    )
    
    # Add include directory for generated headers
    target_include_directories(logos_ia_module_plugin PRIVATE ${REPC_OUTPUT_DIR})
    add_dependencies(logos_ia_module_plugin repc_ia_backend)
else()
    message(WARNING "repc tool not found or .rep file missing — Qt Remote Objects code generation skipped")
endif()
