#include "quantizer.h"

#include "opengl.h"
#include "glWrap.h"
#include <iostream>
#include <cmath>
#include <random>
#include <chrono>

void QUANTIZER::quantize(GLuint tex, GLuint *quantizedTex, std::vector<float *> *colors) {
    
    if (QUANTIZER::qProgramID == 0) return;

    // turn our colors into a texture

    unsigned char colData[(colors->size() + colors->size()%2) * 3];


    for (int i = 0; i < colors->size(); i++){
        colData[i*3] = (char)(colors->at(i)[0] * 255);
        colData[i*3 + 1] = (char)(colors->at(i)[1] * 255);
        colData[i*3 + 2] = (char)(colors->at(i)[2] * 255);
    }
    
    GLuint colorTex;
    glGenTextures(1, &colorTex);
    glBindTexture(GL_TEXTURE_2D, colorTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, (colors->size() + colors->size()%2)/2, 2, 0, GL_RGB, GL_UNSIGNED_BYTE, colData);
    
    // Poor filtering. Needed !
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


    // setup to render
    

    int dims[2];
    GLWRAP::queryTex(tex, dims, GL_TEXTURE_2D);

    // The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
    GLuint Framebuffer = 0;
    glGenFramebuffers(1, &Framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer);

    // The texture we're going to render to
    glGenTextures(1, quantizedTex);

    // "Bind" the newly created texture : all future texture functions will modify this texture
    glBindTexture(GL_TEXTURE_2D, *quantizedTex);


    // Give an empty image to OpenGL ( the last "0" )
    glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, dims[0], dims[1], 0,GL_RGB, GL_UNSIGNED_BYTE, 0);

    // Poor filtering. Needed !
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    // Set "renderedTexture" as our colour attachement #0
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, *quantizedTex, 0);

    // Set the list of draw buffers.
    GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers


    // Always check that our framebuffer is ok
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        *quantizedTex = 0;
        return;
    }


    // Render to our framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer);
    glViewport(0,0,dims[0],dims[1]); // Render on the whole framebuffer, complete from the lower left corner to the upper right


    glBindVertexArray(QUANTIZER::qVAO);

    glUseProgram(QUANTIZER::qProgramID);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, colorTex);

    glUniform1i(glGetUniformLocation(QUANTIZER::qProgramID, "texToQuantize"), 0);
    glUniform1i(glGetUniformLocation(QUANTIZER::qProgramID, "colors"), 1);
    glUniform1i(glGetUniformLocation(QUANTIZER::qProgramID, "numColors"), colors->size());
    
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); //3 vertices * 2 triangles

    glActiveTexture(GL_TEXTURE0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &Framebuffer);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glDeleteTextures(1, &colorTex);

}

void QUANTIZER::setupQuantizer() {
    if (QUANTIZER::qProgramID != 0) return; 

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
    

    glGenBuffers(1, &QUANTIZER::qVBO);  
    glGenVertexArrays(1, &QUANTIZER::qVAO);  
    glGenBuffers(1, &QUANTIZER::qEBO);

    // ..:: Initialization code (done once (unless your object frequently changes)) :: ..
    // 1. bind Vertex Array Object
    glBindVertexArray(QUANTIZER::qVAO);
    // 2. copy our vertices array in a buffer for OpenGL to use
    glBindBuffer(GL_ARRAY_BUFFER, QUANTIZER::qVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, QUANTIZER::qEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW); 
    // 3. then set our vertex attributes pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);  

    QUANTIZER::qProgramID = GLWRAP::LoadShaders("resources\\shaders\\quantizer.vert", "resources\\shaders\\quantizer.frag");
    QUANTIZER::qColorProgID = GLWRAP::LoadShaders("resources\\shaders\\kmean.vert", "resources\\shaders\\kmean.frag");
}

void QUANTIZER::closeQuantizer() {
    if (QUANTIZER::qProgramID == 0) return;
    glDeleteBuffers(1, &QUANTIZER::qVBO);
    glDeleteBuffers(1, &QUANTIZER::qEBO);
    glDeleteVertexArrays(1, &QUANTIZER::qVAO);  

    glDeleteProgram(QUANTIZER::qProgramID);
    glDeleteProgram(QUANTIZER::qColorProgID);

    QUANTIZER::qEBO = 0;
    QUANTIZER::qVAO = 0;
    QUANTIZER::qVBO = 0; 
    QUANTIZER::qProgramID = 0;
    QUANTIZER::qColorProgID = 0;
}

void QUANTIZER::calcBestColors(GLuint tex, std::vector<float *> *colors, bool random) {

    int texDims[2];
    GLWRAP::queryTex(tex, texDims, GL_TEXTURE_2D);
    unsigned char* texData = new unsigned char[texDims[0] * texDims[1] * 4];
    glBindTexture(GL_TEXTURE_2D, tex);
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, texData);
    glBindTexture(GL_TEXTURE_2D, 0);

    std::srand(std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count());

    for (int i = 0; i < colors->size(); i++) {
        if (random) {
            colors->at(i)[0] = float(std::rand()%255) / 255;
            colors->at(i)[1] = float(std::rand()%255) / 255;
            colors->at(i)[2] = float(std::rand()%255) / 255;
            colors->at(i)[3] = 1.0;
        } else {
            int x = std::rand()%texDims[0];
            int y = std::rand()%texDims[1];

            colors->at(0)[0] = float(texData[(x + y*texDims[0]) * 4 + 0]) / 255;
            colors->at(1)[1] = float(texData[(x + y*texDims[0]) * 4 + 1]) / 255;
            colors->at(2)[2] = float(texData[(x + y*texDims[0]) * 4 + 2]) / 255;
            colors->at(3)[3] = float(texData[(x + y*texDims[0]) * 4 + 3]) / 255;
        }
    }


    std::vector<float*> tempColors;
    for (int i = 0; i < colors->size(); i++) {
        tempColors.push_back(new float[4]);
        tempColors.at(i)[0] = 0.0;
        tempColors.at(i)[1] = 0.0;
        tempColors.at(i)[2] = 0.0;
        tempColors.at(i)[3] = 0.0;
    }
    
    std::vector<int> numValsInColor;
    numValsInColor.resize(tempColors.size());
    for (int i = 0; i < numValsInColor.size(); i++)
        numValsInColor[i] = 0;

    for (int x = 0; x < texDims[0]; x++) {
        for (int y = 0; y < texDims[1]; y++) {
            int nearestColor = 0;
            float crntTexVal[4];

            crntTexVal[0] = float(texData[(x + y*texDims[0]) * 4 + 0]) / 255;
            crntTexVal[1] = float(texData[(x + y*texDims[0]) * 4 + 1]) / 255;
            crntTexVal[2] = float(texData[(x + y*texDims[0]) * 4 + 2]) / 255;
            crntTexVal[3] = float(texData[(x + y*texDims[0]) * 4 + 3]) / 255;

            for (int i = 1; i < tempColors.size(); i++) {
                if (getDistance4(colors->at(i), crntTexVal) < getDistance4(colors->at(nearestColor), crntTexVal)) 
                    nearestColor = i;
            }

            tempColors[nearestColor][0] += float(crntTexVal[0]);
            tempColors[nearestColor][1] += float(crntTexVal[1]);
            tempColors[nearestColor][2] += float(crntTexVal[2]);
            tempColors[nearestColor][3] += float(crntTexVal[3]);
            numValsInColor[nearestColor]++;
        }
    }

    for (int i = 0; i < tempColors.size(); i++) {
        if (numValsInColor[i] != 0) {
            tempColors[i][0] = tempColors[i][0] / numValsInColor[i];
            tempColors[i][1] = tempColors[i][1] / numValsInColor[i];
            tempColors[i][2] = tempColors[i][2] / numValsInColor[i];
            tempColors[i][3] = tempColors[i][3] / numValsInColor[i];
        }

        colors->at(i)[0] = tempColors[i][0];
        colors->at(i)[1] = tempColors[i][1];
        colors->at(i)[2] = tempColors[i][2];
        colors->at(i)[3] = tempColors[i][3];

        tempColors[i][0] = 0;
        tempColors[i][1] = 0;
        tempColors[i][2] = 0;
        tempColors[i][3] = 0;
    }

    


    // if (QUANTIZER::qColorProgID == 0) return;

    // // turn our colors into a texture
    // unsigned char colData[(colors->size() + colors->size()%2) * 3];

    // // if (colors->size()%2 != 0) {
    // //     colData = new unsigned char[(colors->size() + 1) * 3];
    // // } else {
    // //     colData = new unsigned char[colors->size() * 3];
    // // }

    // for (int i = 0; i < colors->size(); i++){
    //     colData[i*3] = (char)(colors->at(i)[0] * 255);
    //     colData[i*3 + 1] = (char)(colors->at(i)[1] * 255);
    //     colData[i*3 + 2] = (char)(colors->at(i)[2] * 255);
    // }

    
    // GLuint colorTex;
    // glGenTextures(1, &colorTex);
    // glBindTexture(GL_TEXTURE_2D, colorTex);
    // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, (colors->size() + colors->size()%2)/2, 2, 0, GL_RGB, GL_UNSIGNED_BYTE, colData);
    
    // // Poor filtering. Needed !
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    // // The texture we're going to render to
    // GLuint newColorTex;
    // glGenTextures(1, &newColorTex);

    // // "Bind" the newly created texture : all future texture functions will modify this texture
    // glBindTexture(GL_TEXTURE_2D, newColorTex);

    // // Give an empty image to OpenGL ( the last "0" )
    // glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, (colors->size() + colors->size()%2)/2, 2, 0,GL_RGB, GL_UNSIGNED_BYTE, 0);

    // // Poor filtering. Needed !
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


    // // The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
    // GLuint Framebuffer = 0;
    // glGenFramebuffers(1, &Framebuffer);
    // glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer);

    // // Set "renderedTexture" as our colour attachement #0
    // glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, newColorTex, 0);

    // // Set the list of draw buffers.
    // GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
    // glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers

    // // Always check that our framebuffer is ok
    // if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    //     newColorTex = 0;
    //     return;
    // }


    // // Render to our framebuffer
    // glBindFramebuffer(GL_FRAMEBUFFER, Framebuffer);
    // glViewport(0,0, (colors->size() + colors->size()%2) / 2, 2); // Render on the whole framebuffer, complete from the lower left corner to the upper right


    // glBindVertexArray(QUANTIZER::qVAO);

    // glUseProgram(QUANTIZER::qColorProgID);

    // glActiveTexture(GL_TEXTURE0);
    // glBindTexture(GL_TEXTURE_2D, tex);

    // glActiveTexture(GL_TEXTURE1);
    // glBindTexture(GL_TEXTURE_2D, colorTex);

    // glUniform1i(glGetUniformLocation(QUANTIZER::qColorProgID, "texToQuantize"), 0);
    // glUniform1i(glGetUniformLocation(QUANTIZER::qColorProgID, "colors"), 1);
    // glUniform1i(glGetUniformLocation(QUANTIZER::qColorProgID, "numColors"), colors->size());
    
    // int texDims[2];
    // GLWRAP::queryTex(tex, texDims, GL_TEXTURE_2D);
    // glUniform1i(glGetUniformLocation(QUANTIZER::qColorProgID, "texWidth"), texDims[0]);
    // glUniform1i(glGetUniformLocation(QUANTIZER::qColorProgID, "texHeight"), texDims[1]);

    // for (int i = 0; i < depth; i++) {
    //     glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); //3 vertices * 2 triangles

    //     // copy new colors to old color texture 
    //     glBindTexture(GL_TEXTURE_2D, newColorTex);
    //     glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, colData);

    //     GLenum err = glGetError();
    //     if (err != GL_NO_ERROR) {
    //         GLint binding;
    //         glGetIntegerv(GL_PIXEL_PACK_BUFFER_BINDING, &binding);
    //         std::cout << err << ": binding" << std::endl;
    //     }

    //     glBindTexture(GL_TEXTURE_2D, colorTex);
    //     glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, (colors->size() + colors->size()%2)/2, 2, 0, GL_RGB, GL_UNSIGNED_BYTE, colData);
        
    //     err = glGetError();
    //     if (err != GL_NO_ERROR) {
    //         std::cout << err << std::endl;
    //     }

        
    // }


    // glBindTexture(GL_TEXTURE_2D, 0);
    // glActiveTexture(GL_TEXTURE0);
    // glBindTexture(GL_TEXTURE_2D, 0);

    // glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // glDeleteFramebuffers(1, &Framebuffer);

    // glBindVertexArray(0);
    // glBindBuffer(GL_ARRAY_BUFFER, 0);
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // glDeleteTextures(1, &colorTex);
    // glDeleteTextures(1, &newColorTex);

    // for (int i = 0; i < colors->size(); i++) {
    //     colors->at(i)[0] = float(colData[i*3 + 0]) / 255;
    //     colors->at(i)[1] = float(colData[i*3 + 1]) / 255;
    //     colors->at(i)[2] = float(colData[i*3 + 2]) / 255;
    //     colors->at(i)[3] = 1.0f;
    // }


}

float QUANTIZER::getDistance4(float u[4], float v[4]) {
    float dx = v[0]-u[0];
    float dy = v[1]-u[1];
    float dz = v[2]-u[2];
    float dw = v[3]-u[3];
    return std::sqrt(dx*dx + dy*dy + dz*dz + dw*dw);
}
