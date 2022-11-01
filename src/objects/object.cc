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

#include "object.h"

Object::Object(GLfloat screen_scale,
               Shader *shader_program,
               BaseCamera *camera,
               glm::vec3 position,
               glm::vec3 size) :
        screen_scale_(screen_scale),
        shader_program_(shader_program),
        camera_(camera),
        position(position),
        size(size) {}


void Object::DoCollisions() {}


void Object::SetupLightning_(glm::vec3 light_point) {
    shader_program_->SetVector3f("light.position", light_point);

    // light properties
    auto time = (GLfloat) glfwGetTime();
    shader_program_->SetVector3f("light.ambient", 1.f, 1.f, 1.f);
    shader_program_->SetVector3f("light.diffuse", 0.1f, cos(2 * time), sin(time));
    shader_program_->SetVector3f("light.specular", 1.0f, .0f, .0f);

    // material properties
    shader_program_->SetVector3f("material.specular", 0.5f, 0.5f, 0.5f);
    shader_program_->SetFloat("material.shininess", 256.0f);
}