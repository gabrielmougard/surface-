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

#include "ai/action.h"
#include "scene/components.h"
#include "glm/glm.hpp"


class AIActionFollow : public goap::Action {
public:
    AIActionFollow(std::string name, int cost) : goap::Action(std::move(name), cost) {};

    AIActionFollow(std::string name, int cost, surfacepp::Entity &target_entity) :
            goap::Action(std::move(name), cost, target_entity) {
        setPrecondition(target_entity.getComponent<surfacepp::UuidComponent>().uuid.c_str(), "Dead", false);
        setEffect(target_entity.getComponent<surfacepp::UuidComponent>().uuid.c_str(), "Dead", true);
    };

    bool perform(surfacepp::Entity &applier, surfacepp::Entity &target) const override;
};