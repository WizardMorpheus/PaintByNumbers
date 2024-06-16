#include "glad/glad.h"
#include "GLFW/glfw3.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stbImage/stb_image.h"

#include "headers/GUI.h"


#include <iostream>

void processInput(GLFWwindow *window);

bool initGLFW(GLFWwindow** window);

GLuint loadTex(const char* filePath);

// settings
const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;
const char* SCR_NAME = "Starter";

int main()
{
	GLFWwindow* win;
	if (!initGLFW(&win)) return -1;

	GUI gui = GUI(win);

	gui.setLoadCallback(&loadTex);

	glfwSetWindowUserPointer(win, &gui);

	// glfw: whenever the window size changed (by OS or user resize) this callback function executes
	// ---------------------------------------------------------------------------------------------
	glfwSetFramebufferSizeCallback(win, [](GLFWwindow* window, int width, int height) -> void {
		// make sure the viewport matches the new window dimensions; note that width and
		// height will be significantly larger than specified on retina displays.
		glViewport(0, 0, width, height);
		GUI* gui = (GUI*)glfwGetWindowUserPointer(window);
		gui->framebufferSizeCallback(window, width, height);
		glfwSwapBuffers(window);
	});

	// render loop
	// -----------
	while (!glfwWindowShouldClose(win))
	{
		// input
		// -----
		processInput(win);

		glClearColor(0.3, 0.4, 0.4, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);

		gui.render(win);		


		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(win);
		glfwPollEvents();
	}

	gui.cleanup();

	// glfw: terminate, clearing all previously allocated GLFWresources.
	//---------------------------------------------------------------
	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow *window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}


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

GLuint loadTex(const char *filePath)
{
    int x,y,n;
    unsigned char* data = stbi_load(filePath, &x, &y, &n, 4);
    
    if (data == NULL) return NULL;

	GLuint ret;

    glDeleteTextures(1, &ret);
    glGenTextures(1, &ret);

    // "Bind" the newly created texture : all future texture functions will modify this texture
    glBindTexture(GL_TEXTURE_2D, ret);

    // Give the image to OpenGL
    glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glBindTexture(GL_TEXTURE_2D, NULL);

    stbi_image_free(data);

    return ret;
}
