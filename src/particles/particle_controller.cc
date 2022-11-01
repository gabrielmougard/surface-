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

#include "log.h"
#include "particles/particle_controller.h"


ParticleController::ParticleController(ParticleParameters parameters, uint32_t particles_number):
referenceParameters(parameters)
{
    renderer = new ParticleRenderer();

    for(uint32_t i = 0; i < particles_number; i++){
        particles.emplace_back(randomizeParticleParameters());
    }
    log_info("[Particle system]: %d particles created", particles.size());
}


ParticleController::~ParticleController(){
//    delete renderer;
}


void ParticleController::update(GLfloat deltaTime) {
    for (auto& iter : particles){
        iter.update(deltaTime);
        if (! iter.isActive()){
            iter.setParameters(randomizeParticleParameters());
        }
    }
}


inline ParticleParameters ParticleController::randomizeParticleParameters() {
    // set up rand values instead of reference values
    ParticleParameters randomized_params = referenceParameters;

    randomized_params.color = glm::vec4(
            // 0.7 is used to make it particles lighter
            static_cast<float>(rand()) / static_cast<float>(RAND_MAX / referenceParameters.color.x),
            static_cast<float>(rand()) / static_cast<float>(RAND_MAX / referenceParameters.color.y),
            static_cast<float>(rand()) / static_cast<float>(RAND_MAX / referenceParameters.color.z),
            referenceParameters.color.w
    );
    randomized_params.velocity = glm::vec3(
            rand() % static_cast<int>(referenceParameters.velocity.x) - referenceParameters.velocity.x / 2,
            rand() % static_cast<int>(referenceParameters.velocity.y) + referenceParameters.velocity.y / 3,
            rand() % static_cast<int>(referenceParameters.velocity.z) - referenceParameters.velocity.z / 2
    );
    randomized_params.life_length = static_cast<float>(rand()) / static_cast<float>(RAND_MAX / referenceParameters.life_length);
    randomized_params.rotation = static_cast<float>(rand()) / static_cast<float>(referenceParameters.rotation);
    return randomized_params;
}

void ParticleController::renderParticles(BaseCamera *camera, Shader * shader, GLfloat screen_scale) {
    renderer->render(camera, particles, shader, screen_scale);
}