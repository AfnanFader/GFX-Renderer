@echo off

glslc ./Assets/Shaders/simple_shader.frag -o ./Assets/Compiled_Shaders/simple_frag_shader.spv
glslc ./Assets/Shaders/simple_shader.vert -o ./Assets/Compiled_Shaders/simple_vert_shader.spv

pause