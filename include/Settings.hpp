#ifndef SETTINGS_HPP
#define SETTINGS_HPP
#pragma once

// Enables the Vulkan Validation Layer, will cause some performance drop.
#define ENABLE_VULKAN_VALIDATION 1

// Temp settings for windows control
#define DISPLAY_WIDTH 800
#define DISPLAY_HEIGHT 600
#define MAX_FRAMES_IN_FLIGHT 2;

// Temp shader location <-- might use json for future proofing
#define VERT_SHADER_PATH "/Assets/Compiled_Shaders/simple_vert_shader.spv"
#define FRAG_SHADER_PATH "/Assets/Compiled_Shaders/simple_frag_shader.spv"

#endif