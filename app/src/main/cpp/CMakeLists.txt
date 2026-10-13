cmake_minimum_required(VERSION 3.22.1)
project("sslsy")

set(CMAKE_CXX_STANDARD 17)

# Disable Dobby ASM trampoline (fixes NDK r29 Clang 21 build error)
set(BUILD_WITH_TRAMPOLINE_ASM OFF CACHE BOOL "" FORCE)

# Dobby Minimal Setup
add_subdirectory(dobby)

add_library(${CMAKE_PROJECT_NAME} SHARED
    module.cpp)

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/dobby/include)

target_link_libraries(${CMAKE_PROJECT_NAME}
    dobby
    log)