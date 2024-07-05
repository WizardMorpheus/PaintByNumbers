#version 330 core
layout(location = 0) out vec4 FragColor;

in vec2 texPos;

uniform sampler2D tex1;
uniform sampler2D tex2;


void main()
{
    vec4 col1 = texture(tex1, texPos);
    vec4 col2 = texture(tex2, texPos);
    FragColor = col1*(1.0-col2.a) + col2*col2.a;
}