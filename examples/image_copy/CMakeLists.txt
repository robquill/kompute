cmake_minimum_required(VERSION 3.20)
project(kompute_image_copy)

set(CMAKE_CXX_STANDARD 14)

# Options
option(KOMPUTE_OPT_GIT_TAG "The tag of the repo to use for the example" v0.9.0)
option(KOMPUTE_OPT_FROM_SOURCE "Whether to build example from source or from git fetch repo" ON)

if(KOMPUTE_OPT_FROM_SOURCE)
    add_subdirectory(../../ ${CMAKE_CURRENT_BINARY_DIR}/kompute_build)
else()
    include(FetchContent)
    FetchContent_Declare(kompute GIT_REPOSITORY https://github.com/KomputeProject/kompute.git
        GIT_TAG ${KOMPUTE_OPT_GIT_TAG})
    FetchContent_MakeAvailable(kompute)
    include_directories(${kompute_SOURCE_DIR}/src/include)
endif()

# Compiling shader
# To add more shaders simply copy the vulkan_compile_shader command and replace it with your new shader
vulkan_compile_shader(
    INFILE shader/image_copy.comp
    OUTFILE shader/image_copy.hpp
    NAMESPACE "shader")

vulkan_compile_shader(
    INFILE shader/texture_copy.comp
    OUTFILE shader/texture_copy.hpp
    NAMESPACE "shader")

vulkan_compile_shader(
    INFILE shader/texture_blur.comp
    OUTFILE shader/texture_blur.hpp
    NAMESPACE "shader")

# Then add it to the library, so you can access it later in your code
add_library(shader INTERFACE "shader/image_copy.hpp" "shader/texture_copy.hpp" "shader/texture_blur.hpp")
target_include_directories(shader INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

# Setting up main example code

# Download stb_image.h if not present
set(STB_IMAGE_PATH "${CMAKE_CURRENT_BINARY_DIR}/src/stb_image.h")
if(NOT EXISTS ${STB_IMAGE_PATH})
    message(STATUS "Downloading stb_image.h...")
    file(DOWNLOAD
        "https://raw.githubusercontent.com/nothings/stb/refs/heads/master/stb_image.h"
        ${STB_IMAGE_PATH}
        STATUS status
        LOG log)
    list(GET status 0 status_code)
    if(NOT status_code EQUAL 0)
        message(FATAL_ERROR "Failed to download stb_image.h: ${log}")
    endif()
endif()

# Download stb_image_write.h if not present
set(STB_IMAGE_WRITE_PATH "${CMAKE_CURRENT_BINARY_DIR}/src/stb_image_write.h")
if(NOT EXISTS ${STB_IMAGE_WRITE_PATH})
    message(STATUS "Downloading stb_image_write.h...")
    file(DOWNLOAD
        "https://raw.githubusercontent.com/nothings/stb/refs/heads/master/stb_image_write.h"
        ${STB_IMAGE_WRITE_PATH}
        STATUS status
        LOG log)
    list(GET status 0 status_code)
    if(NOT status_code EQUAL 0)
        message(FATAL_ERROR "Failed to download stb_image_write.h: ${log}")
    endif()
endif()

add_executable(kompute_image_copy src/main.cpp)
target_include_directories(kompute_image_copy PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/src)
target_link_libraries(kompute_image_copy PRIVATE shader kompute::kompute)

