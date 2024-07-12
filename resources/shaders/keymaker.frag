#version 330 core
layout(location = 0) out vec4 FragColor;

in vec2 texPos;

uniform sampler2D fontTex;
uniform vec4 colors[16];
uniform int numColors;

void main() {

    if (texPos.y < 0.5) {
        // 65 is the same as 'A'
        float xPos = float((int(texPos.x * numColors) + 65) % 16) / 16 + (texPos.x * numColors - float(int(texPos.x * numColors))) / 16;
        float yPos = float((int(texPos.x * numColors) + 65) / 16) / 16 + texPos.y / 8;

        FragColor = texture(fontTex, vec2(xPos, yPos));
    } else {
        FragColor = colors[int(texPos.x * numColors)];
    }

} 