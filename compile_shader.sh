#!/bin/bash

# Directories
current_dir=$(pwd)
shader_dir="$current_dir/Assets/Shaders/"
output_dir="$current_dir/Assets/Compiled_Shaders/"

# Check if the directory exists
if [ ! -d "$output_dir" ]; then
    mkdir -p "$output_dir"
    echo "Compiled Directory created: $output_dir"
else
    echo "Compiled Directory already exists: $output_path"
fi

echo "[Compile] Compiling Vertex Shader"
for shader in "$shader_dir"/*.vert; do 
# Extract the base name of the shader file 
    shader_name=$(basename -- "$shader")
    # Compile the shader
    glslc "$shader" -o "$output_dir${shader_name}.spv"
    # Check if compilation was successful
    if [ ! $? -eq 0 ]; then
        echo "Failed to compile $shader_name."
    fi
done

echo "[Compile] Compiling Fragment Shader"
for shader in "$shader_dir"/*.frag; do 
# Extract the base name of the shader file 
    shader_name=$(basename -- "$shader")
    # Compile the shader
    glslc "$shader" -o "$output_dir${shader_name}.spv"
    # Check if compilation was successful
    if [ ! $? -eq 0 ]; then
        echo "Failed to compile $shader_name."
    fi
done

echo "Exited Script"
