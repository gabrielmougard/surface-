#pragma once

#include "glad/glad.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "log.h"
#include "input.h"

enum class CameraMovement {
    kForward,
    kBackward,
    kLeft,
    kRight
};

const float kYaw         = -90.0f;
const float kPitch       =  0.0f;
const float kSpeed       =  80.0f;
const float kSensitivity =  0.1f;
const float kZoom        =  45.0f;


inline const char* FREE_CAMERA_CLASSNAME = "BaseCamera";
inline const char* THIRD_PERSON_CAMERA_CLASSNAME = "ThirdPersonCamera";
inline const char* PLATFORMER_CAMERA_CLASSNAME = "PlatformerCamera";


class BaseCamera {
public:
    glm::vec3 position_;
    glm::vec3 front_;
    glm::vec3 up_;
    glm::vec3 right_;
    glm::vec3 world_up_;
    float yaw_;
    float pitch_;

    float movement_speed_;
    float mouse_sensitivity_;
    float zoom_;

    glm::vec3 main_character_position_;
    glm::vec3 spring_arm_offset_;

    explicit BaseCamera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
                    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
                    float yaw = kYaw,
                    float pitch = kPitch);
    ~BaseCamera();
    virtual glm::vec3 FrontXZ();
    virtual glm::vec3 RightXZ();
    virtual glm::mat4 GetViewMatrix();
    virtual glm::vec3 GetFront() const;
    virtual glm::vec3 GetRight() const;
    virtual glm::vec3 ProcessKeyboard(CameraMovement direction,
                                      float delta_time,
                                      GLfloat speed,
                                      glm::vec3 position);
    virtual void ProcessMouseMovement(float x_offset,
                                      float y_offset,
                                      GLboolean constrain_pitch);
    virtual void ProcessMouseScroll(float y_offset);
protected:
    virtual void UpdateCameraVectors();
};


class ThirdPersonCamera: public BaseCamera {
public:
    glm::vec3 main_character_position_;
    GLfloat spring_arm_length_;
    glm::vec3 spring_arm_offset_;

    explicit ThirdPersonCamera(GLfloat spring_arm_length = 40.f,
                               glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f));
    ~ThirdPersonCamera();
    void ProcessMouseMovement(float x_offset,
                              float y_offset,
                              GLboolean constrain_pitch) override;
    glm::vec3 FrontXZ() override;
    glm::vec3 RightXZ() override;
    glm::mat4 GetViewMatrix() override;
    glm::vec3 GetFront() const override;
    glm::vec3 GetRight() const override;
    glm::vec3 ProcessKeyboard(CameraMovement direction,
                              float delta_time,
                              GLfloat speed,
                              glm::vec3 position) override;
protected:
    void UpdateCameraVectors() override;
};


class PlatformerCamera: public BaseCamera {
public:
    GLfloat spring_arm_length_;
    explicit PlatformerCamera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f));
    glm::vec3 GetFront() const override;
    glm::mat4 GetViewMatrix() override;

    void ProcessMouseMovement(float x_offset,
                              float y_offset,
                              GLboolean constrain_pitch) override;
    glm::vec3 ProcessKeyboard(CameraMovement direction,
                              float delta_time,
                              GLfloat speed,
                              glm::vec3 position) override;
};