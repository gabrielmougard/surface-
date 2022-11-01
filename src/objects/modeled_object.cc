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

#include "camera.h"


ModeledObject::ModeledObject(GLfloat screen_scale,
                             Shader *shader_program,
                             Model *model,
                             BaseCamera *camera,
                             glm::vec3 position,
                             glm::vec3 size) :
        Object(screen_scale, shader_program, camera, position, size),
        model_(model) {}


void ModeledObject::Render(glm::vec3 light_point) {
    glUseProgram(shader_program_->program_ID_);
    this->SetupLightning_(light_point);

    glm::mat4 model = glm::mat4(1.0f);

    model = glm::translate(model, position);
    model = glm::scale(model, size);

    shader_program_->SetMatrix4("model", model);
    glm::mat4 view = camera_->GetViewMatrix();
    shader_program_->SetMatrix4("view", view);
    glm::mat4 projection = glm::perspective(glm::radians(camera_->zoom_), screen_scale_, 0.1f, 1200.0f);
    shader_program_->SetMatrix4("projection", projection);

    model_->Draw(*shader_program_);
}


void ModeledObject::DoCollisions() {}