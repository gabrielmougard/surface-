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

#include "camera.h"


// Camera


BaseCamera::BaseCamera(glm::vec3 position,
                       glm::vec3 up,
                       float yaw,
                       float pitch):
                       front_(glm::vec3(0.0f, 0.0f, -1.0f)),
                       movement_speed_(kSpeed),
                       mouse_sensitivity_(kSensitivity),
                       zoom_(kZoom)
{
    position_ = position;
    world_up_ = up;
    yaw_ = yaw;
    pitch_ = pitch;
    UpdateCameraVectors();
}

BaseCamera::~BaseCamera(){};

glm::vec3 BaseCamera::FrontXZ()
{
    return glm::vec3(this->front_[0], 0.0f, this->front_[2]);
}

glm::vec3 BaseCamera::RightXZ()
{
    return glm::vec3(this->right_[0], 0.0f, this->right_[2]);
}

glm::mat4 BaseCamera::GetViewMatrix()
{
    return glm::lookAt(position_, position_ + front_, up_);
}

glm::vec3 BaseCamera::GetFront() const{
    return glm::vec3(front_[0], 0.0f, front_[2]);
}

glm::vec3 BaseCamera::GetRight() const{
    return right_;
}

glm::vec3 BaseCamera::ProcessKeyboard(CameraMovement direction,
                                  float delta_time,
                                  GLfloat speed,
                                  glm::vec3 position)
{
    float velocity = movement_speed_ * delta_time;
    if (direction == CameraMovement::kForward)
        position_ += front_ * velocity;
    if (direction == CameraMovement::kBackward)
        position_ -= front_ * velocity;
    if (direction == CameraMovement::kLeft)
        position_ -= right_ * velocity;
    if (direction == CameraMovement::kRight)
        position_ += right_ * velocity;
    return position_;
}

void BaseCamera::ProcessMouseMovement(float x_offset,
                                  float y_offset,
                                  GLboolean constrain_pitch)
{
    x_offset *= mouse_sensitivity_;
    y_offset *= mouse_sensitivity_;

    yaw_   += x_offset;
    pitch_ += y_offset;

    if (constrain_pitch)
    {
        if (pitch_ > 89.0f)
            pitch_ = 89.0f;
        if (pitch_ < -89.0f)
            pitch_ = -89.0f;
    }

    UpdateCameraVectors();
}

void BaseCamera::ProcessMouseScroll(float y_offset)
{
    if (zoom_ >= 1.0f && zoom_ <= 45.0f)
        zoom_ -= y_offset;
    if (zoom_ <= 1.0f)
        zoom_ = 1.0f;
    if (zoom_ >= 45.0f)
        zoom_ = 45.0f;
}

void BaseCamera::UpdateCameraVectors()
{
    glm::vec3 front;
    front.x = cos(glm::radians(yaw_)) * cos(glm::radians(pitch_));
    front.y = sin(glm::radians(pitch_));
    front.z = sin(glm::radians(yaw_)) * cos(glm::radians(pitch_));
    front_ = glm::normalize(front);
    right_ = glm::normalize(glm::cross(front_, world_up_));
    up_    = glm::normalize(glm::cross(right_, front_));
}


// TPC


ThirdPersonCamera::ThirdPersonCamera(GLfloat spring_arm_length,
                                     glm::vec3 position):
                                     spring_arm_length_(spring_arm_length),
                                     BaseCamera(position)
{
    pitch_ = -20.0f;
    spring_arm_offset_ = glm::vec3(0.0f, -pitch_, spring_arm_length_);
    this->main_character_position_ = position;
    this->position_ = this->main_character_position_ + spring_arm_offset_;
    UpdateCameraVectors();
}


ThirdPersonCamera::~ThirdPersonCamera(){};


void ThirdPersonCamera::ProcessMouseMovement(float x_offset,
                                             float y_offset,
                                             GLboolean constrain_pitch)
{
    x_offset *= mouse_sensitivity_;
    y_offset *= mouse_sensitivity_;

    if (abs(x_offset) > 0.000001 || abs(y_offset) > 0.000001){
        yaw_   += x_offset;
        pitch_ += y_offset;
        GLfloat x = glm::sin(glm::radians(yaw_)) * spring_arm_length_;
        GLfloat y = glm::cos(glm::radians(yaw_)) * spring_arm_length_;

        if (constrain_pitch)
        {
            if (pitch_ > -0.0f)
                pitch_ = -0.0f;
            if (pitch_ < -30.0f)
                pitch_ = -30.0f;
        }

        spring_arm_offset_ = glm::vec3(-y, -pitch_, -x);
        this->position_ = this->main_character_position_ + spring_arm_offset_;
        UpdateCameraVectors();
    }
}

glm::vec3 ThirdPersonCamera::FrontXZ()
{
    return glm::vec3(this->front_[0], 0.0f, this->front_[2]);
}

glm::vec3 ThirdPersonCamera::RightXZ()

{
    return glm::vec3(this->right_[0], 0.0f, this->right_[2]);
}

glm::mat4 ThirdPersonCamera::GetViewMatrix()
{
    return glm::lookAt(position_, position_ + front_, up_);
}

glm::vec3 ThirdPersonCamera::GetFront() const {
    return glm::vec3(front_[0], 0.0f, front_[2]);
}

glm::vec3 ThirdPersonCamera::GetRight() const {
    return right_;
}


glm::vec3 ThirdPersonCamera::ProcessKeyboard(CameraMovement direction,
                                             float delta_time,
                                             GLfloat speed,
                                             glm::vec3 position)
{
    float velocity = delta_time * speed * 120;
    glm::vec3 new_position;

    if (direction == CameraMovement::kForward)
        new_position = position + this->GetFront() * velocity;
    if (direction == CameraMovement::kBackward)
        new_position = position - this->GetFront() * velocity;
    if (direction == CameraMovement::kLeft)
        new_position = position - this->GetRight() * velocity;
    if (direction == CameraMovement::kRight)
        new_position = position + this->GetRight() * velocity;

    main_character_position_ = new_position;
    position_ = main_character_position_ + spring_arm_offset_;
    UpdateCameraVectors();
    return main_character_position_;
}

void ThirdPersonCamera::UpdateCameraVectors()
{
    glm::vec3 front;
    front.x = cos(glm::radians(yaw_)) * cos(glm::radians(pitch_));
    front.y = sin(glm::radians(pitch_));
    front.z = sin(glm::radians(yaw_)) * cos(glm::radians(pitch_));
    front_ = glm::normalize(front);
    right_ = glm::normalize(glm::cross(front_, world_up_));
    up_    = glm::normalize(glm::cross(right_, front_));
}

PlatformerCamera::PlatformerCamera(glm::vec3 position): BaseCamera(position){
    pitch_ = -20.0f;
    spring_arm_offset_ = glm::vec3(0.0f, -pitch_, spring_arm_length_);
    this->main_character_position_ = position;
    this->position_ = this->main_character_position_ + spring_arm_offset_;
}

glm::mat4 PlatformerCamera::GetViewMatrix() {
    return glm::lookAt(position_ + glm::vec3(0, 140, 0),
                       position_,
                       glm::vec3(0,0,-1));
}

void PlatformerCamera::ProcessMouseMovement(float x_offset,
                                            float y_offset,
                                            GLboolean constrain_pitch)
{}

glm::vec3 PlatformerCamera::GetFront() const{
    return glm::vec3(front_[0], 0.0f, front_[2]);
}

glm::vec3 PlatformerCamera::ProcessKeyboard(CameraMovement direction,
                                            float delta_time,
                                            GLfloat speed,
                                            glm::vec3 position) {
    float velocity = delta_time * speed * 120;
    glm::vec3 new_position;

    if (direction == CameraMovement::kForward)
        new_position = position + this->GetFront() * velocity;
    if (direction == CameraMovement::kBackward)
        new_position = position - this->GetFront() * velocity;
    if (direction == CameraMovement::kLeft)
        new_position = position - this->GetRight() * velocity;
    if (direction == CameraMovement::kRight)
        new_position = position + this->GetRight() * velocity;

    main_character_position_ = new_position;
    position_ = main_character_position_ + spring_arm_offset_;
    return main_character_position_;
}