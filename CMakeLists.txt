cmake_minimum_required(VERSION 3.22)

project(GfxRenderer)

if (WIN32)
    set(GLFW_LIB "${CMAKE_SOURCE_DIR}/lib/GLFW/lib-vc2022")
elseif (APPLE) # Note that this support x86_64 and arm64 MAC
    set(GLFW_LIB "${CMAKE_SOURCE_DIR}/lib/GLFW/lib-universal")
endif()

# Find Vulkan
find_package(Vulkan REQUIRED)

# Fetch certain external libraries via git
include(FetchContent)

FetchContent_Declare(
    glm
    GIT_REPOSITORY "https://github.com/g-truc/glm.git"
    GIT_TAG "1.0.1"
    GIT_SHALLOW TRUE
)

FetchContent_Declare(
    spdlog
    GIT_REPOSITORY "https://github.com/gabime/spdlog.git"
    GIT_TAG "v1.15.0"
    GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(glm)
FetchContent_MakeAvailable(spdlog)

file(GLOB_RECURSE SOURCES "${CMAKE_SOURCE_DIR}/src/*.cpp")

# 3rd Party Libraries headers
include_directories("${CMAKE_SOURCE_DIR}/include")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(${PROJECT_NAME} ${SOURCES})

# Dynamic Libraries
link_directories(${GLFW_LIB})

target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_SOURCE_DIR}/include # 3rd Party Libraries headers
    ${PROJECT_SOURCE_DIR}/src # All headers in the src folder
    glm
    spdlog
)

target_link_libraries(${PROJECT_NAME}
    Vulkan::Vulkan
    glm
    spdlog
)

# Link to GLFW Pre-compiled binaries
# Also Copy the dynamic library file to output directory after build
if (WIN32)
    target_link_libraries(${PROJECT_NAME} "${GLFW_LIB}/glfw3.lib")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${GLFW_LIB}/glfw3.dll" $<TARGET_FILE_DIR:${PROJECT_NAME}>
    )
elseif (APPLE)
    target_link_libraries(${PROJECT_NAME} "${GLFW_LIB}/glfw3.a") 
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${GLFW_LIB}/glfw3.dylib" $<TARGET_FILE_DIR:${PROJECT_NAME}>
    )
endif()