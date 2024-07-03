#ifndef QUANTIZER_H
#define QUANTIZER_H

#include "opengl.h"

#include <vector>

namespace QUANTIZER {


    static GLuint qProgramID;
    static GLuint qColorProgID;
    static GLuint qVBO;
    static GLuint qVAO;
    static GLuint qEBO;
    
    
    /**
     * quantizes Tex into a new texture with the colors in colors
     * requires numColors == std::pow(2, n)
     */
    void quantize(GLuint tex, GLuint* quantizedTex, std::vector<float*>* colors);

    /**
     * sets up the quantizer program to be used
     */
    void setupQuantizer();

    /**
     * deletes the quantizer program 
     */
    void closeQuantizer();

    /**
     * calculates the best representative colors for tex using colors->size() colors, and stores those colors in colors
     * colors is a vec4 representation rgba
     * depth affects the fitting of the result, higher depth, better results (maybe)
     */
    void calcBestColors(GLuint tex, std::vector<float*>* colors, bool random);

    static float getDistance4(float u[4], float v[4]);
}

#endif