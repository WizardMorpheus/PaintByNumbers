
#include "headers/opengl.h"
//#include "headers/glWrap.h"
#include "headers/GUI.h"
#include "headers/glWrap.h"

#include <iostream>

void processInput(GLFWwindow *window);

bool initGLFW(GLFWwindow** window);

// settings
#include "headers/constants.h"

int main()
{
	GLFWwindow* win;
	if (!GLWRAP::initGLFW(&win)) return -1;

	GUI gui = GUI(win);


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


	//gui.limitFPS(60);
	// render loop
	// -----------
	while (!glfwWindowShouldClose(win))
	{
		// input
		// -----
		processInput(win);

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