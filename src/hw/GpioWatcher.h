/*
 * Copyright 2024 Brian Tipold
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _GPIO_WATCHER_H_
#define _GPIO_WATCHER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "lgpio.h"

#ifdef __cplusplus
}
#endif

#include <stdint.h>

#include <vector>
#include <functional>

namespace thermal {
namespace gpio {

typedef std::function<void(int32_t, bool)> Callback;

class Watcher {
public:
    Watcher(int32_t gpioNumber);
    ~Watcher();

    void RegisterOnChangeCallback(Callback callback);
    void UnregisterOnChangeCallback(Callback callback);
    constexpr const int32_t Gpio() const;
    bool Read() const;

private:
    inline static int32_t sHandle = -1;
    const int32_t mGpioDeviceNumber;
};

} // namespace gpio
} // namespace thermal

#endif // _GPIO_WATCHER_H_
