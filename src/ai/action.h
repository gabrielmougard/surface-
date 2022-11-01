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

#include <string>
#include <unordered_map>

#include "world_state.h"
#include "scene/entity.h"


namespace goap {
    class Action {
    protected:
        int cost_;
        std::string name_;
        std::string target_entity_id_;

    public:
        WorldState * effects_;
        WorldState * preconditions_;

        Action(std::string name, int cost);
        Action(std::string name, int cost, surfacepp::Entity &target_entity);
        virtual ~Action() = default;

        virtual bool operableOn(const goap::WorldState& ws) const;
        virtual WorldState actOn(const WorldState& ws) const;
        virtual bool perform(surfacepp::Entity &applier, surfacepp::Entity &target) const = 0;

        virtual void setPrecondition(const char * uuid_v4 , const char * name, bool value) const{
            preconditions_->setFact(uuid_v4, name, value);
        }

        virtual void setEffect(const char * uuid_v4 , const char * name, bool value) const{
            effects_->setFact(uuid_v4, name, value);
        }

        virtual int getCost() const { return cost_; }
        virtual std::string getName() const { return name_; }
        virtual std::string getTargetId() const { return target_entity_id_; }

        virtual void log() const;
    };
}