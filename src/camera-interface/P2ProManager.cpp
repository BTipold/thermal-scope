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

#include "P2ProManager.h"

#include "UsbControl.h"
#include "Webcam.h"

#include "Logger.h"

namespace thermal {
namespace p2pro {

P2ProManager::P2ProManager(std::shared_ptr<Webcam> cam, std::shared_ptr<UsbControl> control)
    : mWebcam(cam)
    , mUsbControl(control)
    , mUsbMode(UsbMode::kNone) {
    return;
}

P2ProManager::~P2ProManager() {
    return;
}

bool P2ProManager::StartVideoStream() {
    if (!SwitchUsbMode(UsbMode::kVideo)) {
       DLOG_ERROR("Err: failed to set usb focus to video");
       return false;
    }
    return true;
}

bool P2ProManager::CommandMode() {
    if (!SwitchUsbMode(UsbMode::kCommand)) {
        DLOG_ERROR("Err: failed to set usb focus to command");
        return false;
    }
    return true;
}

bool P2ProManager::StopVideoStream() {
    return mWebcam->Stop();
}

UsbMode P2ProManager::GetUsbMode() const {
    return mUsbMode;
}

bool P2ProManager::SetPseudoColor(ColorMode color) {
    DLOG_DEBUG("setting pseudo-color to %s", ColorToString(color));

    bool status = false;
    UsbMode oldMode = mUsbMode;
    if (oldMode != UsbMode::kCommand) {
        status = SwitchUsbMode(UsbMode::kCommand);
    }

    if (status == true) {
        std::vector<uint8_t> data = { static_cast<uint8_t>(color) };
        uint16_t command = (static_cast<uint16_t>(CmdCode_t::kPseudoColor) | static_cast<uint16_t>(CmdDir_t::kSet));
        status = mUsbControl->SendCommand(command, 0, data);
        if (!status) {
            DLOG_ERROR("Err: failed to send pseudo color cmd");
        }
    }

    if (oldMode == UsbMode::kVideo) {
        status = SwitchUsbMode(UsbMode::kVideo);
    }
    return status;
}

bool P2ProManager::SwitchUsbMode(UsbMode newMode) {
    bool status = true;
    UsbMode prevMode = mUsbMode;
    DLOG_DEBUG("Switching USB mode: %s -> %s", UsbModeToStr(prevMode),  UsbModeToStr(newMode));

    if (prevMode == newMode) {
        DLOG_INFO("usb mode is the same, skipping");
        return true;
    }

    switch (newMode) {
        case UsbMode::kCommand: {
            if (mWebcam->GetState() == WebcamState::kRunning) {
                mWebcam->Stop();
            }
            if (mWebcam->GetState() == WebcamState::kConnectedAndStopped) {
                mWebcam->ReleaseCamera();
            }

            status = mUsbControl->Acquire();
        } break;
        
        case UsbMode::kVideo: {
            if (mUsbControl->IsAcquired()) {
                mUsbControl->Release();
            }

            status = mWebcam->Open();
            status &= mWebcam->Start();
        } break;
        
        case UsbMode::kNone:
        default:
            DLOG_ERROR("unexpected mode %d", static_cast<int32_t>(newMode));
            break;
    }

    mUsbMode = newMode;
    return status;
}

} // p2pro
} // thermal
