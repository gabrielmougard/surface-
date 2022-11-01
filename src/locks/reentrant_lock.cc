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

#include "reentrant_lock.h"

#include <cassert>

bool ReentrantLock::TryAcquire()
{
    int expected_count = 0;
    if(lock_count.compare_exchange_weak(expected_count, 1, std::memory_order_acquire))
    {
        assert(owner_thread_id == thread_id());
        owner_thread_id = std::this_thread::get_id();
        return true;
    }

    if(owner_thread_id == std::this_thread::get_id())
    {
        assert(lock_count > 0);
        lock_count.fetch_add(1, std::memory_order_relaxed);
        return true;
    }

    return false;
}

void ReentrantLock::Release()
{
    assert(owner_thread_id == std::this_thread::get_id());
    assert(lock_count > 0);

    if(lock_count > 1)
    {
        lock_count.fetch_add(-1, std::memory_order_relaxed);
        return;
    }

    owner_thread_id = thread_id();
    lock_count.store(0, std::memory_order_release);
}