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

#include "VideoOverlay.h"

#include <opencv2/opencv.hpp>

#include "Reticle.h"
#include "Logger.h"

namespace thermal {

inline constexpr const char* const kReticlePath = "/etc/thermal-scope/reticles/";

VideoOverlay::VideoOverlay() 
    : mReticleList()
    , mTopMode()
    , mSideMode() {
    return;
}

VideoOverlay::~VideoOverlay() {
    return;
}

void VideoOverlay::Overlay(cv::Mat& frame) const {
    if (frame.size() != mFinalOverlay.size()) {
        DLOG_ERROR("Frame size does not match reticle size.");
    }

    // Blend the reticle with the frame
    for (int y = 0; y < frame.rows; ++y) {
        for (int x = 0; x < frame.cols; ++x) {
            cv::Vec4b& framePixel = frame.at<cv::Vec4b>(y, x);
            const cv::Vec4b& reticlePixel = mFinalOverlay.at<cv::Vec4b>(y, x);

            // Blend only if the reticle pixel is not fully transparent
            if (reticlePixel[3] > 0) {
                float alpha = reticlePixel[3] / 255.0f;
                for (int c = 0; c < 3; ++c) {
                    framePixel[c] = framePixel[c] * (1 - alpha) + reticlePixel[c] * alpha;
                }
            }
        }
    }
    return;
}

void VideoOverlay::SetOffset(int32_t x, int32_t y) {
    auto reticle = mReticleList.at(mReticleType);
    reticle.SetOffset(x, y);
}

void VideoOverlay::SetX(int32_t x) {
}

void VideoOverlay::SetY(int32_t y) {
}

void VideoOverlay::SetTopMenuMode(TopMode mode) {
    mTopMode = mode;

    switch (mTopMode) {
    case TopMode::kXOffset: {
    } break;

    case TopMode::kPickReticle:
        break;
  
    case TopMode::kPickColor:
        break;

    case TopMode::kNone:
    default:
        break;
    }
}

void VideoOverlay::SetSideMenuMode(SideMode mode) {
    mSideMode = mode;

    switch (mSideMode) {
    case SideMode::kYOffset: {

    } break;

    case SideMode::kZoom: {
        
    } break;
  
    case SideMode::kNone:
    default:
        break;
    }
}


} // namespace thermal
