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

#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "scene/scene.h"
#include "scene/entity.h"
#include "scene/components.h"
#include "renderer/framebuffer.h"
#include "scene/scene_serializer.h"


class Gui
{
public:
    Gui(GLFWwindow * window,
        surfacepp::Framebuffer * framebuffer,
        surfacepp::Scene * scene
    ) : window_(window), framebuffer_(framebuffer), scene_(scene), serializer_(scene_) {};

    ~Gui() = default;
    void ImGuiSetup();
    void onRender();
    void Close();

private:
    void renderComponentsTree_();
    void renderEntityNode(surfacepp::Entity entity);

    glm::vec2 viewport_size;
    bool viewport_focused = false;
    bool viewport_hovered = false;

    GLFWwindow * window_;
    surfacepp::Framebuffer *framebuffer_;
    surfacepp::Scene * scene_;

    surfacepp::Entity selection_context_;
    surfacepp::SceneSerializer serializer_;
};