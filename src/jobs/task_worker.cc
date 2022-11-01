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

#include "task_worker.h"

#include <sstream>
#include "task_base.h"
#include "log.h"

TaskWorker::TaskWorker()
: should_work(true), stopped(false)
{}

void TaskWorker::StartWorker(std::unique_ptr<TaskBase> &&first_task, const TaskWorker::get_task_func& get_next_task,
                             const TaskWorker::release_func& release_worker)
{
    log_info("Staring worker thread");

    {
        std::stringstream name_stream;
        name_stream << std::this_thread::get_id();
        name = std::move(name_stream.str());
    }

    first_task->RunTask();
    first_task.reset(); // Deleting task explicitly since there is an endless cycle below and first_task won't be destroyed until the end of the program

    while (should_work.load(std::memory_order_acquire))
    {
        // Try to get a new task immediately
        {
            auto opt_task = get_next_task();
            if (opt_task)
            {
                auto task = std::move(opt_task.value());
                task->RunTask();
                continue;
            }
        }

        // Wait for the next task
        {
            if(!release_worker(this))
                continue;

            auto task_future = task_promise.get_future();
            auto task = task_future.get();
            // Reset promise to be able to reuse it
            task_promise = std::promise<std::unique_ptr<TaskBase>>();
            if(task)
                task->RunTask();
        }
    }

    stopped.store(true, std::memory_order_release);
}

void TaskWorker::AssignTask(std::unique_ptr<TaskBase>&& task)
{
    std::atomic_thread_fence(std::memory_order_release);
    task_promise.set_value(std::move(task));
}

void TaskWorker::MarkStopped()
{
    should_work.store(false, std::memory_order_release);
}

bool TaskWorker::WaitForStop()
{
    // TODO: It's better to add a timeout
    while(!stopped.load(std::memory_order_acquire))
        std::this_thread::yield();
    return true;
}

const std::string &TaskWorker::GetName() const
{
    return name;
}