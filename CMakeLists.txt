cmake_minimum_required(VERSION 3.22)
project(GfxRenderer)

if(WIN32)
    # Windows-specific settings
    add_definitions(-DPLATFORM_WINDOWS)
elseif(APPLE)
    # macOS-specific settings
    add_definitions(-DPLATFORM_MAC)
endif()

# External Dependencies
include(FetchContent)
FetchContent_Declare(
    glm
    GIT_REPOSITORY "https://github.com/g-truc/glm.git"
    GIT_TAG "1.0.1"
    GIT_SHALLOW TRUE
)

FetchContent_Declare(
    glfw
    GIT_REPOSITORY "https://github.com/glfw/glfw.git"
    GIT_TAG "3.4"
    GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(glm)
FetchContent_MakeAvailable(glfw)

# Vulkan Dependencies
find_package(Vulkan REQUIRED)
file(GLOB_RECURSE VulkanEngineSources CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
)

# C++ Standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Build Config
set(CMAKE_BUILD_TYPE debug)

# Build targets and libraries
add_executable(${PROJECT_NAME} ${VulkanEngineSources})
target_link_libraries(${PROJECT_NAME} PRIVATE Vulkan::Vulkan)
target_link_libraries(${PROJECT_NAME} PRIVATE glm)
target_link_libraries(${PROJECT_NAME} PRIVATE glfw)
target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)