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

#include "UsbControl.h"

#include <libusb.h>

#include <cstring>
#include <vector>
#include <chrono>
#include <thread>

#include "Logger.h"

namespace thermal {
namespace p2pro {

constexpr uint32_t kVendorId = 0x0BDA;
constexpr uint32_t kProductId = 0x5830;

UsbControl::UsbControl()
    : mHandle(nullptr)
    , mContext(nullptr) 
    , mOpen(false) {
    return;
}

UsbControl::~UsbControl() {
    DLOG_DEBUG("shutting down");
    
    // Release the interface
    if (mHandle != nullptr && mOpen) {
        libusb_release_interface(mHandle, 0);
    }

    // Close the USB device
    if (mHandle != nullptr) {
        libusb_close(mHandle);
        mHandle = nullptr;
    }

    // Deinitialize libusb
    if (mContext != nullptr) {
        libusb_exit(mContext);
        mContext = nullptr;
    }
    return;
}

bool UsbControl::Acquire() {
    bool status = true;
    int32_t res = libusb_init(&mContext);
    if (res < 0) {
        DLOG_ERROR("failed to initialize usb (err=%d)", res);
        return false;
    }

    mHandle = libusb_open_device_with_vid_pid(mContext, kVendorId, kProductId);
    if (mHandle == NULL) {
        DLOG_ERROR("failed to open usb");
        return false;
    }

    res = libusb_detach_kernel_driver(mHandle, 0);
    if (res < 0) {
        DLOG_WARN("Failed to detach kernel driver (err=%d)", res);
        status = false;
    }

    res = libusb_claim_interface(mHandle, 0);
    if (res < 0) {
        DLOG_ERROR("Failed to claim interface (err=%d)", res);
        return false;
    }

    DLOG_INFO("Acquired USB Control");
    mOpen = true;
    return status;
}

bool UsbControl::Release() {
    DLOG_DEBUG("Releasing USB control");

    // Release the interface
    if (mHandle == nullptr) {
        DLOG_ERROR("handle is nullptr");
        mOpen = false;
        return true;
    }

    bool status = true;
    int32_t res = libusb_release_interface(mHandle, 0);
    if (res < 0) {
        DLOG_ERROR("Failed to release interface (err=%d)", res);
        status = false;
    }

    res = libusb_attach_kernel_driver(mHandle, 0);
    if (res < 0) {
        DLOG_ERROR("Failed to attach kernel driver (err=%d)", res);
        status = false;
    }

    if (mHandle != nullptr) {
        libusb_close(mHandle);
        mHandle = nullptr;
    }

    mOpen = false;
    DLOG_INFO("Released USB Control");
    return status;
}

bool UsbControl::SendCommand(uint16_t cmd, uint32_t cmd_param, std::vector<uint8_t> data) {
    DLOG_INFO("Sending USB command");
    
    if (data.empty()) {
        data.push_back(0);
    }

    int dataLen = data.size();
    cmd_param = __builtin_bswap32(cmd_param);

    if (dataLen == 1 && data[0] == 0) {
        // Send 8-byte command
        std::vector<uint8_t> d(8);
        std::memcpy(d.data(), &cmd, 2);
        std::memcpy(d.data() + 2, &cmd_param, 4);

        libusb_control_transfer(mHandle, 0x41, 0x45, 0x78, 0x1d00, d.data(), d.size(), 1000);
        BlockUntilDeviceIsReady();
        return true;
    }

    const int outer_chunk_size = 0x100;
    const int inner_chunk_size = 0x40;

    for (int i = 0; i < dataLen; i += outer_chunk_size) {
        std::vector<uint8_t> outer_chunk(data.begin() + i, data.begin() + std::min(i + outer_chunk_size, dataLen));
        
        // Initial camera command
        std::vector<uint8_t> initial_data(8);
        std::memcpy(initial_data.data(), &cmd, 2);
        uint32_t param = cmd_param + i;
        std::memcpy(initial_data.data() + 2, &param, 4);
        uint16_t chunk_size = outer_chunk.size();
        std::memcpy(initial_data.data() + 6, &chunk_size, 2);

        libusb_control_transfer(mHandle, 0x41, 0x45, 0x78, 0x9d00, initial_data.data(), initial_data.size(), 1000);
        BlockUntilDeviceIsReady();

        // Sending inner chunks
        for (int j = 0; j < outer_chunk.size(); j += inner_chunk_size) {
            std::vector<uint8_t> inner_chunk(outer_chunk.begin() + j, outer_chunk.begin() + std::min(j + inner_chunk_size, (int)outer_chunk.size()));
            int to_send = outer_chunk.size() - j;

            if (to_send <= 8) {
                libusb_control_transfer(mHandle, 0x41, 0x45, 0x78, 0x1d08 + j, inner_chunk.data(), inner_chunk.size(), 1000);
                BlockUntilDeviceIsReady();
            } else if (to_send <= 64) {
                libusb_control_transfer(mHandle, 0x41, 0x45, 0x78, 0x9d08 + j, inner_chunk.data(), inner_chunk.size() - 8, 1000);
                libusb_control_transfer(mHandle, 0x41, 0x45, 0x78, 0x1d08 + j + to_send - 8, inner_chunk.data() + inner_chunk.size() - 8, 8, 1000);
                BlockUntilDeviceIsReady();
            } else {
                libusb_control_transfer(mHandle, 0x41, 0x45, 0x78, 0x9d08 + j, inner_chunk.data(), inner_chunk.size(), 1000);
            }
        }
    }
    
    return true;
}

bool UsbControl::IsAcquired() {
    return mOpen;
}


bool UsbControl::CheckIfDeviceIsReady() {
    uint8_t ret[1];
    int transferred = libusb_control_transfer(mHandle, 0xC1, 0x44, 0x78, 0x200, ret, sizeof(ret), 1000);
    if (transferred < 0) {
        throw std::runtime_error("Control transfer failed");
    }
    if ((ret[0] & 1) == 0 && (ret[0] & 2) == 0) {
        return true;
    }
    if ((ret[0] & 0xFC) != 0) {
        //DLOG_WARN("vdcmd status error: %s", std::to_string(ret[0]).c_str());
    }
    return false;
}

bool UsbControl::BlockUntilDeviceIsReady(int timeout) {
    auto start = std::chrono::steady_clock::now();
    while (true) {
        if (CheckIfDeviceIsReady()) {
            return true;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        auto now = std::chrono::steady_clock::now();
        if (std::chrono::duration_cast<std::chrono::seconds>(now - start).count() > timeout) {
            return false;
        }
    }
}

} // p2pro
} // thermal
