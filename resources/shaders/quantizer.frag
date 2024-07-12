#version 330 core
layout(location = 0) out vec4 FragColor;

in vec2 texPos;

uniform sampler2D texToQuantize;
uniform vec4 colors[16];
uniform int numColors;
uniform int highlightedColor;

void main() {

    float minDist = length(colors[0] - texture(texToQuantize, texPos));
    FragColor = colors[0];
    for (int i = 1; i < numColors; i++) {
        float dist = length(colors[i] - texture(texToQuantize, texPos));
        if (dist < minDist) {
            minDist = dist;
            FragColor = colors[i];
        }
    }
    if (highlightedColor != -1 && FragColor != colors[highlightedColor]) {
        FragColor = vec4(1.0, 1.0, 1.0, 1.0);
    }

} 