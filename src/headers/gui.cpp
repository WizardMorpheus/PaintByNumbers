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

const std::vector<std::string> loadFileTypes = { ".jpeg", ".jpg", ".png" , ".tga", ".bmp", ".psd", ".gif", ".hdr", ".pic", ".pnm"};
const std::vector<std::string> saveFileTypes = { ".jpg", ".png" , ".tga", ".bmp"};

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


GUI::GUI(GLFWwindow *window) {
    this->fileMenuOpen = false;
    this->loading = false;
    this->saving = false;
    this->quantized = false;
    this->showSegments = false;
    this->newMethod = false;
    this->loadDialog = ImGui::FileBrowser();
    this->saveDialog = ImGui::FileBrowser(ImGuiFileBrowserFlags_EnterNewFilename);

    this->numColors = 16;
    this->mainMenuHeight = 0;


    this->crntTexID = 0;
    this->crntQuantID = 0;
    this->crntSegmentID = 0;

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
                this->loadDialog.SetTitle("Load");
                this->loadDialog.SetTypeFilters(loadFileTypes) ;
                this->loadDialog.Open();
                this->loading = true;
            }

            if (ImGui::MenuItem("Save")) {
                this->saveDialog.SetTitle("Save");
                this->saveDialog.SetTypeFilters(saveFileTypes);
                this->saveDialog.Open();
                this->saving = true;
            }
            ImGui::EndMenu();
        }

        ImGui::Separator();

        if (this->crntTexID != 0) {
            ImGui::Checkbox("Quantize", &this->quantized);
            ImGui::Checkbox("Show PBN Segments", &this->showSegments);
            ImGui::Checkbox("new method", &this->newMethod);
        }
        
        this->mainMenuHeight = ImGui::GetWindowHeight();

        ImGui::EndMainMenuBar();
    }


    if (this->quantized) {
        GLuint temp;
        QUANTIZER::quantize(this->crntTexID, &temp, this->colorData, this->numColors, this->newMethod);
        if (temp != 0) {
            glDeleteTextures(1, &this->crntQuantID);
            this->crntQuantID = temp;
        }
    }
    if (this->showSegments) {
        GLuint temp;
        QUANTIZER::genSegments(this->crntQuantID, &temp);
        if (temp != 0) {
            glDeleteTextures(1, &this->crntSegmentID);
            this->crntSegmentID = temp;
        }
    }


    ImGui::SetNextWindowPos(ImVec2(0, this->mainMenuHeight));
    ImGui::SetNextWindowSize(ImVec2(this->quantizationMenuWidth, this->io.DisplaySize.y - this->mainMenuHeight));
    if (ImGui::Begin("Quantization Menu", nullptr, ImGuiWindowFlags_NoCollapse |
                                                    ImGuiWindowFlags_NoResize)) {
        ImGui::Text(std::format("#Colors: {}", this->numColors).c_str());

        ImGui::SliderInt("temp", &this->numColors, 1, 16);

        if (ImGui::Button("Randomize Color Pallette")) {
            QUANTIZER::calcBestColors(this->crntTexID, this->colorData, this->numColors, true);// arbitrary depth should be enough
        }
        if (ImGui::Button("'Guess' Best Colors")) {
            QUANTIZER::calcBestColors(this->crntTexID, this->colorData, this->numColors, false);// arbitrary depth should be enough
        }
        ImGui::Separator();
        
        for (int i = 0; i < this->numColors; i++) {
            if (ImGui::ColorEdit4(std::format("Color {}", i).c_str(), &this->colorData[i*4])) {
            }
        }

        ImGui::End();
    }


    ImGui::SetNextWindowPos(ImVec2(this->quantizationMenuWidth, this->mainMenuHeight));
    ImGui::SetNextWindowSize(ImVec2(this->io.DisplaySize.x - this->quantizationMenuWidth,
                                    this->io.DisplaySize.y - this->mainMenuHeight));
    if (ImGui::Begin("Image", nullptr, ImGuiWindowFlags_NoResize |
                                        ImGuiWindowFlags_NoScrollbar |
                                        ImGuiWindowFlags_NoScrollWithMouse |
                                        ImGuiWindowFlags_NoCollapse)) {

        ImVec2 cursorPos = ImGui::GetCursorPos();

        if (this->quantized) {
            if (this->crntQuantID != 0) {
                if (this->crntOverlayID != this->crntQuantID && this->crntOverlayID != this->crntTexID)
                    glDeleteTextures(1, &this->crntOverlayID);
                this->crntOverlayID = this->crntQuantID;
                if (this->showSegments) {
                    QUANTIZER::overlayTextures(this->crntQuantID, this->crntSegmentID, &this->crntOverlayID);
                    if (this->crntOverlayID == 0) this->crntOverlayID = this->crntQuantID;
                }

                this->imguiImageCentred(this->crntOverlayID, ImGui::GetWindowSize());
            }
        } else {
            if (this->crntTexID != 0) {
                if (this->crntOverlayID != this->crntQuantID && this->crntOverlayID != this->crntTexID)
                    glDeleteTextures(1, &this->crntOverlayID);
                this->crntOverlayID = this->crntTexID;
                if (this->showSegments) {
                    QUANTIZER::overlayTextures(this->crntTexID, this->crntSegmentID, &this->crntOverlayID);
                    if (this->crntOverlayID == 0) this->crntOverlayID = this->crntTexID;
                }

                this->imguiImageCentred(this->crntOverlayID, ImGui::GetWindowSize());
            }
        }

        ImGui::End();
    }


    if (this->loading) {
        this->loadDialog.Display();
        if (this->loadDialog.HasSelected()) {
            // load
            GLuint temp;
            GLWRAP::loadTex(this->loadDialog.GetSelected().generic_string().c_str(), &temp);
            if (temp != 0)  {
                this->crntTexID = temp;
                QUANTIZER::quantize(this->crntTexID, &temp, this->colorData, this->numColors, this->newMethod);
                if (temp != 0) {
                    this->crntQuantID = temp;
                }
            }

            this->loadDialog.ClearSelected();
            this->loadDialog.Close();
            this->loading = false;
        }
    }

    if (this->saving) {
        this->saveDialog.Display();
        if (this->saveDialog.HasSelected()) {
            // save
            GLWRAP::saveTex(this->saveDialog.GetSelected().generic_string().c_str(),
                            this->saveDialog.GetSelected().extension().generic_string().c_str(),
                             &this->crntSegmentID);

            this->saveDialog.ClearSelected();
            this->saveDialog.Close();
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

float* GUI::getColorData() {
    return this->colorData;
}

void GUI::setColor(int index, float *color) {
    this->colorData[index*4 + 0] = color[0];
    this->colorData[index*4 + 1] = color[1];
    this->colorData[index*4 + 2] = color[2];
    this->colorData[index*4 + 3] = color[3];
}

void GUI::setCrntTexId(GLuint texID) {
    this->crntTexID = texID;
}

void GUI::limitFPS(int value) {
    this->FPSLimit = value;
    this->timer.start();
    if (value < 1) this->timer.stop();
}
