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
    ImGuiIO io;
    ImGui::FileBrowser fileDialog;

public:
    GUI(GLFWwindow* window);

    void render(GLFWwindow* window);

    void cleanup();

};


#endif