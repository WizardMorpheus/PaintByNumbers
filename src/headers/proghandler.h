#ifndef QUANTIZER_H
#define QUANTIZER_H

#include "opengl.h"

#include <vector>

namespace PROGHANDLER {


    static GLuint quantizerProgramID;
    static GLuint segmentProgID;
    static GLuint overlayProgID;
    static GLuint happyMistakeProgID;
    static GLuint smootherProgID;
    static GLuint keymakerProgID;
    static GLuint VBO;
    static GLuint VAO;
    static GLuint EBO;
    
    
    /**
     * quantizes Tex into a new texture with the colors in colors
     * requires numColors == std::pow(2, n)
     */
    void quantize(GLuint tex, GLuint* quantizedTex, float* colorData, int numColors, int highlightedColor, int smooth, bool happyMistake);

    void genSegments(GLuint quantizedTex, GLuint *segmentTex);

    //stretches and overlays tex2 onto tex1 (will only work if tex2 conatains non 1 alpha values)
    void overlayTextures(GLuint tex1, GLuint tex2, GLuint *overlayTex, float* tex1UV0, float* tex1UV1, float* tex2UV0, float* tex2UV1);

    void generateKey(GLuint fontTex, GLuint* keyTex, float* colorData, int numColors);

    /**
     * sets up the quantizer program to be used
     */
    void setupPrograms();

    /**
     * deletes the quantizer program 
     */
    void closePrograms();

    /**
     * calculates the best representative colors for tex using colors->size() colors, and stores those colors in colors
     * colors is a vec4 representation rgba
     * depth affects the fitting of the result, higher depth, better results (maybe)
     */
    void calcBestColors(GLuint tex, float* colorData, int numColors, bool random);

    static float getDistance4(float u[4], float v[4]);
}

#endif