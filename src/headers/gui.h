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
    bool showSegments;
    bool labelling;
    int smooth;
    int highlightedColor;
    int fontSize;
    bool happyMistake;
    ImGuiIO io;
    ImGui::FileBrowser loadDialog;
    ImGui::FileBrowser saveDialog;

    int numColors;
    int mainMenuHeight;
    const int quantizationMenuWidth = 400;
    float colorData[16*4];

    GLuint crntTexID;
    GLuint crntQuantID;
    GLuint crntSegmentID;
    GLuint crntOverlayID;
    GLuint fontTex;

    ImVec2 imagePosition;
    int imageScale;

    void imguiGetImageFillSizeAndOffset(GLuint Tex, ImVec2 boundingBox, ImVec2 *adjustedSize, ImVec2 *reqOffset);

    void requantize();

public:
    GUI(GLFWwindow* window);

    void render(GLFWwindow* window);

    void cleanup();

    void framebufferSizeCallback(GLFWwindow* window, int width, int height);

    bool getQuantized();
    float* getColorData();

    void setColor(int index, float* color);
    void setCrntTexId(GLuint texID);

    void handleInput();

};

#endif