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

#include "Encoder.h"

#include "Logger.h"

namespace thermal {
namespace hw {

using std::bind;
using std::placeholders::_1;
using std::placeholders::_2;

Encoder::Encoder(int32_t pinA, int32_t pinB, int32_t btnPin)
    : mRotateCallback(nullptr)
    , mClickCallback(nullptr) 
    , mGpioA(pinA)
    , mGpioB(pinB)
    , mGpioBtn(btnPin) {
    mGpioA.RegisterOnChangeCallback(bind(&Encoder::OnRotateEvent, this, _1, _2));
    mGpioB.RegisterOnChangeCallback(bind(&Encoder::OnRotateEvent, this, _1, _2));
    mGpioBtn.RegisterOnChangeCallback(bind(&Encoder::OnClickEvent, this, _1, _2));
    return; 
}

Encoder::~Encoder() {
    return;
}

void Encoder::SetOnRotateCallback(RotateCallback callback) {
    mRotateCallback = callback;
    return;
}

void Encoder::SetOnClickCallback(ClickCallback callback) {
    mClickCallback = callback;
    return;
}

/// @brief called by the gpio::watcher automatically when the gpio level flips.
/// @param gpio - gpio number
/// @param level - high/low
void Encoder::OnRotateEvent(int32_t gpio, [[maybe_unused]] bool level) {
    
    bool currentA = mGpioA.Read();
    bool currentB = mGpioB.Read();

    if (currentA != mPrevA || currentB != mPrevB) {
        
        // Encoder rotation detected
        if (mRotateCallback) {
            if (currentA == currentB) {
                DLOG_DEBUG("Encoder increment");
                mRotateCallback(Direction::kIncrement); // Clockwise
            } else {
                DLOG_DEBUG("Encoder decrement");
                mRotateCallback(Direction::kDecrement); // Counterclockwise
            }
        }

        // Update previous state
        mPrevA = currentA;
        mPrevB = currentB;
    }
}


/// @brief called by the gpio::watcher automatically when the gpio level flips.
/// @param gpio - gpio number
/// @param level - high/low
void Encoder::OnClickEvent(int32_t gpio, bool level) {
    DLOG_DEBUG("Btn Clicked (gpio %d)", gpio);
    mClickCallback(level);
}



} // namespace hw
} // namespace thermal
