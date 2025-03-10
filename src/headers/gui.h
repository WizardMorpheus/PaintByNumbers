#ifndef GUI_H
#define GUI_H

#include "imgui/imgui.h"
#include "imfilebrowser/imfilebrowser.h"
#include "opengl.h"
#include "timer.h"

#include <vector>
#include <thread>

class GUI {
private:
    GLFWwindow* window;

    bool fileMenuOpen;
    bool loading;
    bool loadingVideo;
    bool saving;
    bool manualOpen;
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
    GLuint demoTexID;
    GLuint crntQuantID;
    GLuint crntSegmentID;
    GLuint crntOverlayID;
    GLuint fontTex;

    ImVec2 imagePosition;
    float imageScale;
    float imageScaleExponent;
    float imageScaleImcrement;

    float lastMouseScroll;

    std::thread videoThread;
    bool stopVideoThread;
    bool videoThreadRunning;
    std::mutex videoPathMutex;
    std::filesystem::path videoPath;
    bool updateCrntTex;
    GLuint videoTexID;
    std::mutex crntTexMutex;

    bool allowCrntTexManipulation;

    void getImageSizeAndOffsetInBoundingBox(GLuint Tex, ImVec2 boundingBox, ImVec2 *adjustedSize, ImVec2 *reqOffset);
    void getImageSizeAndOffsetInWindow(GLuint Tex, ImVec2 windowSize, ImVec2* adjustedSize, ImVec2* offset);

    void requantize();
    void regenSegs();

public:
    GUI(GLFWwindow* window);

    void render(GLFWwindow* window);

    void cleanup();

    void framebufferSizeCallback(GLFWwindow* window, int width, int height);

    bool getQuantized();
    float* getColorData();

    void setColor(int index, float* color);
    void setCrntTexId(GLuint texID);
    GLuint& getDemoTexId();

    void handleInput();

};

#endif