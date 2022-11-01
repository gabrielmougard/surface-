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

#include "lock_base.h"

#include <atomic>

class SpinLock : public LockBase
{
public:
    explicit SpinLock(const optional_sting& name = optional_sting());

    bool TryAcquire() override;
    void Acquire() override;
    void Release() override;

protected:
    std::atomic_flag is_locked;
};

inline SpinLock::SpinLock(const optional_sting& name)
: LockBase(name)
{
    lock_class_name = "SpinLock";
    is_locked.clear();
}

inline bool SpinLock::TryAcquire()
{
    const bool is_already_locked = is_locked.test_and_set(std::memory_order_acquire);
    return !is_already_locked;
}

inline void SpinLock::Release()
{
    is_locked.clear(std::memory_order_release);
}