#ifndef GLWRAP_H
#define GLWRAP_H

#include "opengl.h"

namespace GLWRAP
{
    
    /**
     * FURTHER ABSTRACTIONS
     */

    /**
     * initialises GLFW with glad, creates a GLFWwindow* on *window and makes it's context current.
     */
    bool initGLFW(GLFWwindow** window);
    
    /**
     * loads a texture from filePath into memory and sets *Tex as a pointer to it
     * if the texture cannot be loaded for any reason, Tex returns as NULL
     */
    void loadTex(const char* filePath, GLuint* Tex);

    /**
     * retrieves the width and height of a texture Tex and stores them in *dims, Tex is of type target
     */
    void queryTex(GLuint tex, int* dims, GLenum target);

} // namespace GLWRAP


#endif