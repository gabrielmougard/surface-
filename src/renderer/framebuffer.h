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

#include <cstdint>
#include <glad/glad.h>

namespace surfacepp {
    struct FramebufferSpecification
    {
        uint32_t Width = 0, Height = 0;
        uint32_t Samples = 1;

        bool SwapChainTarget = false;
    };


    class Framebuffer
    {
    public:
        Framebuffer(const FramebufferSpecification& spec);
        virtual ~Framebuffer();

        void Invalidate();

        virtual void Bind();
        virtual void Unbind();

        virtual void Resize(uint32_t width, uint32_t height);

        virtual uint32_t GetColorAttachmentRendererID() const { return color_attachment; }

        virtual const FramebufferSpecification& GetSpecification() const { return specification; }
    private:
        uint32_t renderer_id = 0;
        GLuint color_attachment;
        GLuint depth_attachment;
        FramebufferSpecification specification;
    };
}