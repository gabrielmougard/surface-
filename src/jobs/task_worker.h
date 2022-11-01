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

#include <atomic>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <string>

class TaskBase;

class TaskWorker
{
public:
    TaskWorker();

    using get_task_func = std::function<std::optional<std::unique_ptr<TaskBase>>()>;
    using release_func = std::function<bool(TaskWorker *)>;

    void StartWorker(std::unique_ptr<TaskBase> &&first_task, const get_task_func &get_next_task,
                                  const release_func &release_worker);

    void AssignTask(std::unique_ptr<TaskBase> &&task);

    void MarkStopped();

    bool WaitForStop();

    const std::string& GetName() const;

private:
    std::promise<std::unique_ptr<TaskBase>> task_promise;

    std::atomic_bool should_work;
    std::atomic_bool stopped;

    std::string name;
};