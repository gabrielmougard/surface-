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

#include "model.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "shader.h"
#include "camera.h"


class Renderer {
public:
    Renderer() = default;
    ~Renderer() = default;

    void SetupLightning_(glm::vec3 light_point, Shader * shader);
    void Render(BaseCamera *camera, GLfloat screen_scale,  Model *model, Shader *shader, glm::vec3 light_point, glm::mat4 transform);
    void Render(BaseCamera *camera, GLfloat screen_scale,  Model *model, Shader *shader, glm::vec3 light_point, glm::mat4 transform, GLfloat delta_time);
    void RenderThirdPersonCharacter(ThirdPersonCamera *camera, GLfloat screen_scale,  Model *model, Shader *shader, glm::vec3 light_point, glm::vec3 position, glm::vec3 size);
    void RenderCube(BaseCamera *camera, GLfloat screen_scale, GLuint VAO, GLuint texture, Shader *shader, glm::vec3 light_point, glm::mat4 transform);

private:
};