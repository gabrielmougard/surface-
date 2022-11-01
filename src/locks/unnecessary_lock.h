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
#include <cassert>

class UnnecessaryLock : public LockBase
{
public:
    explicit UnnecessaryLock(const optional_sting& name);

    bool TryAcquire() override;

    void Acquire() override;

    void Release() override;

#ifndef NDEBUG
protected:
    volatile bool is_locked = false;
#endif
};

inline UnnecessaryLock::UnnecessaryLock(const optional_sting& name)
: LockBase(name)
{
    lock_class_name = "UnnecessaryLock";
}

inline bool UnnecessaryLock::TryAcquire()
{
#ifndef NDEBUG
    assert(!is_locked);
    is_locked = true;
#endif
    return true;
}

inline void UnnecessaryLock::Acquire()
{
    TryAcquire();
}

inline void UnnecessaryLock::Release()
{
#ifndef NDEBUG
    assert(is_locked);
    is_locked = false;
#endif
}