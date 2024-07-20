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

#ifndef _UTILS_H_
#define _UTILS_H_

#include <stdint.h>

#include <filesystem>

#include "Logger.h"

namespace thermal {
namespace utils {

/**
 * @brief Rotates an enum value by a specified number of steps within a range.
 *
 * This function takes an enum value, adds a specified number of steps (which
 * can be negative), and wraps around within the specified maximum value.
 *
 * @tparam EType The enum type.
 * @param e The enum value to rotate.
 * @param skip The number of steps to rotate the enum value by.
 * @param max The maximum number of enum values (range limit).
 * @return The rotated enum value.
 */
template<typename EType>
static EType RotateEnum(EType e, int32_t max, int32_t skip = 1) {
    if (max == 0) return e;
    int32_t num = static_cast<int32_t>(e);
    num = (num + skip) % max;
    return static_cast<EType>(num);
}

/**
 * @brief Ensures that a directory exists at the specified path.
 *
 * This function checks if a directory exists at the given path. If the directory
 * does not exist, it attempts to create it. If directory creation fails, an error
 * message is logged.
 *
 * @param path The path of the directory to ensure exists.
 */
static void EnsureDirectoryExists(const std::string& path) {
    // Ensure directory exists
    std::filesystem::path dir(path);
    if (!std::filesystem::exists(dir)) {
        if (!std::filesystem::create_directories(dir)) {
            DLOG_ERROR("Failed to create directory %s", path.c_str());
        }
    }
}

} // utils
} // thermal

#endif // _UTILS_H_