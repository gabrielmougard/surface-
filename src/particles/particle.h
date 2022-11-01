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

#include "glm/glm.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

struct ParticleParameters {
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec4 color;
    GLfloat gravity_effect;
    GLfloat life_length;
    GLfloat rotation;
    GLfloat scale;
};

class Particle
{
public:
    Particle(ParticleParameters parameters);
    ~Particle() = default;

    void setParameters(ParticleParameters parameters);

    const glm::vec3 &getPosition() const;

    GLfloat getRotation() const;

    GLfloat getScale() const;

    glm::vec4 getColor() const;

    void update(GLfloat delta_time);

    GLboolean isActive() const;

protected:
    ParticleParameters parameters;

    GLfloat elapsed_time = 0;
};
