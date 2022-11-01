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

#include "node.h"


namespace goap {
    int Node::last_id_ = 0;

    Node::Node(const WorldState state, int g, int h, int parent_id, const Action* action) :
            ws_(state), g_(g), h_(h), parent_id_(parent_id), action_(action) {
        id_ = ++last_id_;
    }

    bool operator<(const Node& lhs, const Node& rhs) {
        return lhs.f() < rhs.f();
    }
}