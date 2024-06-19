#ifndef GUI_H
#define GUI_H

#include "imgui/imgui.h"
#include "imfilebrowser/imfilebrowser.h"
#include "opengl.h"
#include "timer.h"

#include <vector>

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
    const int quantizationMenuWidth = 400;
    std::vector<float*> quantizationColors;

    GLuint crntTexID;
    GLuint crntQuantID;

    /**
     * variables for limitFPS
     */
    Timer timer;
    int FPSLimit;

    void imguiImageCentred(GLuint Tex, ImVec2 boundingBox);

public:
    GUI(GLFWwindow* window);

    void render(GLFWwindow* window);

    void cleanup();

    void framebufferSizeCallback(GLFWwindow* window, int width, int height);

    bool getQuantized();
    std::vector<float*> getQuantizationColors();

    bool setQuantizationColor(int index, float* color);
    void setCrntTexId(GLuint texID);

    /**
     * caps fps at value, 0 or less for no limit, no guaruntee for high fps
     */
    void limitFPS(int value);

};

#endif