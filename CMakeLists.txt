cmake_minimum_required(VERSION 4.0)

project(JacRender)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set the build type to Release by default
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
endif()

# Set flags for debug/release builds
set(DEBUG_FLAGS -Wall -Wextra -Wpedantic -g -O0)
set(RELEASE_FLAGS -O3)

# Directories setup
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(INC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/inc)
set(RES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/res)

# Dependencies
find_package(Vulkan REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)

# Main executable
add_executable(${PROJECT_NAME})

target_sources(${PROJECT_NAME}
    PRIVATE
    ${SRC_DIR}/main.cpp
    ${SRC_DIR}/vulkan/Window.cpp)

target_include_directories(${PROJECT_NAME}
    PRIVATE
    ${INC_DIR})

target_link_libraries(${PROJECT_NAME}
    PRIVATE
    Vulkan::Vulkan
    glfw
    glm)

target_compile_options(${PROJECT_NAME}
    PRIVATE
    $<$<CONFIG:Debug>:${DEBUG_FLAGS}>
    $<$<CONFIG:Release>:${RELEASE_FLAGS}>)
