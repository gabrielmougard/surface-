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
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"


class Shader
{
public:
    GLuint program_ID_;
    Shader() { }
    Shader  &Use();
    void    Compile(const GLchar *vertex_source, const GLchar *fragment_source);
    // Utility functions
    void    SetFloat    (const GLchar *name, GLfloat value, GLboolean use_shader = false);
    void    SetInteger  (const GLchar *name, GLint value, GLboolean use_shader = false);
    void    SetVector2f (const GLchar *name, GLfloat x, GLfloat y, GLboolean use_shader = false);
    void    SetVector2f (const GLchar *name, const glm::vec2 &value, GLboolean use_shader = false);
    void    SetVector3f (const GLchar *name, GLfloat x, GLfloat y, GLfloat z, GLboolean use_shader = false);
    void    SetVector3f (const GLchar *name, const glm::vec3 &value, GLboolean use_shader = false);
    void    SetVector4f (const GLchar *name, GLfloat x, GLfloat y, GLfloat z, GLfloat w, GLboolean use_shader = false);
    void    SetVector4f (const GLchar *name, const glm::vec4 &value, GLboolean use_shader = false);
    void    SetMatrix4  (const GLchar *name, const glm::mat4 &matrix, GLboolean use_shader = false);

    static Shader* LoadFromFile(const GLchar *v_shader_file, const GLchar *f_shader_file);
};