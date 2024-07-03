#version 330 core
layout(location = 0) out vec4 FragColor;

in vec2 texPos;

uniform sampler2D texToQuantize;
uniform sampler2D colors;
uniform int numColors;

uniform int texWidth;
uniform int texHeight;

void main()
{
    // // get the current color we are changing
    // vec4 crntCol = texture(colors, texPos.xy);

    // vec4 avgCol = vec4(0,0,0,0);
    // int avgCount = 0;

    // // loop through the image to be quantized, if a pixel in the image is closest in color to the current color compared
    // // to the other quantization colors, that colors nearest neigbor is the current color and so we add it to the average color
    // for (float x = 0; x < 1.0; x += 1/float(texWidth)) {
    //     for (float y = 0; y < 1.0; y += 1/float(texHeight)) {

    //         vec4 closestCol = crntCol;
    //         int splitIndex = (numColors + numColors%2) /2;

    //         for (float i = 0; i < 1.0; i += 1/float(splitIndex)) {
    //             for (float j = 0.25; j <= 0.75; j += 0.5) {
    //                 if (length(texture(texToQuantize, vec2(x,y)) - texture(colors, vec2(i, j))) < 
    //                     length(texture(texToQuantize, vec2(x,y)) - closestCol)) {
    //                     closestCol = texture(colors, vec2(i, j));
    //                     break;
    //                 }
    //             }
    //             if (closestCol != crntCol) break;                
    //         }

    //         if (closestCol == crntCol) {
    //             avgCount++;
    //             avgCol += texture(texToQuantize, vec2(x,y));
    //         }

    //     }
    // }

    // avgCol /= avgCount;
    // FragColor = avgCol;
    FragColor = texture(colors, texPos) * 2;

    //FragColor = vec4(texPos, 0.0, 1.0);
} 