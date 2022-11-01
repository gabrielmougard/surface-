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

#include "renderer/renderer.h"


void Renderer::SetupLightning_(glm::vec3 light_point, Shader * shader) {
    shader->SetVector3f("light.position", light_point);

    // light properties
    auto time = (GLfloat) glfwGetTime();
    shader->SetVector3f("light.ambient", 1.f, 1.f, 1.f);
    shader->SetVector3f("light.diffuse", 0.1f, cos(2 * time), sin(time));
    shader->SetVector3f("light.specular", 1.0f, .0f, .0f);

    // material properties
    shader->SetVector3f("material.specular", 0.5f, 0.5f, 0.5f);
    shader->SetFloat("material.shininess", 256.0f);
}


void Renderer::Render(BaseCamera *camera,
                 GLfloat screen_scale,
                 Model *model,
                 Shader *shader,
                 glm::vec3 light_point,
                 glm::mat4 transform) {
    glUseProgram(shader->program_ID_);

    this->SetupLightning_(light_point, shader);

    shader->SetMatrix4("model", transform);
    glm::mat4 view = camera->GetViewMatrix();
    shader->SetMatrix4("view", view);
    glm::mat4 projection = glm::perspective(glm::radians(camera->zoom_),
                                            screen_scale, 0.1f, 1200.0f);
    shader->SetMatrix4("projection", projection);

    model->Draw(*shader);
}


void Renderer::Render(BaseCamera *camera, GLfloat screen_scale,  Model *model, Shader *shader, glm::vec3 light_point, glm::mat4 transform, GLfloat delta_time) {
    Render(camera, screen_scale, model, shader, light_point, transform);
}

void Renderer::RenderThirdPersonCharacter(ThirdPersonCamera *camera, GLfloat screen_scale,  Model *model, Shader *shader, glm::vec3 light_point, glm::vec3 position, glm::vec3 size) {
    glUseProgram(shader->program_ID_);
    this->SetupLightning_(light_point, shader);

    shader->SetVector3f("viewPos", ((ThirdPersonCamera *) camera)->position_);

    glm::mat4 projection = glm::perspective(glm::radians(45.f), screen_scale, 0.1f, 500.0f);
    shader->SetMatrix4("projection", projection);

    glm::mat4 mod_matrix = glm::mat4(1.0f);
    mod_matrix = glm::scale(mod_matrix, size);

    shader->SetMatrix4("model", mod_matrix);

    glm::mat4 view = camera->GetViewMatrix();
    view = glm::translate(view, position);
    shader->SetMatrix4("view", view);

//    glBindVertexArray(VAO_);
    model->Draw(*shader);
}


void Renderer::RenderCube(BaseCamera *camera, GLfloat screen_scale, GLuint VAO, GLuint texture, Shader *shader,
                          glm::vec3 light_point, glm::mat4 transform) {
    glUseProgram(shader->program_ID_);
    this->SetupLightning_(light_point, shader);
    shader->SetVector3f("viewPos", camera->position_);

    // bind textures on corresponding texture units
    // bind diffuse map
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    glm::mat4 view = camera->GetViewMatrix();
    glm::mat4 projection = glm::perspective(glm::radians(camera->zoom_), screen_scale, 0.1f, 1200.0f);

    shader->SetMatrix4("model", transform);
    shader->SetMatrix4("view", view);
    shader->SetMatrix4("projection", projection);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}