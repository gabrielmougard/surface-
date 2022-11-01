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


#include "planner.h"

#include <iostream>
#include <algorithm>
#include "ai/actions/ai_action_none.h"


namespace goap {

    Planner::Planner(int (*heuristicFunction)(const WorldState &node, const WorldState &goal)) :
            heuristicFunctionPointer(heuristicFunction) {}

    void Planner::emplaceOpen(Node n) {
        auto it = std::lower_bound(open_.begin(), open_.end(), n);
        open_.emplace(it, std::move(n));
    }

    Node &Planner::fromOpenToClose() {
        if (open_.empty())
            log_err("AI: attempting to pop from empty vector!");
        closed_.push_back(std::move(open_.front()));
        open_.erase(open_.begin());
        return closed_.back();
    }

    bool Planner::isClosed(const WorldState &ws) const {
        if (std::find_if(begin(closed_), end(closed_), [&](const Node &n) { return n.ws_ == ws; }) == end(closed_)) {
            return false;
        }
        return true;
    }

    std::vector<Node>::iterator Planner::getIfOpen(const WorldState &ws) {
        return std::find_if(begin(open_), end(open_), [&](const Node &n) { return n.ws_ == ws; });
    }

    void Planner::logOpenList() const {
        for (const auto &n : open_) {
            n.log();
        }
    }

    void Planner::logClosedList() const {
        for (const auto &n : closed_) {
            n.log();
        }
    }

    std::vector<const Action *>
    Planner::plan(const WorldState &start, const WorldState &goal, const std::vector<const Action *> actions) {
        if (start.meetsGoal(goal)) {
            return std::vector<const Action *>();
        }

        closed_.clear();
        open_.clear();
        open_.emplace_back(
                start,
                START_NODE_COST,
                (*heuristicFunctionPointer)(start, goal),
                START_NODE_PARENT_ID,
                new AIActionNone(START_NODE_NAME, START_NODE_COST)
        );

        // astar
        while (!open_.empty()) {
            Node current(fromOpenToClose());

            // backward reconstruction of the plan created
            if (current.ws_.meetsGoal(goal)) {
                std::vector<const Action *> plan;
                while (current.parent_id_ != START_NODE_PARENT_ID) {
                    plan.push_back(current.action_);
                    auto itr = std::find_if(closed_.begin(), closed_.end(),
                                            [&](const Node &n) { return n.id_ == current.parent_id_; });
                    current = *itr;
                }
                return plan;
            }

            for (const auto &potential_action : actions) {
                if (potential_action->operableOn(current.ws_)) {
                    WorldState outcome = potential_action->actOn(current.ws_);

                    if (isClosed(outcome)) {
                        continue;
                    }

                    auto p_outcome_node = getIfOpen(outcome);

                    if (p_outcome_node == open_.end()) {
                        emplaceOpen(
                                Node(outcome, current.g_ + potential_action->getCost(),
                                     (*heuristicFunctionPointer)(outcome, goal), current.id_, potential_action)
                        );
                    } else if (current.g_ + potential_action->getCost() < p_outcome_node->g_) {
                        open_.erase(p_outcome_node);
                        emplaceOpen(
                                Node(outcome, current.g_ + potential_action->getCost(),
                                     (*heuristicFunctionPointer)(outcome, goal), current.id_, potential_action)
                        );
                    }
                }
            }
        }
        throw std::runtime_error("A* planner could not find a path from start to goal");
    }
}