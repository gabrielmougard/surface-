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

#include "shader.h"

#include <cstring>
#include <fstream>
#include <iterator>
#include <string>

#include "log.h"


Shader &Shader::Use()
{
    glUseProgram(program_ID_);
    return *this;
}

void Shader::Compile(const GLchar* vertex_source, const GLchar* fragment_source)
{
    // Vertex Shader
    const GLuint v_shader_ID = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(v_shader_ID, 1, &vertex_source, NULL);
    glCompileShader(v_shader_ID);

    // Fragment Shader
    const GLuint f_shader_ID = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(f_shader_ID, 1, &fragment_source, NULL);
    glCompileShader(f_shader_ID);

    // Shader Program
    program_ID_ = glCreateProgram();
    glAttachShader(program_ID_, v_shader_ID);
    glAttachShader(program_ID_, f_shader_ID);
    glLinkProgram(program_ID_);

    // Detach and delete shaders as they're linked into our program now and no longer necessary
    glDetachShader(program_ID_, v_shader_ID);
    glDeleteShader(v_shader_ID);

    glDetachShader(program_ID_, f_shader_ID);
    glDeleteShader(f_shader_ID);
}

void Shader::SetFloat(const GLchar *name, GLfloat value, GLboolean use_shader)
{
    if (use_shader)
        this->Use();
    glUniform1f(glGetUniformLocation(this->program_ID_, name), value);
}
void Shader::SetInteger(const GLchar *name, GLint value, GLboolean useShader)
{
    if (useShader)
        this->Use();
    glUniform1i(glGetUniformLocation(this->program_ID_, name), value);
}
void Shader::SetVector2f(const GLchar *name, GLfloat x, GLfloat y, GLboolean useShader)
{
    if (useShader)
        this->Use();
    glUniform2f(glGetUniformLocation(this->program_ID_, name), x, y);
}
void Shader::SetVector2f(const GLchar *name, const glm::vec2 &value, GLboolean useShader)
{
    if (useShader)
        this->Use();
    glUniform2f(glGetUniformLocation(this->program_ID_, name), value.x, value.y);
}
void Shader::SetVector3f(const GLchar *name, GLfloat x, GLfloat y, GLfloat z, GLboolean useShader)
{
    if (useShader)
        this->Use();
    glUniform3f(glGetUniformLocation(this->program_ID_, name), x, y, z);
}
void Shader::SetVector3f(const GLchar *name, const glm::vec3 &value, GLboolean useShader)
{
    if (useShader)
        this->Use();
    glUniform3f(glGetUniformLocation(this->program_ID_, name), value.x, value.y, value.z);
}
void Shader::SetVector4f(const GLchar *name, GLfloat x, GLfloat y, GLfloat z, GLfloat w, GLboolean useShader)
{
    if (useShader)
        this->Use();
    glUniform4f(glGetUniformLocation(this->program_ID_, name), x, y, z, w);
}
void Shader::SetVector4f(const GLchar *name, const glm::vec4 &value, GLboolean useShader)
{
    if (useShader)
        this->Use();
    glUniform4f(glGetUniformLocation(this->program_ID_, name), value.x, value.y, value.z, value.w);
}
void Shader::SetMatrix4(const GLchar *name, const glm::mat4 &matrix, GLboolean useShader)
{
    if (useShader)
        this->Use();
    glUniformMatrix4fv(glGetUniformLocation(this->program_ID_, name), 1, GL_FALSE, glm::value_ptr(matrix));
}

Shader *Shader::LoadFromFile(const GLchar *v_shader_file, const GLchar *f_shader_file)
{
    std::string vertex_code;
    if (std::ifstream file_stream{v_shader_file})
    {
        vertex_code = std::string(std::istreambuf_iterator<char>{file_stream}, {});
    }
    else
    {
        log_err("SHADER: Failed to read vertex shader file \"%s\"", v_shader_file);
        return nullptr;
    }

    std::string fragment_code;
    if (std::ifstream file_stream{f_shader_file})
    {
        fragment_code = std::string(std::istreambuf_iterator<char>{file_stream}, {});
    }
    else
    {
        log_err("SHADER: Failed to read fragment shader file \"%s\"", f_shader_file);
        return nullptr;
    }

    Shader *shader = new Shader();
    log_info("Compiling shader with vertex shader \"%s\" and fragment shader \"%s\"", v_shader_file, f_shader_file);
    shader->Compile(vertex_code.c_str(), fragment_code.c_str());
    return shader;
}