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

#include "ai/actions/ai_action_follow.h"

bool AIActionFollow::perform(surfacepp::Entity &applier, surfacepp::Entity &target) const {
    float speed = 0.5f;
    if (target.getComponent<surfacepp::UuidComponent>().uuid != target_entity_id_)
        return false;
    auto &applier_transform = applier.getComponent<surfacepp::TransformComponent>();
    auto &target_transform = target.getComponent<surfacepp::TransformComponent>();
    glm::vec3 distance = glm::vec3(target_transform.position) - glm::vec3(applier_transform.position);
    if (glm::length(distance) >= speed) {
        applier_transform.is_looking_at = true;
        applier_transform.look_at = target_transform.position;
        applier_transform.position += glm::normalize(distance) * speed;
        return false;
    }
    return true;
}