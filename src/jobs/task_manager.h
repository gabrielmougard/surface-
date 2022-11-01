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
#include <array>
#include <functional>
#include <memory>
#include <optional>
#include <type_traits>
#include "task_handle.h"
#include "task.h"
#include "locks/spin_lock.h"

class TaskWorker;

class TaskManager
{
private:
    TaskManager();

public:
    TaskManager(const TaskManager&) = delete;
    TaskManager(TaskManager&&) = delete;

    ~TaskManager();

    static TaskManager& GetInstance();

    //~ Templates magic. Hurts to understand
    template<class Function, class... Args>
    using ResultType = std::invoke_result_t<std::decay_t<Function>, std::decay_t<Args>...>;

    // Puts task to the execution queue
    template<class Function, class... Args>
    std::shared_ptr<TaskHandle<TaskManager::ResultType<Function, Args...>>> RunTask(TaskPriority priority, Function&& func, Args&&... args)
    {
        using TaskResultType = ResultType<Function, Args...>;

        auto handle = std::make_shared<TaskHandle<TaskResultType>>(std::bind(&TaskManager::WaitForTask, this, std::placeholders::_1));
        auto bound_func = std::bind(std::forward<Function>(func), std::forward<Args>(args)...);
        Task<TaskResultType>* task = new Task<TaskResultType>(bound_func, handle);
        AddTask(std::unique_ptr<TaskBase>(task), priority);
        return handle;
    }

    // Puts task to the execution queue with normal priority
    template<class Function, class... Args>
    std::shared_ptr<TaskHandle<TaskManager::ResultType<Function, Args...>>> RunTask(Function&& func, Args&&... args)
    {
        return RunTask(TaskPriority::Normal, std::forward<Function>(func), std::forward<Args>(args)...);
    }

    // Puts task to the execution queue and waits for the result
    template<class Function, class... Args>
    TaskManager::ResultType<Function, Args...> RunAndWaitForTask(TaskPriority priority, Function&& func, Args&&... args)
    {
        auto handle = RunTask(priority, std::forward<Function>(func), std::forward<Args>(args)...);
        return handle->WaitForTaskResult();
    }

    // Puts task to the execution queue with normal priority and waits for the result
    template<class Function, class... Args>
    TaskManager::ResultType<Function, Args...> RunAndWaitForTask(Function&& func, Args&&... args)
    {
        return RunAndWaitForTask(TaskPriority::Normal, std::forward<Function>(func), std::forward<Args>(args)...);
    }

protected:

    void ShutDown();

    // Blocks current thread until task with this handle will be finished (handle.HasTaskResult() is true)
    void WaitForTask(const TaskHandleBase& task_handle);

    void AddTask(std::unique_ptr<TaskBase> task, TaskPriority priority);

    bool AddFreeWorker(TaskWorker* worker);

    std::optional<std::unique_ptr<TaskBase>> GetNextTask();

private:
    struct TasksContainer
    {
        TasksContainer();

        TaskPriority priority;
        SpinLock lock;
        std::vector<std::unique_ptr<TaskBase>> tasks;
    };

    TasksContainer& GetTasksForPriority(TaskPriority priority);

    static unsigned int CalcMaxWorkersCount();

    std::array<TasksContainer, AllTaskPriorities.size()> waiting_tasks;

    // TODO: Can use a lock-free container, probably?
    SpinLock workers_lock;
    std::vector<std::unique_ptr<TaskWorker>> workers;
    std::vector<TaskWorker*> free_workers;

    unsigned int maxWorkersCount = 0;

};