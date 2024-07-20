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

#ifndef _P2_PRO_MANAGER_H_
#define _P2_PRO_MANAGER_H_

#include <stdint.h>
#include <libusb.h>

#include "UsbControl.h"
#include "Webcam.h"

#include <cstddef>
#include <vector>
#include <memory>

namespace thermal {
namespace p2pro {

// Possible settings that the P2 will accept.
enum class ColorMode : uint8_t {
    kPseudoWhiteHot = 1,
    kPseudoReserved = 2,
    kPseudoIronRed = 3,
    kPseudoRainbow1 = 4,
    kPseudoRainbow2 = 5,
    kPseudoRainbow3 = 6,
    kPseudoRedHot = 7,
    kPseudoHotRed = 8,
    kPseudoRainbow4 = 9,
    kPseudoRainbow5 = 10,
    kPseudoBlackHot = 11,
    kCount,
};

inline constexpr const char * ColorToString(ColorMode color) {
    switch (color) {
        case ColorMode::kPseudoWhiteHot:
            return "WHITEHOT";
        case ColorMode::kPseudoReserved:
            return "RESERVED";
        case ColorMode::kPseudoIronRed:
            return "IRONRED";
        case ColorMode::kPseudoRainbow1:
            return "RAINBOW1";
        case ColorMode::kPseudoRainbow2:
            return "RAINBOW2";
        case ColorMode::kPseudoRainbow3:
            return "RAINBOW3";
        case ColorMode::kPseudoRedHot:
            return "REDHOT";
        case ColorMode::kPseudoHotRed:
            return "HOTRED";
        case ColorMode::kPseudoRainbow4:
            return "RAINBOW4";
        case ColorMode::kPseudoRainbow5:
            return "RAINBOW5";
        case ColorMode::kPseudoBlackHot:
            return "BLACKHOT";
        default:
            return "OUT OF BOUNDS";
    }
}


// Only one driver can aquire the device at a time. This enum will keep track
// of which driver is currently holding the device.
enum class UsbMode : uint8_t {
    kNone,
    kVideo,
    kCommand,
};

inline const char * UsbModeToStr(UsbMode focus) {
    switch (focus) {
        case UsbMode::kNone:
            return "NONE";
        case UsbMode::kVideo:
            return "VIDEO";
        case UsbMode::kCommand:
            return "COMMAND";
        default:
            return "ERR";
    }
}

class P2ProManager {
public:
    P2ProManager(std::shared_ptr<Webcam> cam, std::shared_ptr<UsbControl> control);
    ~P2ProManager();

    bool SetPseudoColor(ColorMode color);
    bool SwitchUsbMode(UsbMode mode);
    bool StartVideoStream();
    bool StopVideoStream();
    bool CommandMode();
    UsbMode GetUsbMode() const;
    p2pro::ColorMode GetCurrentActiveColorMode() const;

private:
    std::shared_ptr<Webcam> mWebcam;
    std::shared_ptr<UsbControl> mUsbControl;
    UsbMode mUsbMode;
};

} // p2pro
} // thermal

#endif // _P2_PRO_MANAGER_H_
