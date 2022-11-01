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

#include <chrono>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <utility>
#include <vector>

#include "lock_base.h"
#include "scoped_lock.h"

namespace testing
{
    // Class to test, if behaviour is incorrect without locking
    class NotLock : public LockBase
    {
    public:
        explicit NotLock(const std::string& name) : LockBase(name)
        {
            lock_class_name = "NotLock";
        }
        bool TryAcquire() override { return true; }
        void Acquire() override {};
        void Release() override {};
    };

    #pragma region Task base

    template<class LockClass>
    class TestTaskBase
    {
    public:
        void RunTask();

    protected:
        explicit TestTaskBase(const std::string& task_name)
        : lock(task_name + " lock")
        {}

        virtual std::vector<std::thread> GetTaskThreads() = 0;
        virtual void OnTaskFinished() = 0;

        LockClass lock;
    };

    template<class LockClass>
    inline void TestTaskBase<LockClass>::RunTask()
    {
        std::cout << "Starting test with a " << lock << std::endl;

        std::vector<std::thread> task_threads = GetTaskThreads();
        for(std::thread& thread : task_threads)
        {
            thread.join();
        }

        OnTaskFinished();
        std::cout << "Finished test" << std::endl;
    }

    #pragma endregion

    #pragma region Print task

    template<class LockClass>
    class PrintTestTask final : public TestTaskBase<LockClass>
    {
    public:
        PrintTestTask(const std::string& task_name, const std::vector<std::string>& values_to_print, bool lock_double = false);

    private:
        std::vector<std::thread> GetTaskThreads() override;
        void OnTaskFinished() override;

        std::vector<std::string> values;
        std::stringstream result_stream;
        const bool lock_double;
    };

    template<class LockClass>
    inline PrintTestTask<LockClass>::PrintTestTask(const std::string& task_name, const std::vector<std::string>& values_to_print, const bool lock_double)
    : TestTaskBase<LockClass>(task_name), lock_double(lock_double)
    {
        values = values_to_print;
    }

    template<class LockClass>
    inline std::vector<std::thread> PrintTestTask<LockClass>::GetTaskThreads()
    {
        std::vector<std::thread> task_threads;

        for(const std::string& value : values)
        {
            std::thread task_thread([&value, this]()
            {
                ScopedLock line_lock(this->lock);

                for(const char& letter : value)
                {
                    if(lock_double)
                        this->lock.Acquire();

                    result_stream << letter;
                    std::this_thread::sleep_for(std::chrono::milliseconds(1));

                    if(lock_double)
                        this->lock.Release();
                }
            });

            task_threads.push_back(std::move(task_thread));
        }

        return task_threads;
    }

    template<class LockClass>
    inline void PrintTestTask<LockClass>::OnTaskFinished()
    {
        std::cout << "Result:\n" << result_stream.str() << std::endl;
    }

    #pragma endregion
}