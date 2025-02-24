#include "proghandler.h"

#include "opengl.h"
#include "glWrap.h"
#include <iostream>
#include <cmath>
#include <random>
#include <chrono>
#include <filesystem>

void PROGHANDLER::quantize(GLuint tex, GLuint *quantizedTex, float* colorData, int numColors, int highlightedColor, int smooth, bool happyMistake) {
    
    if (PROGHANDLER::quantizerProgramID == 0) return;

    // setup to render
    

    int dims[2];
    GLWRAP::queryTex(tex, dims, GL_TEXTURE_2D);

    // The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
    GLuint Framebuffer = 0;
    glGenFramebuffers(1, &Framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer);


    GLuint tempTex;

    // The texture we're going to render to
    glGenTextures(1, &tempTex);

    // "Bind" the newly created texture : all future texture functions will modify this texture
    glBindTexture(GL_TEXTURE_2D, tempTex);


    // Give an empty image to OpenGL ( the last "0" )
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, dims[0], dims[1], 0,GL_RGB, GL_UNSIGNED_BYTE, 0);

    // Poor filtering. Needed !
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    // Set "renderedTexture" as our colour attachement #0
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, tempTex, 0);

    // Set the list of draw buffers.
    GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers


    // Always check that our framebuffer is ok
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        glDeleteTextures(1, &tempTex);
        tempTex = 0;
        return;
    }


    // Render to our framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer);
    glViewport(0, 0, dims[0], dims[1]); // Render on the whole framebuffer, complete from the lower left corner to the upper right


    glBindVertexArray(PROGHANDLER::VAO);
    glBindBuffer(GL_ARRAY_BUFFER, PROGHANDLER::VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, PROGHANDLER::EBO);

    glUseProgram(PROGHANDLER::quantizerProgramID);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex);

    glUniform1i(glGetUniformLocation(PROGHANDLER::quantizerProgramID, "texToQuantize"), 0);
    glUniform4fv(glGetUniformLocation(PROGHANDLER::quantizerProgramID, "colors"), numColors, colorData);
    glUniform1i(glGetUniformLocation(PROGHANDLER::quantizerProgramID, "numColors"), numColors);
    glUniform1i(glGetUniformLocation(PROGHANDLER::quantizerProgramID, "highlightedColor"), highlightedColor);
    

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); // 3 vertices * 2 triangles


    for (int i = 0; i < smooth; i++) {

        // The texture we're going to render to
        glGenTextures(1, quantizedTex);

        // "Bind" the newly created texture : all future texture functions will modify this texture
        glBindTexture(GL_TEXTURE_2D, *quantizedTex);


        // Give an empty image to OpenGL ( the last "0" )
        glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA, dims[0], dims[1], 0,GL_RGB, GL_UNSIGNED_BYTE, 0);

        // Poor filtering. Needed !
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, *quantizedTex, 0);
        // Set the list of draw buffers.
        GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
        glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers

        // Always check that our framebuffer is ok
        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            glDeleteTextures(1, quantizedTex);
            quantizedTex = 0;
            return;
        }

        // Render to our framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer);
        glViewport(0,0,dims[0],dims[1]); // Render on the whole framebuffer, complete from the lower left corner to the upper right


        glBindVertexArray(PROGHANDLER::VAO);

        glUseProgram(PROGHANDLER::smootherProgID);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tempTex);

        glUniform1i(glGetUniformLocation(PROGHANDLER::smootherProgID, "texToQuantize"), 0);
        glUniform1i(glGetUniformLocation(PROGHANDLER::smootherProgID, "texWidth"), dims[0]);
        glUniform1i(glGetUniformLocation(PROGHANDLER::smootherProgID, "texHeight"), dims[1]);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); //3 vertices * 2 triangles

        glDeleteTextures(1, &tempTex);
        tempTex = *quantizedTex;

    }
    *quantizedTex = tempTex;

    if (happyMistake) {

        tempTex = *quantizedTex;

        // The texture we're going to render to
        glGenTextures(1, quantizedTex);

        // "Bind" the newly created texture : all future texture functions will modify this texture
        glBindTexture(GL_TEXTURE_2D, *quantizedTex);


        // Give an empty image to OpenGL ( the last "0" )
        glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA, dims[0], dims[1], 0,GL_RGB, GL_UNSIGNED_BYTE, 0);

        // Poor filtering. Needed !
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, *quantizedTex, 0);
        // Set the list of draw buffers.
        GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
        glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers

        // Always check that our framebuffer is ok
        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            glDeleteTextures(1, quantizedTex);
            quantizedTex = 0;
            return;
        }

        // Render to our framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer);
        glViewport(0,0,dims[0],dims[1]); // Render on the whole framebuffer, complete from the lower left corner to the upper right


        glBindVertexArray(PROGHANDLER::VAO);

        glUseProgram(PROGHANDLER::happyMistakeProgID);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tempTex);

        glUniform1i(glGetUniformLocation(PROGHANDLER::happyMistakeProgID, "texToQuantize"), 0);
        glUniform1i(glGetUniformLocation(PROGHANDLER::happyMistakeProgID, "texWidth"), dims[0]);
        glUniform1i(glGetUniformLocation(PROGHANDLER::happyMistakeProgID, "texHeight"), dims[1]);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); //3 vertices * 2 triangles

        glDeleteTextures(1, &tempTex);

    }


    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &Framebuffer);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

}

void PROGHANDLER::genSegments(GLuint quantizedTex, GLuint *segmentTex) {
    
    if (PROGHANDLER::segmentProgID == 0) return;

    // setup to render
    

    int dims[2];
    GLWRAP::queryTex(quantizedTex, dims, GL_TEXTURE_2D);

    // The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
    GLuint Framebuffer = 0;
    glGenFramebuffers(1, &Framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer);

    // The texture we're going to render to
    glGenTextures(1, segmentTex);

    // "Bind" the newly created texture : all future texture functions will modify this texture
    glBindTexture(GL_TEXTURE_2D, *segmentTex);


    // Give an empty image to OpenGL ( the last "0" )
    glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA, dims[0], dims[1], 0,GL_RGBA, GL_UNSIGNED_BYTE, 0);

    // Poor filtering. Needed !
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    // Set "renderedTexture" as our colour attachement #0
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, *segmentTex, 0);

    // Set the list of draw buffers.
    GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers


    // Always check that our framebuffer is ok
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        *segmentTex = 0;
        return;
    }


    // Render to our framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer);
    glViewport(0,0,dims[0],dims[1]); // Render on the whole framebuffer, complete from the lower left corner to the upper right


    glBindVertexArray(PROGHANDLER::VAO);

    glUseProgram(PROGHANDLER::segmentProgID);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, quantizedTex);

    glUniform1i(glGetUniformLocation(PROGHANDLER::segmentProgID, "texToSegment"), 0);
    glUniform1i(glGetUniformLocation(PROGHANDLER::segmentProgID, "texWidth"), dims[0]);
    glUniform1i(glGetUniformLocation(PROGHANDLER::segmentProgID, "texHeight"), dims[1]);
    
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); //3 vertices * 2 triangles

    glActiveTexture(GL_TEXTURE0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &Framebuffer);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

}

void PROGHANDLER::overlayTextures(GLuint tex1, GLuint tex2, GLuint *overlayTex, float* tex1UV0, float* tex1UV1, float* tex2UV0, float* tex2UV1) {
    
    if (PROGHANDLER::overlayProgID == 0) return;

    // setup to render
    

    int dims[2];
    GLWRAP::queryTex(tex1, dims, GL_TEXTURE_2D);

    // The Viewportframebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
    GLuint Framebuffer = 0;
    glGenFramebuffers(1, &Framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer);

    // The texture we're going to render to
    glGenTextures(1, overlayTex);

    // "Bind" the newly created texture : all future texture functions will modify this texture
    glBindTexture(GL_TEXTURE_2D, *overlayTex);


    // Give an empty image to OpenGL ( the last "0" )
    glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA, dims[0], dims[1], 0,GL_RGB, GL_UNSIGNED_BYTE, 0);

    // Poor filtering. Needed !
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    // Set "renderedTexture" as our colour attachement #0
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, *overlayTex, 0);

    // Set the list of draw buffers.
    GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers


    // Always check that our framebuffer is ok
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        *overlayTex = 0;
        return;
    }


    // Render to our framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer);
    glViewport(0,0,dims[0],dims[1]); // Render on the whole framebuffer, complete from the lower left corner to the upper right


    glBindVertexArray(PROGHANDLER::VAO);

    glUseProgram(PROGHANDLER::overlayProgID);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex1);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, tex2);

    glUniform1i(glGetUniformLocation(PROGHANDLER::overlayProgID, "tex1"), 0);
    glUniform1i(glGetUniformLocation(PROGHANDLER::overlayProgID, "tex2"), 1);
    glUniform2f(glGetUniformLocation(PROGHANDLER::overlayProgID, "tex1UV0"), tex1UV0[0], tex1UV0[1]);
    glUniform2f(glGetUniformLocation(PROGHANDLER::overlayProgID, "tex1UV1"), tex1UV1[0], tex1UV1[1]);
    glUniform2f(glGetUniformLocation(PROGHANDLER::overlayProgID, "tex2UV0"), tex2UV0[0], tex2UV0[1]);
    glUniform2f(glGetUniformLocation(PROGHANDLER::overlayProgID, "tex2UV1"), tex2UV1[0], tex2UV1[1]);


    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); //3 vertices * 2 triangles

    glActiveTexture(GL_TEXTURE0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &Framebuffer);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


}

void PROGHANDLER::generateKey(GLuint fontTex, GLuint *keyTex, float *colorData, int numColors) {
    if (PROGHANDLER::keymakerProgID == 0) return;

    // setup to render
    
    // get font texture dimensions, this should be a 16x16 square containing the 256 characters, left to right, top to bottom
    int dims[2], outputDims[2];
    GLWRAP::queryTex(fontTex, dims, GL_TEXTURE_2D);

    // change dimensions so that they are the right size for the keyTex
    outputDims[0] = (dims[0]/16) * numColors;
    outputDims[1] = dims[0]/8;

    // The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
    GLuint Framebuffer = 0;
    glGenFramebuffers(1, &Framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer);

    // The texture we're going to render to
    glGenTextures(1, keyTex);

    // "Bind" the newly created texture : all future texture functions will modify this texture
    glBindTexture(GL_TEXTURE_2D, *keyTex);


    // Give an empty image to OpenGL ( the last "0" )
    glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA, outputDims[0], outputDims[1], 0,GL_RGBA, GL_UNSIGNED_BYTE, 0);

    // Poor filtering. Needed !
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    // Set "renderedTexture" as our colour attachement #0
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, *keyTex, 0);

    // Set the list of draw buffers.
    GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers


    // Always check that our framebuffer is ok
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        *keyTex = 0;
        return;
    }


    // Render to our framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer);
    glViewport(0,0,outputDims[0],outputDims[1]); // Render on the whole framebuffer, complete from the lower left corner to the upper right


    glBindVertexArray(PROGHANDLER::VAO);

    glUseProgram(PROGHANDLER::keymakerProgID);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, fontTex);

    glUniform1i(glGetUniformLocation(PROGHANDLER::keymakerProgID, "fontTex"), 0);
    glUniform4fv(glGetUniformLocation(PROGHANDLER::keymakerProgID, "colors"), numColors, colorData);
    glUniform1i(glGetUniformLocation(PROGHANDLER::keymakerProgID, "numColors"), numColors);
    
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); //3 vertices * 2 triangles

    glActiveTexture(GL_TEXTURE0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &Framebuffer);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void PROGHANDLER::setupPrograms() {
    if (PROGHANDLER::quantizerProgramID != 0) return; 

    static float vertices[] = {
        1.0f,  1.0f, 0.0f,  //top right
        1.0f, -1.0f, 0.0f,  //bottom right
        -1.0f, -1.0f, 0.0f, //bottom left
        -1.0f, 1.0f, 0.0f  //top left
    }; 
    static unsigned int indices[] = {  // note that we start from 0!
        0, 1, 3,   // first triangle
        1, 2, 3    // second triangle
    }; 
    

    glGenBuffers(1, &PROGHANDLER::VBO);  
    glGenVertexArrays(1, &PROGHANDLER::VAO);  
    glGenBuffers(1, &PROGHANDLER::EBO);

    // ..:: Initialization code (done once (unless your object frequently changes)) :: ..
    // 1. bind Vertex Array Object
    glBindVertexArray(PROGHANDLER::VAO);
    // 2. copy our vertices array in a buffer for OpenGL to use
    glBindBuffer(GL_ARRAY_BUFFER, PROGHANDLER::VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, PROGHANDLER::EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW); 
    // 3. then set our vertex attributes pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);  

    std::filesystem::path p = std::filesystem::current_path();
    p.append("resources/shaders");

    PROGHANDLER::quantizerProgramID =   GLWRAP::LoadShaders((p.append("quantizer.vert")).c_str(), 
                                                            p.parent_path().append("shaders/quantizer.frag").c_str());
    PROGHANDLER::segmentProgID =        GLWRAP::LoadShaders(p.parent_path().append("segmentGenerator.vert").c_str(), 
                                                            p.parent_path().append("segmentGenerator.frag").c_str());
    PROGHANDLER::overlayProgID =        GLWRAP::LoadShaders(p.parent_path().append("splice.vert").c_str(), 
                                                            p.parent_path().append("splice.frag").c_str());
    PROGHANDLER::happyMistakeProgID =   GLWRAP::LoadShaders(p.parent_path().append("happyMistake.vert").c_str(), 
                                                            p.parent_path().append("happyMistake.frag").c_str());
    PROGHANDLER::smootherProgID =       GLWRAP::LoadShaders(p.parent_path().append("smoother.vert").c_str(), 
                                                            p.parent_path().append("smoother.frag").c_str());
    PROGHANDLER::keymakerProgID =       GLWRAP::LoadShaders(p.parent_path().append("keymaker.vert").c_str(), 
                                                            p.parent_path().append("keymaker.frag").c_str());

}

void PROGHANDLER::closePrograms() {
    if (PROGHANDLER::quantizerProgramID == 0) return;
    glDeleteBuffers(1, &PROGHANDLER::VBO);
    glDeleteBuffers(1, &PROGHANDLER::EBO);
    glDeleteVertexArrays(1, &PROGHANDLER::VAO);  

    glDeleteProgram(PROGHANDLER::quantizerProgramID);
    glDeleteProgram(PROGHANDLER::segmentProgID);
    glDeleteProgram(PROGHANDLER::overlayProgID);
    glDeleteProgram(PROGHANDLER::happyMistakeProgID);
    glDeleteProgram(PROGHANDLER::smootherProgID);
    glDeleteProgram(PROGHANDLER::keymakerProgID);


    PROGHANDLER::EBO = 0;
    PROGHANDLER::VAO = 0;
    PROGHANDLER::VBO = 0; 
    PROGHANDLER::quantizerProgramID = 0;
    PROGHANDLER::segmentProgID = 0;
    PROGHANDLER::overlayProgID = 0;
    PROGHANDLER::happyMistakeProgID = 0;
    PROGHANDLER::smootherProgID = 0;
    PROGHANDLER::keymakerProgID = 0;

}

void PROGHANDLER::calcBestColors(GLuint tex, float* colorData, int numColors, bool random) {

    int texDims[2];
    GLWRAP::queryTex(tex, texDims, GL_TEXTURE_2D);
    unsigned char* texData = new unsigned char[texDims[0] * texDims[1] * 4];
    glBindTexture(GL_TEXTURE_2D, tex);
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, texData);
    glBindTexture(GL_TEXTURE_2D, 0);

    std::srand(std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count());

    for (int i = 0; i < numColors; i++) {
        if (random) {
            colorData[i*4 + 0] = float(std::rand()%255) / 255;
            colorData[i*4 + 1] = float(std::rand()%255) / 255;
            colorData[i*4 + 2] = float(std::rand()%255) / 255;
            colorData[i*4 + 3] = 1.0;
        } else {
            int x = std::rand()%texDims[0];
            int y = std::rand()%texDims[1];

            colorData[i*4 + 0] = float(texData[(x + y*texDims[0]) * 4 + 0]) / 255;
            colorData[i*4 + 1] = float(texData[(x + y*texDims[0]) * 4 + 1]) / 255;
            colorData[i*4 + 2] = float(texData[(x + y*texDims[0]) * 4 + 2]) / 255;
            colorData[i*4 + 3] = float(texData[(x + y*texDims[0]) * 4 + 3]) / 255;
        }
    }


    std::vector<float*> tempColors;
    for (int i = 0; i < numColors; i++) {
        tempColors.push_back(new float[4]);
        tempColors.at(i)[0] = 0.0;
        tempColors.at(i)[1] = 0.0;
        tempColors.at(i)[2] = 0.0;
        tempColors.at(i)[3] = 0.0;
    }
    
    std::vector<int> numValsInColor;
    numValsInColor.resize(tempColors.size());
    for (long unsigned int i = 0; i < numValsInColor.size(); i++)
        numValsInColor[i] = 0;

    for (int x = 0; x < texDims[0]; x++) {
        for (int y = 0; y < texDims[1]; y++) {
            int nearestColor = 0;
            float crntTexVal[4];

            crntTexVal[0] = float(texData[(x + y*texDims[0]) * 4 + 0]) / 255;
            crntTexVal[1] = float(texData[(x + y*texDims[0]) * 4 + 1]) / 255;
            crntTexVal[2] = float(texData[(x + y*texDims[0]) * 4 + 2]) / 255;
            crntTexVal[3] = float(texData[(x + y*texDims[0]) * 4 + 3]) / 255;

            for (long unsigned int i = 1; i < tempColors.size(); i++) {
                if (getDistance4(&colorData[i*4], crntTexVal) < getDistance4(&colorData[nearestColor*4], crntTexVal)) 
                    nearestColor = i;
            }

            tempColors[nearestColor][0] += float(crntTexVal[0]);
            tempColors[nearestColor][1] += float(crntTexVal[1]);
            tempColors[nearestColor][2] += float(crntTexVal[2]);
            tempColors[nearestColor][3] += float(crntTexVal[3]);
            numValsInColor[nearestColor]++;
        }
    }

    for (long unsigned int i = 0; i < tempColors.size(); i++) {
        if (numValsInColor[i] != 0) {
            tempColors[i][0] = tempColors[i][0] / numValsInColor[i];
            tempColors[i][1] = tempColors[i][1] / numValsInColor[i];
            tempColors[i][2] = tempColors[i][2] / numValsInColor[i];
            tempColors[i][3] = tempColors[i][3] / numValsInColor[i];
        }

        colorData[i*4 + 0] = tempColors[i][0];
        colorData[i*4 + 1] = tempColors[i][1];
        colorData[i*4 + 2] = tempColors[i][2];
        colorData[i*4 + 3] = tempColors[i][3];

        tempColors[i][0] = 0;
        tempColors[i][1] = 0;
        tempColors[i][2] = 0;
        tempColors[i][3] = 0;
    }

}

float PROGHANDLER::getDistance4(float u[4], float v[4]) {
    float dx = v[0]-u[0];
    float dy = v[1]-u[1];
    float dz = v[2]-u[2];
    float dw = v[3]-u[3];
    return std::sqrt(dx*dx + dy*dy + dz*dz + dw*dw);
}
