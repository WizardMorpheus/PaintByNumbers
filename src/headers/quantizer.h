#ifndef QUANTIZER_H
#define QUANTIZER_H

#include "opengl.h"

#include <vector>

namespace QUANTIZER {


    static GLuint qProgramID;
    static GLuint qSegmentProgID;
    static GLuint qOverlayProgID;
    static GLuint qHappyMistakeProgID;
    static GLuint qSmootherProgID;
    static GLuint qVBO;
    static GLuint qVAO;
    static GLuint qEBO;
    
    
    /**
     * quantizes Tex into a new texture with the colors in colors
     * requires numColors == std::pow(2, n)
     */
    void quantize(GLuint tex, GLuint* quantizedTex, float* colorData, int numColors, int highlightedColor, int smooth, bool happyMistake);

    void genSegments(GLuint quantizedTex, GLuint *segmentTex);

    //stretches and overlays tex2 onto tex1 (will only work if tex2 conatains non 1 alpha values)
    void overlayTextures(GLuint tex1, GLuint tex2, GLuint *overlayTex, float* tex1UV0, float* tex1UV1, float* tex2UV0, float* tex2UV1);

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
    void calcBestColors(GLuint tex, float* colorData, int numColors, bool random);

    static float getDistance4(float u[4], float v[4]);
}

#endif