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

#include <unordered_map>

#include "Reticle.h"
#include "Logger.h"

namespace thermal {

 const std::unordered_map<ReticleType, std::string> kReticlePaths = {
    { ReticleType::kDefault, "/etc/thermal-scope/reticles/default.png"},
    { ReticleType::kCross, "/etc/thermal-scope/reticles/cross.png"},
    { ReticleType::kChevron, "/etc/thermal-scope/reticles/chevron.png"},
    { ReticleType::kSmall, "/etc/thermal-scope/reticles/small.png"},
    { ReticleType::kDot, "/etc/thermal-scope/reticles/dot.png"},
    { ReticleType::kEotech, "/etc/thermal-scope/reticles/eotech.png"},
};

constexpr int32_t kThickness = 2;
constexpr int32_t kFontFace = cv::FONT_HERSHEY_SIMPLEX;


VideoOverlay::VideoOverlay() 
    : mReticle(kReticlePaths.at(ReticleType::kDefault))
    , mFinalOverlay()
    , mTopMsg{{TopMode::kXOffset, ""},
              {TopMode::kPickColor, ""},
              {TopMode::kPickReticle, ""}}
    , mSideMsg{{SideMode::kYOffset, ""},
               {SideMode::kZoom, ""}}
    , mTopMode(TopMode::kNone)
    , mSideMode(SideMode::kNone) {

    Redraw();
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
    mReticle.SetOffset(x, y);
    Redraw();
}

void VideoOverlay::SetX(int32_t x) {
    DLOG_DEBUG("adjusting x offset %d", x);
    mReticle.SetX(x);
    mTopMsg[TopMode::kXOffset] = std::to_string(x);
    Redraw();
}

void VideoOverlay::SetY(int32_t y) {
    DLOG_DEBUG("adjusting y offset %d", y);
    mReticle.SetY(y);
    mSideMsg[SideMode::kYOffset] = std::to_string(y);
    Redraw();
}

void VideoOverlay::SetZoom(int32_t level) {
    DLOG_DEBUG("adjusting zoom %d", level);
    mSideMsg[SideMode::kZoom] = std::to_string(level);
    Redraw();
}

void VideoOverlay::SetReticleType(ReticleType reticleType) {
    if (kReticlePaths.contains(reticleType)) {
        mReticle.SetImagePath(kReticlePaths.at(reticleType));
        mTopMsg[TopMode::kPickReticle] = std::string(ReticleTypeToStr(reticleType));
        Redraw();
    }
}

void VideoOverlay::SetColorMode(p2pro::ColorMode pseudocolor) {
    mTopMsg[TopMode::kPickColor] = p2pro::ColorToString(pseudocolor);
    Redraw();
    return;
}

void VideoOverlay::SetTopMenuMode(TopMode mode) {
    mTopMode = mode;
    Redraw();
    return;
}

void VideoOverlay::SetSideMenuMode(SideMode mode) {
    mSideMode = mode;
    Redraw();
    return;
}

void VideoOverlay::Redraw() {
    DLOG_DEBUG("recalculating overlay");
    mReticle.GetOverlay().copyTo(mFinalOverlay);

    if (mTopMode != TopMode::kNone) {
        static const std::unordered_map<TopMode, std::string> map {
            {TopMode::kNone, "Exit"}, 
            {TopMode::kXOffset, "Zero X"},
            {TopMode::kPickReticle, "Reticle"},
            {TopMode::kPickColor, "Colour Mode"},
        };

        std::string text = map.at(mTopMode);
        bool status = DrawTextCentreAligned(mFinalOverlay, text, cv::Point(120, 35), 0.4, kThickness);
        status &= DrawTextCentreAligned(mFinalOverlay, mTopMsg[mTopMode], cv::Point(120, 55), 0.4, kThickness);
    }

    if (mSideMode != SideMode::kNone) {
        static const std::unordered_map<SideMode, std::string> map {
            {SideMode::kNone, "Exit"},
            {SideMode::kYOffset, "Zero Y"},
            {SideMode::kZoom, "Zoom"},
        };

        std::string text = map.at(mSideMode);
        bool status = DrawTextCentreAligned(mFinalOverlay, text, cv::Point(190, 110), 0.4, kThickness);
        status &= DrawTextCentreAligned(mFinalOverlay, mSideMsg[mSideMode], cv::Point(190, 130), 0.4, kThickness);
    }
}

bool VideoOverlay::DrawTextCentreAligned(cv::Mat& image, const std::string& text, cv::Point centerPos, double size, int32_t thickness) const {
    cv::Scalar color(15, 15, 15, 255);

    // Get the text size
    int baseline = 0;
    cv::Size textSize = cv::getTextSize(text, kFontFace, size, thickness, &baseline);

    // Calculate the position for center alignment
    cv::Point textOrg(centerPos.x - (textSize.width / 2), centerPos.y + (textSize.height / 2));

    // Put the text in the center
    cv::putText(image, text, textOrg, kFontFace, size, color, thickness);
    return true;
}


} // namespace thermal
