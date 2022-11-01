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

#include "particles/particle_renderer.h"

#include "camera.h"
#include "shader.h"


ParticleRenderer::ParticleRenderer()
{
    glGenVertexArrays(1, &this->VAO_);

    // set up positions VBO
    glGenBuffers(1, &this->vertexPositionsVBO_);
    glBindBuffer(GL_ARRAY_BUFFER, this->vertexPositionsVBO_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(particle_vertices_), particle_vertices_, GL_STATIC_DRAW);
    glBindVertexArray(this->VAO_);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), nullptr);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // set up modelView VBO
    createEmptyVBO(instanceDataLength * maxQuadCount);
    createQuadAttributesVBO(1, 4, instanceDataLength, 0);
    createQuadAttributesVBO(2, 4, instanceDataLength, 4);
    createQuadAttributesVBO(3, 4, instanceDataLength, 8);
    createQuadAttributesVBO(4, 4, instanceDataLength, 12);
    createQuadAttributesVBO(5, 4, instanceDataLength, 16);

    glBindVertexArray(this->VAO_);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);
    glEnableVertexAttribArray(4);
    glEnableVertexAttribArray(5);
    glBindVertexArray(0);
}


void ParticleRenderer::render(BaseCamera *camera, const std::vector<Particle>& particles, Shader * shader, GLfloat screen_scale) {
    shader->Use();

    vboBufferWritePosition = -1;
    glm::mat4 projection = glm::perspective(glm::radians(camera->zoom_), screen_scale, 0.1f, 1200.0f);
    shader->SetMatrix4("projectionMatrix", projection);
    for (auto& it : particles) {
        if (it.isActive()) {
            updateModelViewMatrix(it.getPosition(), it.getRotation(), it.getScale(), it.getColor(), camera->GetViewMatrix());
        }
    }
    updateQuadAttributesVBO(particles);
    glBindVertexArray(VAO_);
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, particles.size());
    glBindVertexArray(0);
}


void ParticleRenderer::updateModelViewMatrix(glm::vec3 position, GLfloat rotation, GLfloat scale, glm::vec4 color,
                                             glm::mat4 view) {
    glm::mat4 model = glm::mat4(1);
    model = glm::translate(model, position);
    model[0][0] = view[0][0];
    model[0][1] = view[1][0];
    model[0][2] = view[2][0];
    model[1][0] = view[0][1];
    model[1][1] = view[1][1];
    model[1][2] = view[2][1];
    model[2][0] = view[0][2];
    model[2][1] = view[1][2];
    model[2][2] = view[2][2];
    model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0, 0.0, 1.0));
    model = glm::scale(model, glm::vec3(scale, scale, scale));

    modelViewMatrix_ = view * model;

    vboAttributesBuffer[++vboBufferWritePosition] = modelViewMatrix_[0][0];
    vboAttributesBuffer[++vboBufferWritePosition] = modelViewMatrix_[0][1];
    vboAttributesBuffer[++vboBufferWritePosition] = modelViewMatrix_[0][2];
    vboAttributesBuffer[++vboBufferWritePosition] = modelViewMatrix_[0][3];
    vboAttributesBuffer[++vboBufferWritePosition] = modelViewMatrix_[1][0];
    vboAttributesBuffer[++vboBufferWritePosition] = modelViewMatrix_[1][1];
    vboAttributesBuffer[++vboBufferWritePosition] = modelViewMatrix_[1][2];
    vboAttributesBuffer[++vboBufferWritePosition] = modelViewMatrix_[1][3];
    vboAttributesBuffer[++vboBufferWritePosition] = modelViewMatrix_[2][0];
    vboAttributesBuffer[++vboBufferWritePosition] = modelViewMatrix_[2][1];
    vboAttributesBuffer[++vboBufferWritePosition] = modelViewMatrix_[2][2];
    vboAttributesBuffer[++vboBufferWritePosition] = modelViewMatrix_[2][3];
    vboAttributesBuffer[++vboBufferWritePosition] = modelViewMatrix_[3][0];
    vboAttributesBuffer[++vboBufferWritePosition] = modelViewMatrix_[3][1];
    vboAttributesBuffer[++vboBufferWritePosition] = modelViewMatrix_[3][2];
    vboAttributesBuffer[++vboBufferWritePosition] = modelViewMatrix_[3][3];

    vboAttributesBuffer[++vboBufferWritePosition] = color.x;
    vboAttributesBuffer[++vboBufferWritePosition] = color.y;
    vboAttributesBuffer[++vboBufferWritePosition] = color.z;
    vboAttributesBuffer[++vboBufferWritePosition] = color.w;
}


void ParticleRenderer::createEmptyVBO(uint32_t floatCount) {
    glGenBuffers(1, &this->quadAttributesVBO_);
    glBindBuffer(GL_ARRAY_BUFFER, quadAttributesVBO_);
    glBufferData(GL_ARRAY_BUFFER, floatCount * sizeof(GLfloat), nullptr, GL_STREAM_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ParticleRenderer::createQuadAttributesVBO(uint32_t attribute, uint32_t dataSize, uint32_t instancedDataLength,
                                               uint32_t offset) const {
    /* VAO structure:
     *
     * positionsVBO:      2f 2f, 2f 2f, 2f 2f, 2f 2f   x, y for each vertex of a quad. Each x, y pair obviously goes for different vertex.
     * quadAttributesVBO: 4f, 4f, 4f, 4f, 4f           mat4:ModelViewMatrix, vec4:color for each quad. So glVertexAttribDivisor is used here.
     *
     * */
    glBindBuffer(GL_ARRAY_BUFFER, quadAttributesVBO_);
    glBindVertexArray(VAO_);
    glVertexAttribPointer(attribute, dataSize, GL_FLOAT, false, instancedDataLength * sizeof(GLfloat), (const void *)(offset * sizeof(GLfloat)));
    glVertexAttribDivisor(attribute, 1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}


void ParticleRenderer::updateQuadAttributesVBO(const std::vector<Particle>& particles) {
    glBindBuffer(GL_ARRAY_BUFFER, quadAttributesVBO_);
    glBufferData(GL_ARRAY_BUFFER, instanceDataLength * maxQuadCount * sizeof(GLfloat), vboAttributesBuffer, GL_STREAM_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vboAttributesBuffer), vboAttributesBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}