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

#include "task_manager.h"

#include <algorithm>
#include <cassert>
#include <thread>
#include "task_worker.h"
#include "locks/scoped_lock.h"
#include "log.h"

TaskManager::TaskManager()
    : workers_lock("WorkersLock"), maxWorkersCount(CalcMaxWorkersCount())
{
    assert(maxWorkersCount > 0);
    workers.reserve(maxWorkersCount);
    free_workers.reserve(maxWorkersCount);

    for(unsigned i = 0; i < AllTaskPriorities.size(); ++i)
    {
        const auto priority = AllTaskPriorities[i];
        waiting_tasks[i].priority = priority;
        waiting_tasks[i].lock.UpdateName(std::string(GetPriorityName(priority)) + "PriorityTasksLock");
    }
}

TaskManager::~TaskManager()
{
    ShutDown();
}

TaskManager &TaskManager::GetInstance()
{
    static TaskManager manager;
    return manager;
}

void TaskManager::ShutDown()
{
    ScopedLock scopeWorkersLock(workers_lock);

    for(auto& worker : workers)
    {
        worker->MarkStopped();
        // If worker is waiting for a task, assign null task
        auto free_worker = std::find(free_workers.begin(), free_workers.end(), worker.get());
        if(free_worker != free_workers.end())
        {
            free_workers.erase(free_worker);
            worker->AssignTask(nullptr);
        }

        if(!worker->WaitForStop())
        {
            log_err("Worker %s refusing to shut down", worker->GetName().c_str());
        }
    }

    free_workers.clear();
    workers.clear();
}

void TaskManager::WaitForTask(const TaskHandleBase &task_handle)
{
    while (!task_handle.HasTaskResult())
    {
        auto next_task = GetNextTask();
        if(next_task.has_value())
            next_task.value()->RunTask();
        else
            std::this_thread::yield();
    }
}

void TaskManager::AddTask(std::unique_ptr<TaskBase> task, TaskPriority priority)
{
    // Try to assign task to a worker immediately
    {
        ScopedLock scopeWorkersLock(workers_lock);

        // Instantly assign task, if there is any free workers
        if (!free_workers.empty())
        {
            auto worker = free_workers.back();
            free_workers.pop_back();
            worker->AssignTask(std::move(task));
            return;
        }

        // Create new worker and assign task to it, if we can
        if (workers.size() < maxWorkersCount)
        {
            auto *newWorker = new TaskWorker();
            workers.emplace_back(newWorker);

            auto workerThread = std::thread(&TaskWorker::StartWorker, newWorker, std::move(task),
                                            [this] { return GetNextTask(); },
                                            [this](TaskWorker *worker) { return AddFreeWorker(worker); });
            workerThread.detach();

            return;
        }
    }

    // Put a task in the queue, if no worker can run it now
    {
        auto& container = GetTasksForPriority(priority);
        ScopedLock scopeTasksLock(container.lock);
        container.tasks.push_back(std::move(task));
    }
}

bool TaskManager::AddFreeWorker(TaskWorker *worker)
{
    if(!workers_lock.TryAcquire())
        return false;
    ScopedLock scopeWorkersLock(workers_lock, true);

    free_workers.push_back(worker);
    return true;
}

std::optional<std::unique_ptr<TaskBase>> TaskManager::GetNextTask()
{
    for(auto& tasks : waiting_tasks)
    {
        if (!tasks.lock.TryAcquire())
            continue;

        ScopedLock scopeTasksLock(tasks.lock, true);
        if(tasks.tasks.empty())
            continue;

        auto result = std::move(tasks.tasks.back());
        tasks.tasks.pop_back();
        return result;
    }

    return {};
}

TaskManager::TasksContainer &TaskManager::GetTasksForPriority(TaskPriority priority)
{
    for(auto& tasks : waiting_tasks)
    {
        if(tasks.priority == priority)
            return tasks;
    }

    assert(false); // Shouldn't hit that
    return waiting_tasks[0];
}

unsigned int TaskManager::CalcMaxWorkersCount()
{
    const auto result = std::thread::hardware_concurrency();
    if (result == 0)
        return 4; // 4 threads by default
    return result;
}

TaskManager::TasksContainer::TasksContainer()
: lock("TasksLock"), priority(TaskPriority::Normal)
{}