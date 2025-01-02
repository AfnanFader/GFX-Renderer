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

FetchContent_Declare(
    spdlog
    GIT_REPOSITORY "https://github.com/gabime/spdlog.git"
    GIT_TAG "v1.15.0"
    GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(glm)
FetchContent_MakeAvailable(glfw)
FetchContent_MakeAvailable(spdlog)

# Vulkan Dependencies
find_package(Vulkan REQUIRED)

# Compiler settings
set(CMAKE_BUILD_TYPE debug)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add subdirectories and other external libraries
add_subdirectory("src/Renderer")

# Load all headers
include_directories(${CMAKE_SOURCE_DIR}/src)

# List out all the source files in /src
file(GLOB_RECURSE MainSource CONFIGURE_DEPENDS "src/*.cpp")

# Creating executable
add_executable(${PROJECT_NAME} "${MainSource}")

# Linking libraries
target_link_libraries(${PROJECT_NAME} PRIVATE Renderer)
target_link_libraries(${PROJECT_NAME} PRIVATE Vulkan::Vulkan)
target_link_libraries(${PROJECT_NAME} PRIVATE glm)
target_link_libraries(${PROJECT_NAME} PRIVATE glfw)
target_link_libraries(${PROJECT_NAME} PRIVATE spdlog)