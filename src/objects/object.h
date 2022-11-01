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

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "log.h"
#include "model.h"
#include "shader.h"
#include "camera.h"
#include "objects/object.h"

class BaseCamera;

class Object {
protected:
    GLuint VAO_ = 0;

    GLfloat screen_scale_;
    Shader *shader_program_;
    BaseCamera *camera_;

public:
    glm::vec3 position;
    glm::vec3 size;

    Object(GLfloat screen_scale,
           Shader *shader_program,
           BaseCamera *camera,
           glm::vec3 position,
           glm::vec3 size);

    virtual ~Object() = default;

    virtual void Render(glm::vec3 light_point) = 0;

    virtual void DoCollisions();

    virtual void SetupLightning_(glm::vec3 light_point);
};


class CustomGeometryObject : public Object {
protected:
    GLuint VBO_ = 0;
    GLuint texture = 0;
    GLfloat _vertices[288] = {
            // positions          // normals           // texture coords
             -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
             0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
             0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
             0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
            -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

            -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
             0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
             0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
             0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
            -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
            -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

            -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
            -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
            -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
            -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
            -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
            -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

             0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
             0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
             0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
             0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
             0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
             0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

            -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
             0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
             0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
             0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

            -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
             0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
             0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
             0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
            -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
            -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
    };

    virtual void loadTexture_(char const *path);

public:
    CustomGeometryObject(GLfloat screen_scale,
                         Shader *shader_program,
                         BaseCamera *camera,
                         const char *texture_name,
                         glm::vec3 position,
                         glm::vec3 size);

    void Init(char const *path);

    void Render(glm::vec3 light_point) override;

    void DoCollisions() override;
};


class ModeledObject : public Object {
protected:
    Model *model_ = nullptr;

public:
    ModeledObject(GLfloat screen_scale,
                  Shader *shader_program,
                  Model *model,
                  BaseCamera *camera,
                  glm::vec3 position,
                  glm::vec3 size);

    void Render(glm::vec3 light_point) override;

    void DoCollisions() override;

    GLboolean isSticked = false;
};


class MoveRotationKeeper {

public:
    MoveRotationKeeper() = default;

    void FlushDirections() {
        right_direction_ = 0.0f;
        front_direction_ = 0.0f;
    }

    void SetMoveRight() {
        right_direction_ = 1.0f;
        last_x_rotation_ = right_direction_;
    }

    void SetMoveLeft() {
        right_direction_ = -1.0f;
        last_x_rotation_ = right_direction_;
    }

    void SetMoveForward() {
        front_direction_ = 1.0f;
        last_z_rotation_ = front_direction_;
    }

    void SetMoveBackward() {
        front_direction_ = -1.0f;
        last_z_rotation_ = front_direction_;
    }

    void SetLastModelMatrix(glm::mat4 model) {
        this->model_ = model;
    }

    glm::mat4 GetLastModelMatrix() {
        return this->model_;
    }

    GLboolean IsMovingBothDirections() const {
        return right_direction_ && front_direction_;
    }

    GLfloat GetFrontDirection() const {
        return front_direction_;
    }

    GLfloat GetRightDirection() const {
        return right_direction_;
    }

    GLfloat CalculateFrontRotation(GLfloat rotation_step) {
        front_rotation_amount_ += front_direction_ * rotation_step;
        return front_rotation_amount_;
    }

    GLfloat CalculateRightRotation(GLfloat rotation_step) {
        right_rotation_amount_ += right_direction_ * rotation_step;
        return right_rotation_amount_;
    }

private:
    GLfloat right_direction_ = 0.0f;
    GLfloat front_direction_ = 0.0f;

    GLfloat front_rotation_amount_ = 0.0f;
    GLfloat right_rotation_amount_ = 0.0f;

    GLfloat last_z_rotation_ = 0.0f;
    GLfloat last_x_rotation_ = 0.0f;

    glm::mat4 model_ = glm::mat4(0);
};


class ThirdPersonCharacter : public ModeledObject {
public:
    ThirdPersonCharacter(GLfloat screen_scale,
                         Shader *shader_program,
                         Model *model,
                         ThirdPersonCamera *camera,
                         glm::vec3 size);

    ~ThirdPersonCharacter() override = default;

    glm::vec3 GetFront() const;

    glm::vec3 GetRight() const;

    void Render(glm::vec3 light_point) override;

    void ProcessRotation();

    GLfloat GetRotationMultiplier();

private:
    glm::mat4 model_matrix = glm::mat4(0);

    GLfloat speed_ = 1.0f;
    GLfloat time_ = 0.0f;

    MoveRotationKeeper rotation_keeper_;
};