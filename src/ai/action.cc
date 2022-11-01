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

#include <algorithm>

#include "action.h"
#include "world_state.h"
#include "scene/components.h"


namespace goap {
    Action::Action(std::string name, int cost): name_(std::move(name)), cost_(cost) {
        this->preconditions_ = new WorldState("");
        this->effects_ = new WorldState("");
    }

    Action::Action(std::string name, int cost, surfacepp::Entity &target_entity):
    name_(std::move(name)),
    cost_(cost),
    target_entity_id_(target_entity.getComponent<surfacepp::UuidComponent>().uuid)
    {
        target_entity.addComponent<surfacepp::AITargetComponent>(name_);
        this->preconditions_ = new WorldState("");
        this->effects_ = new WorldState("");
    }

    bool Action::operableOn(const WorldState& ws) const {
        return ws.meetsGoal(*this->preconditions_);
    }

    WorldState Action::actOn(const WorldState& ws) const {
        if (! operableOn(ws)){
            log_err("AI: action '%s' can't operate on '%s'", getName().c_str(), ws.name.c_str());
        }

        WorldState tmp(ws);
        for (const auto& effect : effects_->facts) {
            tmp.setFact(effect.entity_uuid.c_str(), effect.name.c_str(), effect.value);
        }
        return tmp;
    }

    void Action::log() const {
        log_dbg("%s Preconditions:", name_.c_str());
        this->preconditions_->log();
        log_dbg("%s Effects:", name_.c_str());
        this->effects_->log();
    }
}