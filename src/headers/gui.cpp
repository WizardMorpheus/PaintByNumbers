#include "GUI.h"

#include "GLFW/glfw3.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "imfilebrowser/imfilebrowser.h"

#include <format>
#include <vector>
#include <string>

const std::vector<std::string> allowedFileTypes = { ".jpeg", ".jpg", ".png" , ".tga", ".bmp", ".psd", ".gif", ".hdr", ".pic", ".pnm"};

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

    this->crntTexID = NULL;
    this->loadCallback = NULL;

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    this->io = ImGui::GetIO(); 
    (void)this->io;
    this->io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    this->io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls


    int w,h;
    glfwGetFramebufferSize(window, &w, &h);
    this->io.DisplaySize = ImVec2(w, h);


    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

}

void GUI::render(GLFWwindow* window) {

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
            if (this->crntTexID != NULL)
                ImGui::Image((ImTextureID)this->crntTexID, ImGui::GetWindowSize());

            ImGui::End();
        }


        if (this->loading || this->saving) {
            this->fileDialog.Display();
            if (this->fileDialog.HasSelected()) {
                // load or save

                if (this->loading && this->loadCallback != NULL) {
                    GLuint temp = this->loadCallback(fileDialog.GetSelected().generic_string().c_str());
                    if (temp != NULL) this->crntTexID = temp;
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
}

void GUI::cleanup() {
    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
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

void GUI::setLoadCallback(GLuint(* func)(const char* filePath)) {
    this->loadCallback = func;
}
