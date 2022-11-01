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

#include <array>
#include <cassert>

enum class TaskPriority
{
    VeryHigh,
    High,
    Normal,
    Low,
    VeryLow
};

constexpr static auto AllTaskPriorities = std::array
{
    TaskPriority::VeryHigh,
    TaskPriority::High,
    TaskPriority::Normal,
    TaskPriority::Low,
    TaskPriority::VeryLow,
};

constexpr static const char* GetPriorityName(TaskPriority priority)
{
    switch (priority)
    {
        case TaskPriority::VeryHigh:
            return "VeryHigh";
        case TaskPriority::High:
            return "High";
        case TaskPriority::Normal:
            return "Normal";
        case TaskPriority::Low:
            return "Low";
        case TaskPriority::VeryLow:
            return "VeryLow";
        default:
            assert(false);
            return "Unknown";
    }
}

// Abstract templateless task class
class TaskBase
{
public:
    virtual void RunTask() = 0;
};