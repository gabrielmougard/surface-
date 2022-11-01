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

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <soloud.h>

#include <map>
#include <iostream>
#include <random>
#include <sstream>

#include "model.h"
#include "input.h"
#include "shader.h"
#include "camera.h"
#include "uuid.h"
#include "audio/audio.h"
#include "objects/object.h"
#include "particles/particle_controller.h"
#include "ai/action.h"
#include "ai/world_state.h"


namespace surfacepp {
    struct UuidComponent {
        std::string uuid;
        explicit UuidComponent(const std::string &uuid) {
            if (uuid.empty())
                this->uuid = UUID::generate_uuid_v4();
            else
                this->uuid = uuid;
        }
    };

    struct StateComponent {
        bool create_flag = true;
        bool destroy_flag = false;
        bool reload_script_flag = true;

        StateComponent() = default;
    };

    struct TagComponent {
        std::string tag;

        TagComponent() = default;

        TagComponent(const TagComponent &) = default;

        explicit TagComponent(const std::string &tag)
                : tag(tag) {}
    };

    struct TransformComponent {
        glm::vec3 position;
        glm::vec3 rotation;
        glm::vec3 size;

        bool is_looking_at = false;
        glm::vec3 look_at{0.0f, 0.0f, 0.0f};

        TransformComponent(glm::vec3 position, glm::vec3 rotation, glm::vec3 size) : position(position), rotation(rotation), size(size) {};

        glm::mat4 getTransform() const {
            if (!is_looking_at){
                glm::mat4 rot = glm::toMat4(glm::quat(rotation));
                return glm::translate(glm::mat4(1.0f), position)
                                      * rot
                                      * glm::scale(glm::mat4(1.0f), size);
            } else {
                glm::mat4 rot = glm::inverse(glm::lookAt(position, look_at, glm::vec3(0, 1, 0)));
                return rot * glm::scale(glm::mat4(1.0f), size);
            }
        }
    };

    struct ShaderProgramComponent {
        std::string v_shader_path;

        explicit ShaderProgramComponent(const char *v_shader_path) : v_shader_path(v_shader_path) {};
    };


    struct ModelComponent {
        explicit ModelComponent(const char *path) :
                model_path(std::string(path)) {}

        std::string model_path;
    };


    struct CameraComponent {
        GLfloat input_speed;
        BaseCamera *camera;

        explicit CameraComponent(BaseCamera *camera, GLfloat input_speed): camera(camera), input_speed(input_speed) {
            this->camera->GetViewMatrix();
        };

        glm::vec3 ProcessKeyboard(CameraMovement direction, float delta_time, GLfloat speed, glm::vec3 position) {
            return camera->ProcessKeyboard(direction, delta_time, speed, position);
        }

        void ProcessMouseMovement(float x_offset, float y_offset, GLboolean constrain_pitch = true) {
            camera->ProcessMouseMovement(x_offset, y_offset, constrain_pitch);
        }

        BaseCamera * GetCamera() {
            return camera;
        }
    };

    struct InputComponent {
        surfacepp::Input input;

        explicit InputComponent(GLFWwindow *window) : input(window) {};
    };

    struct AudioCoreComponent {
        SoLoud::Soloud soloud;

        explicit AudioCoreComponent(SoLoud::Soloud soloud) : soloud(soloud) {};
    };

    struct ScreenScaleComponent {
        GLfloat screen_scale;

        explicit ScreenScaleComponent(GLfloat screen_scale) : screen_scale(screen_scale) {};
    };

    struct ModelsCacheComponent {
        std::map<std::string, Model> cache;

        explicit ModelsCacheComponent(std::map<string, Model> models) : cache(std::move(models)) {};
    };

    struct ShadersCacheComponent {
        std::map<std::string, Shader> cache;

        explicit ShadersCacheComponent(std::map<string, Shader> shaders) : cache(std::move(shaders)) {};
    };

    struct IlluminateCacheComponent {
        explicit IlluminateCacheComponent(std::vector<glm::vec3> light_sources) : light_sources(
                std::move(light_sources)) {
        };

        std::vector<glm::vec3> light_sources;
    };

    struct ThirdPersonCharacterComponent {
        bool is_third_person_char = true;

        explicit ThirdPersonCharacterComponent(bool is_tpc) : is_third_person_char(is_tpc) {};
    };

    struct CubeObjectComponent {
        GLfloat _vertices[288] = {
                // positions         // normals        // texture coords
                -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
                0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,
                0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
                0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
                -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,
                -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,

                -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
                0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
                0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
                0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
                -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
                -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,

                -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
                -0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
                -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
                -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
                -0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,

                0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
                0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
                0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
                0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
                0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,

                -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
                0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,
                0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
                0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
                -0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,
                -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,

                -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
                0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
                0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
                0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
                -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
                -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f
        };
        GLuint VBO_ = 0;
        GLuint VAO_ = 0;
        GLuint texture = 0;
        std::string texture_path;

        explicit CubeObjectComponent(const char *texture_path) : texture_path(texture_path) {
            glGenVertexArrays(1, &VAO_);
            glGenBuffers(1, &VBO_);

            glBindVertexArray(VAO_);

            glBindBuffer(GL_ARRAY_BUFFER, VBO_);
            glBufferData(GL_ARRAY_BUFFER, sizeof(_vertices), _vertices, GL_STATIC_DRAW);

            // position attribute
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) 0);
            glEnableVertexAttribArray(0);
            // normales coord attribute
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (3 * sizeof(float)));
            glEnableVertexAttribArray(1);
            // texture coord attribute
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (6 * sizeof(float)));
            glEnableVertexAttribArray(2);

            // texture
            loadTexture_(texture_path);
        };

        void loadTexture_(char const *path) {
            glGenTextures(1, &texture);

            int width, height, nrComponents;
            unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
            if (data) {
                GLenum format;
                if (nrComponents == 1)
                    format = GL_RED;
                else if (nrComponents == 3)
                    format = GL_RGB;
                else if (nrComponents == 4)
                    format = GL_RGBA;
                else {
                    log_err("CustomGeometryObject: unable to determine texture format");
                    format = GL_RED;
                }

                glBindTexture(GL_TEXTURE_2D, texture);
                glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
                glGenerateMipmap(GL_TEXTURE_2D);

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

                stbi_image_free(data);
            } else {
                log_err("Failed to load texture %s", path);
                stbi_image_free(data);
            }
        }
    };

    struct ParticlesComponent {
        ParticleController controller;

        ParticlesComponent(ParticleParameters parameters, uint32_t particles_number) : controller(parameters,
                                                                                                  particles_number) {}
    };

    struct AudioPositionedComponent {
        AudioPositioned * audio;
        bool is_playing = false;

        explicit AudioPositionedComponent(AudioPositioned *audio) : audio(audio) {};
    };

    struct AudioBackgroundComponent {
        AudioBackground * audio;
        bool is_playing = false;

        explicit AudioBackgroundComponent(AudioBackground * audio) : audio(audio) {};
    };

    struct AudioSpeechComponent {
        AudioSpeech * audio;
        bool is_playing = false;

        explicit AudioSpeechComponent(AudioSpeech * audio) : audio(audio) {};
    };

    struct PyScriptComponent {
        std::string script_path;
        std::string _script_input_path;

        explicit PyScriptComponent(const char * script_path) : script_path(script_path), _script_input_path(script_path) {};
    };

    struct AIComponent {
        std::vector<const goap::Action *> actions_list;
        goap::WorldState initial_world_state;
        goap::WorldState goal_world_state;
        int (*heuristicFunctionPointer)(const goap::WorldState &node, const goap::WorldState &goal);

        explicit AIComponent(std::vector<const goap::Action *> actions,
                             goap::WorldState initial,
                             goap::WorldState goal,
                             int (*heuristicFunction)(const goap::WorldState &node, const goap::WorldState &goal)):
        actions_list(std::move(actions)),
        initial_world_state(std::move(initial)),
        goal_world_state(std::move(goal)),
        heuristicFunctionPointer(heuristicFunction)
        {};
    };

   struct AITargetComponent {
       // Indicates that this entity acts as a target for some action
       std::string action_name;

       explicit AITargetComponent(std::string action_name): action_name(std::move(action_name)){};
   };
}