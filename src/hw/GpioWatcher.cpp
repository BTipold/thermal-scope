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

#include "GpioWatcher.h"

#include <unordered_map>
#include <functional>

#include "Logger.h"

namespace thermal {
namespace gpio {

constexpr const int32_t kGpioChip0 = 0; 

static std::unordered_map<int32_t, std::vector<gpio::Callback>> sCallbackMap;

void DelegateCallback(int32_t numEvents, lgGpioAlert_p alert, void* userData) {

    // handle each event
    for (int32_t i = 0; i < numEvents; i++) {

        // gpio report contains details about the pin
        lgGpioReport_t gpioInfo = alert[i].report;
        if (sCallbackMap.contains(gpioInfo.gpio)) {

            // handle each callback
            for (auto cb : sCallbackMap.at(gpioInfo.gpio)) {
                cb(gpioInfo.gpio, (gpioInfo.level == /* HIGH */ 1));
            }
        }
    }
    return;
}

Watcher::Watcher(int32_t gpioNumber) 
    : mGpioDeviceNumber(gpioNumber) {

    // Static handle should be initialized by the first instance.
    // If this is the first instance, then it needs to be opened.
    if (sHandle < 0) {

        // Open GPIO chip 0. This is what it is on my RPI Zero 2W.
        sHandle = ::lgGpiochipOpen(kGpioChip0);
        if (sHandle < 0) {
            DLOG_ERROR("Failed to open GPIO /dev/gpiochip0 (err %d)", sHandle);
            return;
        } else {
            DLOG_INFO("successfully opened /dev/gpiochip0");
        }
    }

    // Set the GPIO pin as an input
    int32_t status = ::lgGpioClaimInput(sHandle, LG_SET_PULL_UP, gpioNumber);
    if (status == 0) {
        DLOG_DEBUG("claimed gpio%d", gpioNumber);
    } else {
        DLOG_ERROR("Failed to set GPIO %d as input (err %d)", gpioNumber, status);
        return;
    }

    //status = ::lgGpioSet

    // Set up the alert function for the GPIO pin
    status = ::lgGpioSetAlertsFunc(sHandle, gpioNumber, &DelegateCallback, nullptr);
    if (status == 0) {
        DLOG_DEBUG("Successfully registered a callback on gpio%d", gpioNumber);
    } else {
        DLOG_ERROR("Failed to set alert function for GPIO %d (err %d)", gpioNumber, status);
    }
    return;
}

Watcher::~Watcher() {
    DLOG_DEBUG("");
    if (sHandle > 0) {
        ::lgGpiochipClose(sHandle);
    }

    if (sCallbackMap.contains(mGpioDeviceNumber)) {
        sCallbackMap.erase(mGpioDeviceNumber);
    }
    return;
}

void Watcher::RegisterOnChangeCallback(Callback callback) {
    sCallbackMap[mGpioDeviceNumber].push_back(callback);
    return;
}

void Watcher::UnregisterOnChangeCallback(Callback callback) {
    auto vec = sCallbackMap[mGpioDeviceNumber];
    auto it = std::remove_if(vec.begin(), vec.end(), [&callback](const Callback& cb) {
            return cb.target<void(int32_t, bool)>() == callback.target<void(int32_t, bool)>();
        });

    if (it != vec.end()) {
        vec.erase(it, vec.end());
    }
}

constexpr const int32_t Watcher::Gpio() const {
    return mGpioDeviceNumber;
}

bool Watcher::Read() const {
    int32_t value = ::lgGpioRead(sHandle, mGpioDeviceNumber);
    if (value < 0) {
        DLOG_ERROR("failed to read the gpio%d (err %d)", mGpioDeviceNumber, value);
    }

    return (value == /* HIGH */ 1);
}

} // namespace gpio
} // namespace thermal
