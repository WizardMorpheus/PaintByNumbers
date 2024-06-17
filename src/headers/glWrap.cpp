#include "glWrap.h"

#include "opengl.h"
#include "constants.h"
#include "stbImage/stb_image.h"

#include <iostream>

namespace GLWRAP
{
    
    /**
     * FURTHER ABSTRACTIONS
     */

    bool initGLFW(GLFWwindow** window) {
        // glfw: initialize and configure
        // ------------------------------
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); //uncomment this statement to fix compilation on OS X
#endif
        // glfw window creation
        // --------------------
        *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, SCR_NAME, NULL, NULL);
        if (*window == NULL)
        {
            std::cout << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
            return false;
        }
        glfwMakeContextCurrent(*window);
        glfwSwapInterval(1); // Enable vsync

        // glad: load all OpenGL function pointers
        // ---------------------------------------
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cout << "Failed to initialize GLAD" << std::endl;
            return false;
        }

        return true;
    }



    void loadTex(const char* filePath, GLuint* tex) {
        int x,y,n;
        unsigned char* data = stbi_load(filePath, &x, &y, &n, 4);
        
        if (data == NULL) {
            *tex = NULL;
            return;
        }

        glGenTextures(1, tex);

        // "Bind" the newly created texture : all future texture functions will modify this texture
        glBindTexture(GL_TEXTURE_2D, *tex);

        // Give the image to OpenGL
        glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

        glBindTexture(GL_TEXTURE_2D, NULL);

        stbi_image_free(data);
    }

    void queryTex(GLuint tex, int *dims, GLenum target) {
        glBindTexture(target, tex);
        glGetTexLevelParameteriv(target, 0, GL_TEXTURE_WIDTH, dims);
        if (target == GL_TEXTURE_2D || target == GL_TEXTURE_3D)
            glGetTexLevelParameteriv(target, 0, GL_TEXTURE_HEIGHT, dims + 1);
        if (target == GL_TEXTURE_3D)
            glGetTexLevelParameteriv(target, 0, GL_TEXTURE_DEPTH, dims + 2);
    }

} // namespace GLWRAP