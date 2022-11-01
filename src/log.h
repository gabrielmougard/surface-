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

// Log levels
enum SurfaceppLogLevel : unsigned int
{
    kSurfaceppLogLevelErr = 0x00000001,
    kSurfaceppLogLevelWarn = 0x00000002,
    kSurfaceppLogLevelInfo = 0x00000004,
    kSurfaceppLogLevelDebug = 0x00000008
};


void _log (SurfaceppLogLevel level, const char * requested_format, ...);

void _glfwErrorCallback(int error, const char* description);

void GLAPIENTRY _glMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity,
                                   GLsizei length, const GLchar* message, const void* userParam);

void _flush_log();


#define log_err(...) \
    _log (kSurfaceppLogLevelErr, __VA_ARGS__)

#define log_warn(...) \
    _log (kSurfaceppLogLevelWarn, __VA_ARGS__)

#define log_info(...) \
    _log (kSurfaceppLogLevelInfo, __VA_ARGS__)

#define log_dbg(...) \
    _log (kSurfaceppLogLevelDebug, __VA_ARGS__)