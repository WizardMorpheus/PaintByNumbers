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
     * save a texture to the given filepath as a png image
     */
    void saveTex(const char* filePath, const char* fileType, GLuint* tex);

    /**
     * retrieves the width and height of a texture Tex and stores them in *dims, Tex is of type target
     */
    void queryTex(GLuint tex, int* dims, GLenum target);

    /**
     * loads a shader program from a vertex and fragment shader file path
     */
    GLuint LoadShaders(const char* vertex_file_path, const char* fragment_file_path);



} // namespace GLWRAP


#endif