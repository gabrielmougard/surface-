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

#include <entt/entt.hpp>

#include "log.h"
#include "scene/scene.h"

#include <iostream>

namespace surfacepp {
    class Entity {
    public:
        Entity() = default;
        Entity(entt::entity handle, Scene* scene);
        Entity(const Entity& other) = default;
        ~Entity() = default;

        template<typename T, typename... Args>
        T& addComponent(Args&&... args){
            if (hasComponent<T>())
                log_err("%s already exist in entity %d, replacing", typeid(T).name(), entityHandle_);

            T& component = scene_->registry.emplace_or_replace<T>(entityHandle_, std::forward<Args>(args)...);
            return component;
        }

        template<typename T>
        T& getComponent(){
            return scene_->registry.get<T>(entityHandle_);
        }

        template<typename T>
        bool hasComponent()
        {
            return scene_->registry.has<T>(entityHandle_);
        }

        template<typename T>
        void removeComponent()
        {
            if (!hasComponent<T>())
                log_err("%s does not exist %s in entity %d", typeid(T).name(), entityHandle_);
            scene_->registry.remove_if_exists<T>(entityHandle_);
        }

        bool operator==(const Entity& other) const
        {
            return entityHandle_ == other.entityHandle_ && scene_ == other.scene_;
        }

        bool operator!=(const Entity& other) const
        {
            return !(*this == other);
        }

        uint64_t getHandle (){
            return (uint64_t)this->entityHandle_;
        }
    private:
        entt::entity entityHandle_{0};
        Scene* scene_ = nullptr;
    };
}