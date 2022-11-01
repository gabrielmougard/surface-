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

#include "objects/object.h"

#include "stb_image.h"

#include "camera.h"

CustomGeometryObject::CustomGeometryObject(GLfloat screen_scale,
                                           Shader *shader_program,
                                           BaseCamera *camera,
                                           const char *texture_name,
                                           glm::vec3 position,
                                           glm::vec3 size) :
        Object(screen_scale, shader_program, camera, position, size) {
    CustomGeometryObject::Init(texture_name);
};


void CustomGeometryObject::loadTexture_(char const *path) {
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


void CustomGeometryObject::Init(char const *path) {
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
    loadTexture_(path);
    shader_program_->Use();
    shader_program_->SetInteger("texture1", 0);
}


void CustomGeometryObject::Render(glm::vec3 light_point) {
    glUseProgram(shader_program_->program_ID_);
    this->SetupLightning_(light_point);
    shader_program_->SetVector3f("viewPos", camera_->position_);

    // bind textures on corresponding texture units
    // bind diffuse map
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::scale(model, size);

    glm::mat4 view = camera_->GetViewMatrix();

    glm::mat4 projection = glm::perspective(glm::radians(camera_->zoom_), screen_scale_, 0.1f, 1200.0f);

    shader_program_->SetMatrix4("model", model);
    shader_program_->SetMatrix4("view", view);
    shader_program_->SetMatrix4("projection", projection);

    glBindVertexArray(VAO_);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

void CustomGeometryObject::DoCollisions() {}