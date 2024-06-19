#include "quantizer.h"

#include "opengl.h"
#include "glWrap.h"

void QUANTIZER::quantize(GLuint tex, GLuint *quantizedTex, int numColors, std::vector<float *> *colors) {
    
    if (QUANTIZER::qProgramID == NULL) return;

    // turn our colors into a texture
    unsigned char colData[colors->size() * 3];
    for (int i = 0; i < colors->size(); i++){
        colData[i*3] = (char)(colors->at(i)[0] * 255);
        colData[i*3 + 1] = (char)(colors->at(i)[1] * 255);
        colData[i*3 + 2] = (char)(colors->at(i)[2] * 255);
    }
    
    GLuint colorTex;
    glGenTextures(1, &colorTex);
    glBindTexture(GL_TEXTURE_2D, colorTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, colors->size(), 1, 0, GL_RGB, GL_UNSIGNED_BYTE, colData);
    
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
        *quantizedTex = NULL;
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


    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &Framebuffer);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glDeleteTextures(1, &colorTex);

}

void QUANTIZER::setupQuantizer() {
    if (QUANTIZER::qProgramID != NULL) return; 

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

}

void QUANTIZER::closeQuantizer() {
    if (QUANTIZER::qProgramID == NULL) return;
    glDeleteBuffers(1, &QUANTIZER::qVBO);
    glDeleteBuffers(1, &QUANTIZER::qEBO);
    glDeleteVertexArrays(1, &QUANTIZER::qVAO);  

    glDeleteProgram(QUANTIZER::qProgramID);

    QUANTIZER::qEBO = NULL;
    QUANTIZER::qVAO = NULL;
    QUANTIZER::qVBO = NULL;
    QUANTIZER::qProgramID = NULL;
}
