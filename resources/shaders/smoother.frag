#version 330 core
layout(location = 0) out vec4 FragColor;

in vec2 texPos;

uniform sampler2D texToQuantize;
uniform int texWidth;
uniform int texHeight;

void main()
{

    int sim = 0;
    vec4 nearColors[8];
    int nearColorCount[8];
    int numNearColors = 0;
    for (int i = 0; i < 8; i++) {
        nearColors[i] = vec4(0.0, 0.0, 0.0, 1.0);
        nearColorCount[i] = 0; 
    }

    float deltaX = 1/float(texWidth);
    float deltaY = 1/float(texHeight);

    for (float x = texPos.x - deltaX; x <= texPos.x + deltaX; x += deltaX) {
        for (float y = texPos.y - deltaY; y <= texPos.y + deltaY; y += deltaY) {
            if (x < 0 || y < 0 || (x == texPos.x && y == texPos.y)) continue;
            if (texture(texToQuantize, vec2(x,y)) != texture(texToQuantize, texPos)) {
                for (int i = 0; i <= numNearColors; i++) {
                    if (nearColors[i] == texture(texToQuantize, vec2(x,y))) nearColorCount[i]++;
                    if (i == numNearColors) {
                        nearColorCount[i]++;
                        nearColors[i] = texture(texToQuantize, vec2(x,y));
                    }
                }
            } else {
                sim++;
            }
        }
    }

    if (sim < 4) {
        int bestColor = 0;
        FragColor = nearColors[0];
        for (int i = 1; i < numNearColors; i++) {
            if (nearColorCount[i] > nearColorCount[bestColor]) FragColor = nearColors[i];
        }
    } else {
        FragColor = texture(texToQuantize, texPos);
    }

    
    
} 