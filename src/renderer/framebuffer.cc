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

#include "renderer/framebuffer.h"
#include "log.h"

namespace surfacepp {

    static const uint32_t s_MaxFramebufferSize = 8192;

    Framebuffer::Framebuffer(const FramebufferSpecification& spec)
            : specification(spec)
    {
        Invalidate();
    }

    Framebuffer::~Framebuffer()
    {
        glDeleteFramebuffers(1, &renderer_id);
        glDeleteTextures(1, &color_attachment);
        glDeleteTextures(1, &depth_attachment);
    }

    void Framebuffer::Invalidate()
    {
        if (renderer_id)
        {
            glDeleteFramebuffers(1, &renderer_id);
            glDeleteTextures(1, &color_attachment);
            glDeleteTextures(1, &depth_attachment);
        }

        glCreateFramebuffers(1, &renderer_id);
        glBindFramebuffer(GL_FRAMEBUFFER, renderer_id);

        glCreateTextures(GL_TEXTURE_2D, 1, &color_attachment);
        glBindTexture(GL_TEXTURE_2D, color_attachment);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, specification.Width, specification.Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_attachment, 0);

        glCreateTextures(GL_TEXTURE_2D, 1, &depth_attachment);
        glBindTexture(GL_TEXTURE_2D, depth_attachment);
        glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH24_STENCIL8, specification.Width, specification.Height);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depth_attachment, 0);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            log_err("Framebuffer is incomplete!");

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void Framebuffer::Bind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, renderer_id);
        glViewport(0, 0, specification.Width, specification.Height);
    }

    void Framebuffer::Unbind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void Framebuffer::Resize(uint32_t width, uint32_t height)
    {
        if (width == 0 || height == 0 || width > s_MaxFramebufferSize || height > s_MaxFramebufferSize)
        {
            log_warn("Attempted to resize framebuffer to %d %d", width, height);
            return;
        }
        specification.Width = width;
        specification.Height = height;

        Invalidate();
    }
}