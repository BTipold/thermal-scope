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

void Encoder::ClearOnRotateCallback() {
    mRotateCallback = nullptr;
    return;
}

void Encoder::ClearOnClickCallback() {
    mClickCallback = nullptr;
    return;
}

// from https://www.allaboutcircuits.com/projects/how-to-use-a-rotary-encoder-in-a-mcu-based-project/
void Encoder::OnRotateEvent([[maybe_unused]] int32_t, [[maybe_unused]] bool) {

    // Read A and B signals
    bool currentA = mGpioA.Read();
    bool currentB = mGpioB.Read();
    
    // Record the A and B signals in seperate sequences
    mSeqA <<= 1;
    mSeqA |= currentA;
    
    mSeqB <<= 1;
    mSeqB |= currentB;
    
    // Mask the MSB four bits
    mSeqA &= 0b00001111;
    mSeqB &= 0b00001111;

    // Compare the recorded sequence with the expected sequence
    if ((mSeqA == 0b00001001) && (mSeqB == 0b00000011)) {
        DLOG_DEBUG("INCREMENT");
        mRotateCallback(Direction::kIncrement);
    }
    
    if ((mSeqA == 0b00000011) && (mSeqB == 0b00001001)) {
        DLOG_DEBUG("DECREMENT");
        mRotateCallback(Direction::kDecrement);
    }
}

/// @brief called by the gpio::watcher automatically when the gpio level flips.
/// @param gpio - gpio number
/// @param level - high/low
void Encoder::OnClickEvent([[maybe_unused]] int32_t, bool level) {
    mClickCallback(level);
}



} // namespace hw
} // namespace thermal
