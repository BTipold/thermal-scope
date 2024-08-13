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

#ifndef _ENCODER_H_
#define _ENCODER_H_

#include <stdint.h>

#include <vector>
#include <functional>

#include "GpioWatcher.h"

namespace thermal {
namespace hw {

/**
 * @brief Enum class to represent the direction of rotation.
 */
enum class Direction : int8_t {
    kDecrement = -1, ///< Decrement direction
    kIncrement = 1   ///< Increment direction
};

/**
 * @brief Typedef for the callback function for rotation events.
 *        This is called once per step.
 */
typedef std::function<void(Direction)> RotateCallback;

/**
 * @brief Typedef for the callback function for click events.
 *        This is called on low and high.
 */
typedef std::function<void(bool)> ClickCallback;

/**
 * @brief Class to handle rotary encoder and button press events.
 */
class Encoder {
public:
    /**
     * @brief Constructor to initialize the encoder with GPIO pins for A, B, and button.
     * @param pinA GPIO pin number for the A signal.
     * @param pinB GPIO pin number for the B signal.
     * @param btnPin GPIO pin number for the button signal.
     */
    Encoder(int32_t pinA, int32_t pinB, int32_t btnPin);
    
    /**
     * @brief Destructor.
     */
    ~Encoder();

    /**
     * @brief Set the callback function for rotation events.
     * @param callback A function to be called when a rotation event occurs.
     */
    void SetOnRotateCallback(RotateCallback callback);
    
    /**
     * @brief Set the callback function for click events.
     * @param callback A function to be called when a click event occurs.
     */
    void SetOnClickCallback(ClickCallback callback);

    /**
     * @brief Clear the callback function for rotate events.
     */
    void ClearOnRotateCallback();

    /**
     * @brief Clear the callback function for click events.
     */
    void ClearOnClickCallback();

private:
    RotateCallback mRotateCallback; ///< Callback function for rotation events.
    ClickCallback mClickCallback;   ///< Callback function for click events.
    gpio::Watcher mGpioA;           ///< GPIO watcher for the A pin.
    gpio::Watcher mGpioB;           ///< GPIO watcher for the B pin.
    gpio::Watcher mGpioBtn;         ///< GPIO watcher for the button pin.
    uint8_t mSeqA = 0;              ///< Holds the last 4 states in a bitfield
    uint8_t mSeqB = 0;              ///< Holds the last 4 states in a bitfield

    /**
     * @brief Internal method to handle rotation events.
     * @param gpio unused.
     * @param level unused.
     */
    void OnRotateEvent(int32_t, bool);
    
    /**
     * @brief Internal method to handle click events.
     * @param gpio unused.
     * @param level The level of the GPIO pin (true for high, false for low).
     */
    void OnClickEvent(int32_t, bool level);
};

} // namespace hw
} // namespace thermal

#endif // _ENCODER_H_