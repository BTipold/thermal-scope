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

#ifndef _THERMAL_SCOPE_APPLICATION_H_
#define _THERMAL_SCOPE_APPLICATION_H_

#include <stdint.h>
#include <opencv2/videoio.hpp>

#include <memory>

#include "CommonDefs.h"
#include "FrameBuffer.h"
#include "PersistentValue.h"
#include "P2ProManager.h"
#include "Reticle.h"
#include "UsbControl.h"
#include "VideoOverlay.h"
#include "Webcam.h"

namespace thermal {

class ThermalScopeApplication {
public:
    ThermalScopeApplication(int32_t argc, char* argv[]);
    ~ThermalScopeApplication();

    void Init();
    void Run();

private:
    std::unique_ptr<p2pro::P2ProManager> mP2ProManager;
    hw::FrameBuffer mFrameBuffer;
    hw::Encoder mSideEncoder;
    hw::Encoder mTopEncoder;
    VideoOverlay mOverlay;
    TopMode mTopMode;
    SideMode mSideMode;

    // persistent settings
    persistent::Value<int32_t, p2pro::ColorMode> mColorSetting;
    persistent::Value<int32_t, ReticleType> mReticleSetting;
    persistent::Value<int32_t> mXOffsetSetting;
    persistent::Value<int32_t> mYOffsetSetting;
    persistent::Value<uint32_t> mZoomSetting;

    bool OnCameraData(cv::Mat& frame, bool lastFrame);
    void OnRotateSide(hw::Direction direction);
    void OnRotateTop(hw::Direction direction);
    void OnClickSide(bool level);
    void OnClickTop(bool level);
};

}

#endif // _THERMAL_SCOPE_APPLICATION_H_