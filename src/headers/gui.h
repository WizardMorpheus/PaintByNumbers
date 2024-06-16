#ifndef GUI_H
#define GUI_H

#include "imgui/imgui.h"
#include "imfilebrowser/imfilebrowser.h"
#include "GLFW/glfw3.h"

class GUI {
private:
	// Our state
    bool fileMenuOpen;
    bool loading;
    bool saving;
    bool quantized;
    ImGuiIO io;
    ImGui::FileBrowser fileDialog;

    int numColors;
    int mainMenuHeight;

public:
    GUI(GLFWwindow* window);

    void render(GLFWwindow* window);

    void cleanup();

    bool getQuantized();

};


#endif