#ifndef QUANTIZER_H
#define QUANTIZER_H

#include "opengl.h"

#include <vector>

namespace QUANTIZER {


    static GLuint qProgramID;
    static GLuint qVBO;
    static GLuint qVAO;
    static GLuint qEBO;
    
    
    /**
     * quantizes Tex with numColors colors, into a new texture and set QuantizedTex to be a pointer to it.
     * requires numColors == std::pow(2, n)
     */
    void quantize(GLuint tex, GLuint* quantizedTex, int numColors, std::vector<float*>* colors);

    /**
     * sets up the quantizer program to be used
     */
    void setupQuantizer();

    /**
     * deletes the quantizer program 
     */
    void closeQuantizer();
}

#endif