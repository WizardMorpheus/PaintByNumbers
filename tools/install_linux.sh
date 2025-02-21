#!/bin/bash

if [ -f resources/.installed ]; then
    echo "Dependencies already installed"
    exit 0
fi

# GLFW
sudo apt install libglfw3-dev


# imgui
wget https://github.com/ocornut/imgui/archive/refs/tags/v1.91.8.zip -O imgui.zip
unzip imgui.zip
rm imgui.zip
mkdir -p include/imgui
cp -r imgui-1.91.8/*.h imgui-1.91.8/*.cpp imgui-1.91.8/backends/imgui_impl_glfw* imgui-1.91.8/backends/imgui_impl_opengl3* ./include/imgui/
rm -r imgui-1.91.8

# OPENGL
sudo apt-get install cmake xorg-dev libglu1-mesa-dev

touch resources/.installed