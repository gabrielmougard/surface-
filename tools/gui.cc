// Copyright 2022 gab
// 
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
//     http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "gui.h"

#include <filesystem>


static int InputTextCallback(ImGuiInputTextCallbackData* data){
    // Resize string callback
    std::string* str = (std::string*)data->UserData;
//    IM_ASSERT(data->Buf == str->c_str());
//    str->resize(data->BufTextLen);
    data->Buf = (char*)str->c_str();
    log_err("str %s", str->c_str());
}


void Gui::ImGuiSetup() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void) io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle &style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }
    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window_, true);
    const char *glsl_version = "#version 130";
    ImGui_ImplOpenGL3_Init(glsl_version);
}

void Gui::onRender() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    // -------

    static bool dockspace_open = true;
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
    dockspace_flags ^= ImGuiDockNodeFlags_NoResize;

    // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
    // because it would be confusing to have two docking targets within each others.
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    ImGuiViewport *viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
                    ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

    // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render a background.
//        if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
//        window_flags |= ImGuiWindowFlags_NoBackground;

    // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
    // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
    // all active windows docked into it will lose their parent and become undocked.
    // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
    // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("DockSpace Demo", &dockspace_open, window_flags);
    ImGui::PopStyleVar();

    ImGui::PopStyleVar(2);

    // DockSpace
    ImGuiIO &io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
    }

    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            // Disabling fullscreen would allow the window to be moved to the front of other windows,
            // which we can't undo at the moment without finer window depth/z control.
            //ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen_persistant);
            if (ImGui::MenuItem("Open last scene('scene.yaml')")){
                auto reg_view = scene_->registry.view<surfacepp::UuidComponent>(entt::exclude<surfacepp::InputComponent>);
                for (const auto entity : reg_view){
                    scene_->registry.destroy(entity);
                }
                serializer_.Deserialize("scene.yaml");
            }
            if (ImGui::MenuItem("Save current scene('scene.yaml')")){
                serializer_.Serialize("scene.yaml");
            }
            if (ImGui::MenuItem("Exit"))
                glfwSetWindowShouldClose(window_, GL_TRUE);
            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }

//    ImGui::Begin("Light settings");
//    static float col4[4] = {};
//    ImGui::ColorEdit4("Color", col4);
//    ImGui::End();

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0, 0});
    ImGui::Begin("Game Viewport    ");

    viewport_focused = ImGui::IsWindowFocused();
    viewport_hovered = ImGui::IsWindowHovered();

//        io = ImGui::GetIO();
//        e.Handled |= e.IsInCategory(EventCategoryMouse) & io.WantCaptureMouse;
//        e.Handled |= e.IsInCategory(EventCategoryKeyboard) & io.WantCaptureKeyboard;

    ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
    if (viewport_size.x != viewportPanelSize.x or viewport_size.y != viewportPanelSize.y) {
        framebuffer_->Resize((uint32_t) viewportPanelSize.x, (uint32_t) viewportPanelSize.y);
        viewport_size = {viewportPanelSize.x, viewportPanelSize.y};
    }

    uint32_t textureID = framebuffer_->GetColorAttachmentRendererID();

    // for now original texture is mirrored
    // updating uv coordinates in here
    // https://github.com/ocornut/imgui/wiki/Image-Loading-and-Displaying-Examples#about-texture-coordinates
    ImVec2 uv0 = ImVec2(0.0, 1.0); // upper-left portion of the texture
    ImVec2 uv1 = ImVec2(1.0, 0.0); // bottom-right portion of the texture
    ImGui::Image(reinterpret_cast<void *>(textureID), ImVec2{viewport_size.x, viewport_size.y}, uv0, uv1);

    ImGui::End();
//        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0, 0});
    ImGui::PopStyleVar();
    ImGui::End();

//    ImGui::ShowDemoWindow();

    renderComponentsTree_();

    // Rendering
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        GLFWwindow *backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
    }
}


void Gui::renderComponentsTree_() {
    ImGui::Begin("Components tree");

    static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;

    auto components_view = this->scene_->registry.view<surfacepp::TagComponent, surfacepp::TransformComponent>();

    this->scene_->registry.each([&](auto entity_handle){
        surfacepp::Entity entity{ entity_handle , scene_ };
        renderEntityNode(entity);
    });

    ImGui::End();
}


void Gui::renderEntityNode(surfacepp::Entity entity) {
    auto& tag = entity.getComponent<surfacepp::TagComponent>();

    ImGuiTreeNodeFlags flags = ((selection_context_ == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
    bool open = ImGui::TreeNodeEx((void *)entity.getHandle(), flags, tag.tag.c_str());
    if (ImGui::IsItemClicked())
    {
        selection_context_ = entity;
    }

    if (open){
        if (entity.hasComponent<surfacepp::TransformComponent>()){
            auto& transform = entity.getComponent<surfacepp::TransformComponent>();
            glm::vec3 rotation = glm::degrees(transform.rotation);
            ImGui::InputFloat3("Position", glm::value_ptr(transform.position));
            if (ImGui::InputFloat3("Rotation", glm::value_ptr(rotation))) {
                transform.rotation = glm::radians(rotation);
            }
            ImGui::InputFloat3("Size", glm::value_ptr(transform.size));

            if (entity.hasComponent<surfacepp::AudioPositionedComponent>()) {
                auto &audio  = entity.getComponent<surfacepp::AudioPositionedComponent>();
                static char file_name[256];
                strcpy(file_name, audio.audio->GetSoundName().c_str());
                if (ImGui::InputText("File name", file_name, 256 ))
                    audio.audio->SetSoundName(file_name);
                if (ImGui::Button("Run playback"))
                    audio.audio->RunPlayback(transform.position);
                if (ImGui::Button("Stop playback"))
                    audio.audio->StopPlayback();
            }
        }
        if (entity.hasComponent<surfacepp::ParticlesComponent>()){
            auto& particles_component = entity.getComponent<surfacepp::ParticlesComponent>();
            ImGui::ColorEdit4("Color", glm::value_ptr(particles_component.controller.referenceParameters.color));
            ImGui::InputFloat3("Position", glm::value_ptr(particles_component.controller.referenceParameters.position));
        }

        if (entity.hasComponent<surfacepp::PyScriptComponent>()){
            auto &script = entity.getComponent<surfacepp::PyScriptComponent>();
            auto &state = entity.getComponent<surfacepp::StateComponent>();
            static char script_path[256];
            strcpy(script_path, script.script_path.c_str());
            if (ImGui::InputText("Script path", script_path, 256))
                script._script_input_path = std::string(script_path);

            if (ImGui::Button("Reload script"))
                state.reload_script_flag = true;
        }

        if (entity.hasComponent<surfacepp::AudioSpeechComponent>()){
            auto &audio = entity.getComponent<surfacepp::AudioSpeechComponent>();

            static char text_to_speak[256];
            strcpy(text_to_speak, audio.audio->GetTextToSpeak().c_str());
            if (ImGui::InputText("Text to speech", text_to_speak, 256 ))
                audio.audio->SetTextToSpeak(text_to_speak);
            if (ImGui::Button("Playback sound"))
                audio.audio->RunPlayback();
        }

        if (entity.hasComponent<surfacepp::AudioBackgroundComponent>()) {
            auto &audio = entity.getComponent<surfacepp::AudioBackgroundComponent>();
            static char file_name[256];
            strcpy(file_name, audio.audio->GetSoundName().c_str());
            if (ImGui::InputText("File name", file_name, 256 ))
                audio.audio->SetSoundName(file_name);
            if (ImGui::Button("Run playback"))
                audio.audio->RunPlayback();
            if (ImGui::Button("Stop playback"))
                audio.audio->StopPlayback();
        }

        {
            auto& state = entity.getComponent<surfacepp::StateComponent>();
            if (ImGui::Button("Remove element")) {
                state.destroy_flag = true;
            }
        }

        ImGui::TreePop();
    }

}

void Gui::Close() {
    log_info("Closing the GUI");
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}