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

void GUI::imguiGetImageFillSizeAndOffset(GLuint Tex, ImVec2 boundingBox, ImVec2* adjustedSize, ImVec2* reqOffset) {
    int dims[2];
    GLWRAP::queryTex(Tex, dims, GL_TEXTURE_2D);
    ImVec2 dimsVec = ImVec2(dims[0], dims[1]);

    boundingBox = boundingBox * std::pow(1.05, this->imageScale);

    if (boundingBox.x/boundingBox.y > dimsVec.x/dimsVec.y) {
        adjustedSize->y = boundingBox.y;
        adjustedSize->x = boundingBox.y * (dimsVec.x/dimsVec.y);

        reqOffset->x = (boundingBox.x - adjustedSize->x) * 0.5f;
        reqOffset->y = 0;
    } else {
        adjustedSize->x = boundingBox.x;
        adjustedSize->y = boundingBox.x * (dimsVec.y/dimsVec.x);

        reqOffset->x = 0;
        reqOffset->y = (boundingBox.y - adjustedSize->y) * 0.5f;
    }

    *reqOffset = *reqOffset + this->imagePosition;

}

void GUI::requantize(){
    GLuint temp;
    QUANTIZER::quantize(this->crntTexID, &temp, this->colorData, this->numColors, this->highlightedColor, this->smooth, this->happyMistake);
    if (temp != 0) {
        glDeleteTextures(1, &this->crntQuantID);
        this->crntQuantID = temp;
    }
}

GUI::GUI(GLFWwindow *window) {
    this->fileMenuOpen = false;
    this->loading = false;
    this->saving = false;
    this->quantized = false;
    this->showSegments = false;
    this->smooth = false;
    this->labelling = false;
    this->highlightedColor = -1;

    this->loadDialog = ImGui::FileBrowser();
    this->loadDialog.SetTitle("Load");
    this->loadDialog.SetTypeFilters(loadFileTypes);

    this->saveDialog = ImGui::FileBrowser(ImGuiFileBrowserFlags_EnterNewFilename);
    this->saveDialog.SetTitle("Save");
    this->saveDialog.SetTypeFilters(saveFileTypes);

    this->numColors = 16;
    this->mainMenuHeight = 0;
    this->fontSize = 0;

    this->crntTexID = 0;
    this->crntQuantID = 0;
    this->crntSegmentID = 0;
    this->crntOverlayID = 0;
    this->fontTex = 0;
    GLWRAP::loadTex("bin/debug/fontTex.png", &this->fontTex);

    this->imagePosition = ImVec2(0,0);
    this->imageScale = 0;

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

    QUANTIZER::setupQuantizer();
}

void GUI::render(GLFWwindow* window) {

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
                this->loadDialog.Open();
                this->loading = true;
            }

            ImGui::EndMenu();
        }

        ImGui::Separator();

        if (this->crntTexID != 0) {
            ImGui::Checkbox("Quantize", &this->quantized);
            ImGui::Checkbox("Show PBN Segments", &this->showSegments);
            if (ImGui::InputInt("Smooth", &this->smooth)) requantize();
            if (ImGui::Checkbox("Happy Mistake", &this->happyMistake)) requantize();

        }
        
        this->mainMenuHeight = ImGui::GetWindowHeight();

        ImGui::EndMainMenuBar();
    }


    if (this->showSegments) {
        GLuint temp;
        if (!this->labelling && !this->saving) {
            QUANTIZER::genSegments(this->crntQuantID, &temp);
            if (temp != 0) {
                glDeleteTextures(1, &this->crntSegmentID);
                this->crntSegmentID = temp;
            }
        }
    }


    ImGui::SetNextWindowPos(ImVec2(0, this->mainMenuHeight));
    ImGui::SetNextWindowSize(ImVec2(this->quantizationMenuWidth, this->io.DisplaySize.y - this->mainMenuHeight));
    if (ImGui::Begin("Quantization Menu", nullptr, ImGuiWindowFlags_NoCollapse |
                                                    ImGuiWindowFlags_NoResize)) {
        ImGui::Text(std::format("#Colors: {}", this->numColors).c_str());

        if (ImGui::SliderInt("temp", &this->numColors, 1, 16)) {
            requantize();
        }

        if (ImGui::Button("Randomize Color Pallette")) {
            QUANTIZER::calcBestColors(this->crntTexID, this->colorData, this->numColors, true);// arbitrary depth should be enough
            requantize();
        }
        if (ImGui::Button("'Guess' Best Colors")) {
            QUANTIZER::calcBestColors(this->crntTexID, this->colorData, this->numColors, false);// arbitrary depth should be enough
            requantize();
        }
        ImGui::Separator();
        
        for (int i = 0; i < this->numColors; i++) {
            if (ImGui::ColorEdit4(std::format("Color {}", i).c_str(), &this->colorData[i*4])) {
                requantize();
            }
        }
    
        if (ImGui::Button(this->labelling ? "Cancel Labelling" : "Label Sections")) {
            this->labelling = !this->labelling;
            if (this->labelling) this->highlightedColor = 0;
            else this->highlightedColor = -1;
            requantize();
        }
        if (this->labelling) {
            if (ImGui::Button("Next Color")) {
                this->highlightedColor++;
                requantize();
            }
            ImGui::SameLine();
            if (ImGui::Button("Last Color")) {
                this->highlightedColor--;
                requantize();
            }

        }

        ImGui::Separator();

       

        ImGui::Text("Image info:");
        if (this->crntTexID != 0) {
            int dims[2];
            GLWRAP::queryTex(this->crntTexID, dims, GL_TEXTURE_2D);
            ImGui::Text(std::format("Width: {}", dims[0]).c_str());
            ImGui::Text(std::format("Height: {}", dims[1]).c_str());
        } else {
            ImGui::Text("No image loaded");
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

        ImGui::SetCursorPos(ImVec2(0, 0));


        if (this->labelling) {
            if (this->crntQuantID != 0) {
                if (this->crntOverlayID != this->crntQuantID && this->crntOverlayID != this->crntTexID)
                    glDeleteTextures(1, &this->crntOverlayID);
                float UV0[2] =  {0.0, 0.0};
                float UV1[2] =  {1.0, 1.0};

                QUANTIZER::overlayTextures(this->crntQuantID, this->crntSegmentID, &this->crntOverlayID,
                                            UV0, UV1, UV0, UV1);
                if (this->crntOverlayID == 0) this->crntOverlayID = this->crntQuantID;

                ImVec2 adjustedSize, offset;
                this->imguiGetImageFillSizeAndOffset(this->crntOverlayID, ImGui::GetWindowSize(), &adjustedSize, &offset);
                ImGui::SetCursorPos(ImGui::GetCursorPos() + offset);
                ImGui::Image((ImTextureID)this->crntOverlayID, adjustedSize);
            }
        } else if (this->quantized) {
            if (this->crntQuantID != 0) {
                if (this->crntOverlayID != this->crntQuantID && this->crntOverlayID != this->crntTexID)
                    glDeleteTextures(1, &this->crntOverlayID);
                this->crntOverlayID = this->crntQuantID;
                if (this->showSegments || this->labelling) {
                    float UV0[2] =  {0.0, 0.0};
                    float UV1[2] =  {1.0, 1.0};

                    QUANTIZER::overlayTextures(this->crntQuantID, this->crntSegmentID, &this->crntOverlayID,
                                                UV0, UV1, UV0, UV1);
                    if (this->crntOverlayID == 0) this->crntOverlayID = this->crntQuantID;
                }

                ImVec2 adjustedSize, offset;
                this->imguiGetImageFillSizeAndOffset(this->crntOverlayID, ImGui::GetWindowSize(), &adjustedSize, &offset);
                ImGui::SetCursorPos(ImGui::GetCursorPos() + offset);
                ImGui::Image((ImTextureID)this->crntOverlayID, adjustedSize);
            }
        } else {
            if (this->crntTexID != 0) {
                if (this->crntOverlayID != this->crntQuantID && this->crntOverlayID != this->crntTexID)
                    glDeleteTextures(1, &this->crntOverlayID);
                this->crntOverlayID = this->crntTexID;
                if (this->showSegments || this->labelling) {
                    float UV0[2] =  {0.0, 0.0};
                    float UV1[2] =  {1.0, 1.0};

                    QUANTIZER::overlayTextures(this->crntTexID, this->crntSegmentID, &this->crntOverlayID,
                                                UV0, UV1, UV0, UV1);
                    if (this->crntOverlayID == 0) this->crntOverlayID = this->crntTexID;
                }

                ImVec2 adjustedSize, offset;
                this->imguiGetImageFillSizeAndOffset(this->crntOverlayID, ImGui::GetWindowSize(), &adjustedSize, &offset);
                ImGui::SetCursorPos(ImGui::GetCursorPos() + offset);
                ImGui::Image((ImTextureID)this->crntOverlayID, adjustedSize);
            }
        }

        if (this->labelling) {
            if (this->crntSegmentID == 0) {
                GLuint temp;
                QUANTIZER::genSegments(this->crntQuantID, &temp);
                if (temp != 0) {
                    glDeleteTextures(1, &this->crntSegmentID);
                    this->crntSegmentID = temp;
                }
            }

            int fontTexDims[2];
            GLWRAP::queryTex(this->crntTexID, fontTexDims, GL_TEXTURE_2D);

            if (ImGui::IsKeyPressed(ImGuiKey_Comma)) this->fontSize--;
            else if (ImGui::IsKeyPressed(ImGuiKey_Period)) this->fontSize++;

            float stampWidth = float(fontTexDims[0])/16 * std::pow(1.1, this->fontSize);

            ImVec2 stampSize = ImVec2(stampWidth, stampWidth);
            float tex2UV0[2] = {float((this->highlightedColor + 48)%16)/16, float((this->highlightedColor + 48)/16)/16};
            float tex2UV1[2] = {float((this->highlightedColor + 48)%16 + 1)/16, float((this->highlightedColor + 48)/16 + 1)/16};


            ImGui::SetCursorPos(ImGui::GetMousePos() - ImGui::GetWindowPos() - stampSize);
            ImGui::Image((ImTextureID)this->fontTex, stampSize,
                            ImVec2(tex2UV0[0], tex2UV0[1]), ImVec2(tex2UV1[0], tex2UV1[1]));
            

            // save
            if (this->highlightedColor > this->numColors) {
                if(!this->saveDialog.IsOpened()) this->saveDialog.Open();
                this->saveDialog.Display();
                if (this->saveDialog.HasSelected()) {
                    // save
                    GLWRAP::saveTex(this->saveDialog.GetSelected().generic_string().c_str(),
                                    this->saveDialog.GetSelected().extension().generic_string().c_str(),
                                    &this->crntSegmentID);

                    this->saveDialog.ClearSelected();
                    this->saveDialog.Close();
                    this->saving = false;
                    this->labelling = false;
                    this->highlightedColor = -1;
                }
            } else {
                ImVec2 mousePos = ImGui::GetMousePos();
                if (mousePos.x > ImGui::GetWindowPos().x && mousePos.y > ImGui::GetWindowPos().y &&
                    mousePos.x < ImGui::GetWindowPos().x + ImGui::GetWindowSize().x && mousePos.y < ImGui::GetWindowPos().y + ImGui::GetWindowSize().y) {
                    if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                        ImVec2 adjustedSize, offset;
                        this->imguiGetImageFillSizeAndOffset(this->crntOverlayID, ImGui::GetWindowSize(), &adjustedSize, &offset);
                        offset = mousePos - ImGui::GetWindowPos() - offset - stampSize;
                        offset.x /= adjustedSize.x;
                        offset.y /= adjustedSize.y;

                        float tex1UV0[2] = {offset.x, offset.y};
                        float tex1UV1[2] = {offset.x + stampWidth/adjustedSize.x, offset.y + stampWidth/adjustedSize.y};
                        
                        GLuint temp;
                        QUANTIZER::overlayTextures(this->crntSegmentID, this->fontTex, &temp, tex1UV0, tex1UV1, tex2UV0, tex2UV1);
                        if (temp != 0) {
                            glDeleteTextures(1, &this->crntSegmentID);
                            this->crntSegmentID = temp;
                        }
                    }
                }
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
                requantize();
            }

            this->loadDialog.ClearSelected();
            this->loadDialog.Close();
            this->loading = false;
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

void GUI::handleInput() {
    if (ImGui::IsKeyPressed(ImGuiKey_LeftArrow)) this->imagePosition.x+=5;
    if (ImGui::IsKeyPressed(ImGuiKey_RightArrow)) this->imagePosition.x-=5;
    if (ImGui::IsKeyPressed(ImGuiKey_DownArrow)) this->imagePosition.y-=5;
    if (ImGui::IsKeyPressed(ImGuiKey_UpArrow)) this->imagePosition.y+=5;

    if (ImGui::IsKeyPressed(ImGuiKey_K)) this->imageScale--;
    if (ImGui::IsKeyPressed(ImGuiKey_L)) this->imageScale++;


}
