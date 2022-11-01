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

#include "node.h"
#include "action.h"
#include "world_state.h"

#include <vector>


namespace goap {
    class Planner {
    private:
        const int START_NODE_COST = 0;
        const int START_NODE_PARENT_ID = 0;
        const char * START_NODE_NAME = "Start node";

        std::vector<Node> open_;
        std::vector<Node> closed_;
        int (*heuristicFunctionPointer)(const WorldState &node, const WorldState &goal);

        bool isClosed(const WorldState& ws) const;

        std::vector<goap::Node>::iterator getIfOpen(const WorldState& ws);

        Node& fromOpenToClose();

        void emplaceOpen(Node n);

    public:
        Planner(int (*heuristicFunction)(const WorldState &node, const WorldState &goal));

        void logOpenList() const;
        void logClosedList() const;

        std::vector<const Action *> plan(const WorldState& start, const WorldState& goal, const std::vector<const Action *> actions);
    };
}