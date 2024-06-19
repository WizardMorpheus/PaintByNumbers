#include "GUI.h"

#include "opengl.h"

#include "glWrap.h"
#include "quantizer.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stbImage/stb_image.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "imfilebrowser/imfilebrowser.h"

#include <format>
#include <vector>
#include <string>

const std::vector<std::string> allowedFileTypes = { ".jpeg", ".jpg", ".png" , ".tga", ".bmp", ".psd", ".gif", ".hdr", ".pic", ".pnm"};

void GUI::imguiImageCentred(GLuint Tex, ImVec2 boundingBox) {
    ImVec2 adjustedSize;

    int dims[2];
    GLWRAP::queryTex(Tex, dims, GL_TEXTURE_2D);
    ImVec2 dimsVec = ImVec2(dims[0], dims[1]);

    if (boundingBox.x/boundingBox.y > dimsVec.x/dimsVec.y) {
        adjustedSize.y = boundingBox.y;
        adjustedSize.x = boundingBox.y * (dimsVec.x/dimsVec.y);

        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (boundingBox.x - adjustedSize.x) * 0.5f);
    } else {
        adjustedSize.x = boundingBox.x;
        adjustedSize.y = boundingBox.x * (dimsVec.y/dimsVec.x);

        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + (boundingBox.y - adjustedSize.y) * 0.5f);
    }
    ImGui::Image((ImTextureID)Tex, adjustedSize);
}

GUI::GUI(GLFWwindow *window)
{
    this->fileMenuOpen = false;
    this->loading = false;
    this->saving = false;
    this->quantized = false;
    this->fileDialog = ImGui::FileBrowser();

    this->numColors = 16;
    this->mainMenuHeight = 0;
    this->quantizationColors = std::vector<float*>();
    for (int i = 0; i < numColors; i++) {
        quantizationColors.push_back(new float[4]);
    }

    this->crntTexID = 0;
    this->crntQuantID = 0;

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    this->io = ImGui::GetIO(); 
    (void)this->io;
    this->io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    this->io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    this->timer = Timer();
    this->FPSLimit = -1;


    int w,h;
    glfwGetFramebufferSize(window, &w, &h);
    this->io.DisplaySize = ImVec2(w, h);


    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    QUANTIZER::setupQuantizer();
}

void GUI::render(GLFWwindow* window) {

    if (this->FPSLimit >= 1) {
        if (this->timer.elapsedMilliseconds() < 1000/this->FPSLimit) return;
    }

    glClearColor(0.3, 0.4, 0.4, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // main menu bar
    if (ImGui::BeginMainMenuBar()) {
        ImGui::SetWindowFontScale(1);
        if (ImGui::BeginMenu("file", &this->fileMenuOpen)) {

            if (ImGui::MenuItem("Load")) {
                this->fileDialog.SetTitle("Load");
                this->fileDialog.SetTypeFilters(allowedFileTypes) ;
                this->fileDialog.Open();
                this->loading = true;
            }

            if (ImGui::MenuItem("Save")) {
                this->fileDialog.SetTitle("Save");
                this->fileDialog.SetTypeFilters(allowedFileTypes) ;
                this->fileDialog.Open();
                this->saving = true;
            }
            ImGui::EndMenu();
        }

        ImGui::Separator();

        ImGui::Checkbox("Quantize", &this->quantized);
        
        this->mainMenuHeight = ImGui::GetWindowHeight();

        ImGui::EndMainMenuBar();
    }

    if (this->quantized) {
        GLuint temp;
        QUANTIZER::quantize(this->crntTexID, &temp, this->numColors, &this->quantizationColors);
        if (temp != 0) {
            this->crntQuantID = temp;
        }
        
        ImGui::SetNextWindowPos(ImVec2(0, this->mainMenuHeight));
        ImGui::SetNextWindowSize(ImVec2(this->quantizationMenuWidth, this->io.DisplaySize.y - this->mainMenuHeight));
        if (ImGui::Begin("Quantization Menu", nullptr, ImGuiWindowFlags_NoCollapse |
                                                        ImGuiWindowFlags_NoResize)) {
            ImGui::Text(std::format("#Colors: {}", this->numColors).c_str());
            ImGui::Separator();
            
            for (int i = 0; i < this->quantizationColors.size(); i++) {
                ImGui::ColorEdit4(std::format("Color {}", i).c_str(), this->quantizationColors[i]);
            }

            ImGui::End();
        }
    }


    ImGui::SetNextWindowPos(ImVec2(this->quantized ? this->quantizationMenuWidth : 0, this->mainMenuHeight));
    ImGui::SetNextWindowSize(ImVec2(this->io.DisplaySize.x - (this->quantized ? this->quantizationMenuWidth : 0),
                                    this->io.DisplaySize.y - this->mainMenuHeight));
    if (ImGui::Begin("Image", nullptr, ImGuiWindowFlags_NoResize |
                                        ImGuiWindowFlags_NoScrollbar |
                                        ImGuiWindowFlags_NoScrollWithMouse |
                                        ImGuiWindowFlags_NoCollapse)) {
        //helloo
        if (this->quantized) {
            if (this->crntQuantID != 0)
                this->imguiImageCentred(this->crntQuantID, ImGui::GetWindowSize());
        } else {
            if (this->crntTexID != 0)
                this->imguiImageCentred(this->crntTexID, ImGui::GetWindowSize());
        }
        ImGui::End();
    }


    if (this->loading || this->saving) {
        this->fileDialog.Display();
        if (this->fileDialog.HasSelected()) {
            // load or save

            if (this->loading) {
                GLuint temp;
                GLWRAP::loadTex(fileDialog.GetSelected().generic_string().c_str(), &temp);
                if (temp != 0)  {
                    this->crntTexID = temp;
                    QUANTIZER::quantize(this->crntTexID, &temp, this->numColors, &this->quantizationColors);
                    if (temp != 0) {
                        this->crntQuantID = temp;
                    }
                }
            }
            if (this->saving) {

            }

            this->fileDialog.ClearSelected();
            this->fileDialog.Close();
            this->loading = false;
            this->saving = false;
        }
    }

    // Rendering
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    if (this->FPSLimit >= 1) this->timer.start();
}

void GUI::cleanup() {
    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    QUANTIZER::closeQuantizer();
}

void GUI::framebufferSizeCallback(GLFWwindow *window, int width, int height) {
    this->io.DisplaySize = ImVec2(width, height);
    this->render(window);
}

bool GUI::getQuantized() {
    return this->quantized;
}

std::vector<float *> GUI::getQuantizationColors() {
    return std::vector<float *>(this->quantizationColors);
}

bool GUI::setQuantizationColor(int index, float *color) {
    if (index < this->quantizationColors.size()) {
        this->quantizationColors.at(index) = color;
        return true;
    }
    return false;
}

void GUI::setCrntTexId(GLuint texID) {
    this->crntTexID = texID;
}

void GUI::limitFPS(int value) {
    this->FPSLimit = value;
    this->timer.start();
    if (value < 1) this->timer.stop();
}
