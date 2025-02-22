@echo off

if exist ./resources/.installed exit

:: GLFW
powershell -Command "Invoke-WebRequest -OutFile file.zip -Uri https://github.com/glfw/glfw/releases/download/3.4/glfw-3.4.bin.WIN64.zip"
tar -xf file.zip
del file.zip
xcopy glfw-3.4.bin.WIN64\include\* include /E /I /Y
xcopy glfw-3.4.bin.WIN64\lib-vc2022\* lib /E /I /Y
rmdir /S /Q glfw-3.4.bin.WIN64

:: imgui
powershell -Command "Invoke-WebRequest -OutFile file.zip -Uri https://github.com/ocornut/imgui/archive/refs/tags/v1.91.8.zip"
tar -xf file.zip
del file.zip
xcopy imgui-1.91.8\*.h include\imgui /Y
xcopy imgui-1.91.8\*.cpp include\imgui /Y
xcopy imgui-1.91.8\backends\imgui_impl_opengl3* include\imgui /Y
rmdir /S /Q imgui-1.91.8

echo Installation complete.

powershell -Command "New-Item -Path './resources/.installed' -ItemType File"