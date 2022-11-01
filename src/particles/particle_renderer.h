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

#include <vector>
#include "particles/particle.h"

class BaseCamera;
class Shader;


static const size_t maxQuadCount = 10000;
static const size_t maxVertexCount = maxQuadCount * 4;
static const size_t maxIndexCount = maxQuadCount * 6;
static const size_t instanceDataLength = 20; // mat4 ModelView matrix + vec4 for colors

class ParticleRenderer
{
public:
    ParticleRenderer();
    ~ParticleRenderer() = default;

    void render(BaseCamera *camera, const std::vector<Particle>& particles, Shader * shader, GLfloat screen_scale) ;
    void updateModelViewMatrix(glm::vec3 position, GLfloat rotation, GLfloat scale, glm::vec4 color, glm::mat4 view);

protected:
        static constexpr GLfloat particle_vertices_[8] = {
             0.5f, -0.5f,    //  3     1
             0.5f,  0.5f,    //
            -0.5f, -0.5f,    //  2     0
            -0.5f,  0.5f     // we are using GL_TRIANGLE_STRIP
        };

    void createEmptyVBO(uint32_t floatCount);
    void createQuadAttributesVBO(uint32_t attribute, uint32_t dataSize, uint32_t instancedDataLength, uint32_t offset) const;
    void updateQuadAttributesVBO(const std::vector<Particle>& particles);

    glm::mat4 modelViewMatrix_;
    GLuint VAO_;
    GLuint vertexPositionsVBO_;
    GLuint quadAttributesVBO_;

    GLfloat vboAttributesBuffer[instanceDataLength * maxQuadCount];
    uint64_t vboBufferWritePosition = 0;
};