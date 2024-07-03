#version 330 core
layout(location = 0) out vec4 FragColor;

in vec2 texPos;

uniform sampler2D texToQuantize;
uniform sampler2D colors;
uniform int numColors;

void main()
{
    float minDist = length(texture(colors, vec2(0, 0.5)) - texture(texToQuantize, texPos));
    FragColor = texture(colors, vec2(0, 0.5));
    for (float i = 1; i < numColors; i++) {
        float dist = length(texture(colors, vec2(i/numColors, 0.5)) - texture(texToQuantize, texPos));
        if (dist < minDist) {
            minDist = dist;
            FragColor = texture(colors, vec2(i/numColors, 0.5));
        }
    }
    
    //FragColor = texture(colors, texPos);
} 