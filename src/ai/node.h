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

#include "action.h"
#include "world_state.h"


namespace goap {
    struct Node {
        static int last_id_;

        WorldState ws_;
        int id_;
        int parent_id_;
        int g_;              // The A* cost from 'start' to 'here'
        int h_;              // The estimated remaining cost to 'goal' form 'here'
        const Action* action_;     // The action that got us here

        Node(const WorldState state, int g, int h, int parent_id, const Action* action);

        int f() const {
            return g_ + h_;
        }

        void log() const {
            log_dbg("Node { id: %d, parent: %d F: %d G: %d H: %d }", id_, parent_id_, f(), g_, h_);
        }
    };

    bool operator<(const Node& lhs, const Node& rhs);
}