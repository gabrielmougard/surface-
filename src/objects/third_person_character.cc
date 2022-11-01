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


ThirdPersonCharacter::ThirdPersonCharacter(
        GLfloat screen_scale,
        Shader *shader_program,
        Model *model,
        ThirdPersonCamera *camera,
        glm::vec3 size
) :
        ModeledObject(screen_scale, shader_program, model, nullptr, glm::vec3(0), size) {
    camera_ = camera;
    rotation_keeper_.SetLastModelMatrix(glm::rotate(glm::mat4(1), glm::radians(90.0f), glm::vec3(0, 1.0, 0)));
    glGenVertexArrays(1, &VAO_);
}

glm::vec3 ThirdPersonCharacter::GetFront() const{
    return glm::vec3(((ThirdPersonCamera *) camera_)->front_[0],
                     0.0f,
                     ((ThirdPersonCamera *) camera_)->front_[2]);
}

glm::vec3 ThirdPersonCharacter::GetRight() const{
    return ((ThirdPersonCamera *) camera_)->right_;
}


void ThirdPersonCharacter::Render(glm::vec3 light_point) {
    glUseProgram(shader_program_->program_ID_);
    this->SetupLightning_(light_point);

    shader_program_->SetVector3f("viewPos", ((ThirdPersonCamera *) camera_)->position_);

    glm::mat4 projection = glm::perspective(glm::radians(45.f), screen_scale_, 0.1f, 500.0f);
    shader_program_->SetMatrix4("projection", projection);

    this->ProcessRotation();

    glm::mat4 view = ((ThirdPersonCamera *) camera_)->GetViewMatrix();
    view = glm::translate(view, position);
    shader_program_->SetMatrix4("view", view);

    glBindVertexArray(VAO_);
    model_->Draw(*shader_program_);
}

void ThirdPersonCharacter::ProcessRotation() {
    model_matrix = glm::rotate(glm::mat4(1), glm::radians(90.0f), glm::vec3(0, 1.0, 0));
    GLfloat rotation_delta = GetRotationMultiplier();

    if (rotation_keeper_.IsMovingBothDirections()) {
        model_matrix = glm::rotate(model_matrix,
                                   rotation_keeper_.GetFrontDirection() *
                                   rotation_keeper_.CalculateFrontRotation(rotation_delta),
                                   rotation_keeper_.GetFrontDirection() * GetFront() +
                                   rotation_keeper_.GetRightDirection() * GetRight());
        rotation_keeper_.SetLastModelMatrix(model_matrix);
    } else if (rotation_keeper_.GetFrontDirection()) {
        model_matrix = glm::rotate(model_matrix,
                                   rotation_keeper_.CalculateFrontRotation(rotation_delta),
                                   GetFront());
        rotation_keeper_.SetLastModelMatrix(model_matrix);
    } else if (rotation_keeper_.GetRightDirection()) {
        model_matrix = glm::rotate(model_matrix,
                                   rotation_keeper_.CalculateRightRotation(rotation_delta),
                                   GetRight());
        rotation_keeper_.SetLastModelMatrix(model_matrix);
    } else {
        model_matrix = rotation_keeper_.GetLastModelMatrix();
    }

    rotation_keeper_.FlushDirections();

    model_matrix = glm::scale(model_matrix, size);
    shader_program_->SetMatrix4("model", model_matrix);
}

GLfloat ThirdPersonCharacter::GetRotationMultiplier() {
    if (time_ == 0.0f)
        time_ = (float) glfwGetTime();

    GLfloat old_time = time_;
    time_ = (float) glfwGetTime();
    return 9 * (time_ - old_time);
}