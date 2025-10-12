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
find_package(assimp REQUIRED)

# Fetch external dependencies
include(FetchContent)

# VulkanMemoryAllocator
FetchContent_Declare(
    VulkanMemoryAllocator
    GIT_REPOSITORY https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator.git
    GIT_TAG v3.1.0)
FetchContent_MakeAvailable(VulkanMemoryAllocator)

# stb_image - header-only library
FetchContent_Declare(
    stb
    GIT_REPOSITORY https://github.com/nothings/stb.git
    GIT_TAG master)
FetchContent_MakeAvailable(stb)

# Create an interface library for stb
add_library(stb INTERFACE)
target_include_directories(stb INTERFACE ${stb_SOURCE_DIR})

# Main executable
add_executable(${PROJECT_NAME})

target_sources(${PROJECT_NAME}
    PRIVATE
    ${SRC_DIR}/main.cpp
    # utilities
    ${SRC_DIR}/common/utils.cpp
    # Implementation wrappers for external libraries
    ${SRC_DIR}/core/memory/vma.cpp
    ${SRC_DIR}/core/memory/stb_image.cpp
    # Device management
    ${SRC_DIR}/core/device/Instance.cpp
    ${SRC_DIR}/core/device/Device.cpp
    ${SRC_DIR}/core/device/Surface.cpp
    # Pipeline management
    ${SRC_DIR}/core/pipeline/Framebuffer.cpp
    ${SRC_DIR}/core/pipeline/Pipeline.cpp
    ${SRC_DIR}/core/pipeline/Swapchain.cpp
    ${SRC_DIR}/core/pipeline/Shader.cpp
    # Memory management
    #
    # Commands and command buffers
    ${SRC_DIR}/core/commands/CommandPool.cpp
    ${SRC_DIR}/core/commands/CommandBuffer.cpp
    # Low level vulkan helpers
    ${SRC_DIR}/vulkan/utils.cpp
    ${SRC_DIR}/vulkan/api.cpp
    # Shader definitions
    ${SRC_DIR}/shader/defs_global.cpp
    ${SRC_DIR}/shader/defs_material.cpp
    ${SRC_DIR}/shader/defs_instance.cpp
    # High level abstractions
    ${SRC_DIR}/Window.cpp
    ${SRC_DIR}/Renderer.cpp
    ${SRC_DIR}/MemoryManager.cpp
    ${SRC_DIR}/Camera.cpp)

target_include_directories(${PROJECT_NAME}
    PRIVATE
    ${INC_DIR})

target_link_libraries(${PROJECT_NAME}
    PRIVATE
    Vulkan::Vulkan
    glfw
    glm
    stb
    VulkanMemoryAllocator
    assimp)

target_compile_options(${PROJECT_NAME}
    PRIVATE
    $<$<CONFIG:Debug>:${DEBUG_FLAGS}>
    $<$<CONFIG:Release>:${RELEASE_FLAGS}>)

target_compile_definitions(${PROJECT_NAME}
    PRIVATE
    SHADER_DIR_BASE="${CMAKE_CURRENT_SOURCE_DIR}/shaders/"
    TEXTURE_DIR="${CMAKE_CURRENT_SOURCE_DIR}/textures/"
)

# compile shaders
add_subdirectory(shaders)
add_dependencies(${PROJECT_NAME} shaders)
