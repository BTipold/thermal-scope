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

#ifndef _USB_CONTROL_H_
#define _USB_CONTROL_H_

#include <stdint.h>
#include <libusb.h>

#include <cstddef>
#include <vector>

namespace thermal {
namespace p2pro {

enum class CmdCode_t : uint32_t {
    kSysResetToRom = 0x0805,
    kSpiTransfer = 0x8201,
    kGetDeviceInfo = 0x8405,
    kPseudoColor = 0x8409,
    kShutterVtemp = 0x840c,
    kPropTpdParams = 0x8514,
    kCurVtemp = 0x8b0d,
    kPreviewStart = 0xc10f,
    kPreviewStop = 0x020f,
    kY16PreviewStart = 0x010a,
    kY16PreviewStop = 0x020a,
};

enum class CmdDir_t : uint32_t {
    kGet = 0x0000,
    kSet = 0x4000,
};

class UsbControl {
public:
    UsbControl();
    ~UsbControl();
    bool Acquire();
    bool Release();
    bool SendCommand(uint16_t cmd, uint32_t cmd_param = 0, std::vector<uint8_t> data = {0});
    bool IsAcquired();

private:
    bool mOpen;
    libusb_device_handle* mHandle;
    libusb_context* mContext;

    bool CheckIfDeviceIsReady();
    bool BlockUntilDeviceIsReady(int timeout = 5);
};

} // p2pro
} // thermal

#endif // _P2_PRO_USB_CONTROL_H_
