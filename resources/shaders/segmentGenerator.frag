#version 330 core
layout(location = 0) out vec4 FragColor;

in vec2 texPos;

uniform sampler2D texToSegment;

uniform int texWidth;
uniform int texHeight;

void main()
{
    float diff = 0;
    float deltaX = 1/float(texWidth);
    float deltaY = 1/float(texHeight);

    for (float x = texPos.x - deltaX; x <= texPos.x + deltaX; x += deltaX) {
        for (float y = texPos.y - deltaY; y <= texPos.y + deltaY; y += deltaY) {
            if (x < 0 || y < 0 || (x == texPos.x && y == texPos.y)) continue;
            if (texture(texToSegment, vec2(x,y)) != texture(texToSegment, texPos)) diff++;
        }
    }

    diff /= 6;
    FragColor = vec4(1.0-diff, 1.0-diff, 1.0-diff, diff);

}