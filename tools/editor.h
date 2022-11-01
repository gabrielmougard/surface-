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

#include <string>

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "glm/gtc/random.hpp"
#include "glm/gtx/string_cast.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include <soloud.h>
#include <soloud_wav.h>
#include <soloud_speech.h>
#include <pybind11/embed.h>

#include "log.h"
#include "input.h"
#include "model.h"
#include "shader.h"
#include "camera.h"
#include "audio/audio.h"
#include "scene/scene.h"
#include "scene/entity.h"
#include "text_renderer.h"
#include "objects/object.h"
#include "scene/components.h"
#include "renderer/framebuffer.h"
#include "particles/particle_controller.h"
#include "gui.h"


namespace surfacepp {
    class Editor {
    public:
        Editor();
        virtual ~Editor() = default;

        void OpenglSetup();
        void CreateSceneLayout();

        void Run();
        void OnUpdate(float ts);
        void OnOpenglRender(float ts);
    private:

        void FramebufferSetup();
        void Close();

        GLFWwindow* window_;
        surfacepp::Scene *scene_;
        Framebuffer *framebuffer_;
        Gui *gui_;
    };
}