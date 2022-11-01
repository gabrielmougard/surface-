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

#include "particles/particle.h"
#include "particles/particle_renderer.h"

class BaseCamera;

class ParticleController{

public:
    ParticleController(ParticleParameters parameters, uint32_t particles_number);
    ~ParticleController();

    void update(GLfloat deltaTime);
    void renderParticles(BaseCamera *camera, Shader * shader, GLfloat screen_scale);
    int getParticlesNumber(){
        return this->particles.size();
    };

    ParticleParameters referenceParameters;
private:
    ParticleParameters randomizeParticleParameters();

    ParticleRenderer *renderer;
    std::vector<Particle> particles;
};