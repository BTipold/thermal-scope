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

#ifndef _VIDEO_OVERLAY_H_
#define _VIDEO_OVERLAY_H_

#include <stdint.h>
#include <opencv2/videoio.hpp>
#include <unordered_map>

#include "CommonDefs.h"
#include "Encoder.h"
#include "Reticle.h"
#include "P2ProManager.h"

namespace thermal {

class VideoOverlay {
public:
    VideoOverlay();
    ~VideoOverlay();

    // Method to overlay the reticle on a given frame
    void Overlay(cv::Mat& frame) const;
    void SetOffset(int32_t x, int32_t y);
    void SetX(int32_t x);
    void SetY(int32_t y);
    void SetZoom(int32_t level);
    void SetReticleType(ReticleType reticleType);
    void SetColorMode(p2pro::ColorMode pseudocolor);
    void SetTopMenuMode(TopMode mode);
    void SetSideMenuMode(SideMode mode);

    void Redraw();

private:
    Reticle mReticle;
    cv::Mat mFinalOverlay;
    std::unordered_map<TopMode, std::string> mTopMsg;
    std::unordered_map<SideMode, std::string> mSideMsg;

    TopMode mTopMode;
    SideMode mSideMode;

    bool DrawTextCentreAligned(cv::Mat& image, const std::string& text, cv::Point centrePos, double size, int32_t thickness) const;
};

}

#endif // _VIDEO_OVERLAY_H_
