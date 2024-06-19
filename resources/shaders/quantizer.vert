#version 330 core
layout (location = 0) in vec3 aPos;

out vec2 texPos;

void main()
{
    texPos = (aPos.xy + vec2(1, 1)) / 2;
    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
}