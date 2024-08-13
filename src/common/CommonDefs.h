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

#ifndef _COMMON_DEFS_H_
#define _COMMON_DEFS_H_

#include <stdint.h>

namespace thermal {

enum class TopMode : int8_t {
    kNone = 0,
    kXOffset = 1,
    kPickReticle = 2,
    kPickColor = 3,
    kCount,
};

inline constexpr const char * TopModeToString(TopMode color) {
    switch (color) {
        case TopMode::kNone:
            return "NONE";
        case TopMode::kXOffset:
            return "X-OFFSET";
        case TopMode::kPickReticle:
            return "RETICLE";
        case TopMode::kPickColor:
            return "COLOR";
        default:
            return "UNKNOWN";
    }
}

enum class SideMode : int8_t {
    kNone = 0,
    kYOffset = 1,
    kZoom = 2,
    kCount,
};

inline constexpr const char * SideModeToString(SideMode color) {
    switch (color) {
        case SideMode::kNone:
            return "NONE";
        case SideMode::kYOffset:
            return "Y-OFFSET";
        case SideMode::kZoom:
            return "ZOOM";
        default:
            return "UNKNOWN";
    }
}

} // thermal

#endif // _COMMON_DEFS_H_