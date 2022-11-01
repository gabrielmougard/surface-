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

#include "ai/actions/ai_action_none.h"

bool AIActionNone::perform(surfacepp::Entity &applier, surfacepp::Entity &target) const {
    if (target.getComponent<surfacepp::UuidComponent>().uuid != target_entity_id_) {
        return false;
    }
    log_err("AI: Action: perform function is not defined");
    return false;
}
