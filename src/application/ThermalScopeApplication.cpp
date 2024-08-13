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

#include "ThermalScopeApplication.h"

#include <opencv2/opencv.hpp>

#include <condition_variable>
#include <mutex>
#include <functional>
#include <algorithm>

#include "Logger.h"
#include "UsbControl.h"
#include "Utils.h"

namespace thermal {

constexpr const size_t kP2ProResolutionWidth = 256u;
constexpr const size_t kP2ProResolutionHeight = 192u;
constexpr const size_t kLcd1in28Width = 240u;
constexpr const size_t kLcd1in28Height = 240u;
constexpr const int32_t kP2ProFrameRate = 25u;
constexpr const int32_t kP2ProDevId = 0;
constexpr const int32_t kFrameBufferChannels = 4;
constexpr const int32_t kExpectedFrameSize = kLcd1in28Width * kLcd1in28Height * kFrameBufferChannels;

constexpr const int32_t kSideEncoderGpioA = 13;
constexpr const int32_t kSideEncoderGpioB = 19;
constexpr const int32_t kSideEncoderGpioBtn = 26;
constexpr const int32_t kTopEncoderGpioA = 20;
constexpr const int32_t kTopEncoderGpioB = 21;
constexpr const int32_t kTopEncoderGpioBtn = 16;

inline constexpr const char* const kFrameBuffer0 = "/dev/fb0";

using std::shared_ptr;
using std::placeholders::_1;
using std::mutex;
using std::unique_lock;
using std::make_shared;
using std::make_unique;
using hw::Direction;

ThermalScopeApplication::ThermalScopeApplication(int32_t argc, char* argv[]) 
    : mP2ProManager(nullptr)
    , mFrameBuffer(kFrameBuffer0)
    , mSideEncoder(kSideEncoderGpioA, kSideEncoderGpioB, kSideEncoderGpioBtn)
    , mTopEncoder(kTopEncoderGpioA, kTopEncoderGpioB, kTopEncoderGpioBtn)
    , mTopMode(TopMode::kNone)
    , mSideMode(SideMode::kNone)
    , mColorSetting(p2pro::ColorMode::kPseudoRainbow4, "color")
    , mReticleSetting(ReticleType::kDefault, "reticle")
    , mXOffsetSetting(0, "x")
    , mYOffsetSetting(0, "y")
    , mZoomSetting(0, "zoom") {}

ThermalScopeApplication::~ThermalScopeApplication() {}

void ThermalScopeApplication::Init() {
    shared_ptr<p2pro::Webcam> camera = make_shared<p2pro::Webcam>(
        kP2ProResolutionWidth, kP2ProResolutionHeight, kP2ProFrameRate, kP2ProDevId);
    shared_ptr<p2pro::UsbControl> control = make_shared<p2pro::UsbControl>();
    mP2ProManager = make_unique<p2pro::P2ProManager>(camera, control);

    // Setup the callbacks
    camera->RegisterOnDataCallback(std::bind(&ThermalScopeApplication::OnCameraData, this, _1));
    mSideEncoder.SetOnClickCallback(std::bind(&ThermalScopeApplication::OnClickSide, this, _1));
    mSideEncoder.SetOnRotateCallback(std::bind(&ThermalScopeApplication::OnRotateSide, this, _1));
    mTopEncoder.SetOnClickCallback(std::bind(&ThermalScopeApplication::OnClickTop, this, _1));
    mTopEncoder.SetOnRotateCallback(std::bind(&ThermalScopeApplication::OnRotateTop, this, _1));

    // Load settings from filesystem
    mColorSetting.Load();
    mXOffsetSetting.Load();
    mYOffsetSetting.Load();
    mZoomSetting.Load();

    // Initialize offset with the saved settings
    mOverlay.SetOffset(mXOffsetSetting, mYOffsetSetting);

    // todo
    // mP2ProManager->CommandMode();
    // p2pro::ColorMode currentColorModeSetOnDevice = mP2ProManager->GetCurrentActiveColorMode();
    // if (mColorSetting != currentColorModeSetOnDevice) {
    //     if (!mP2ProManager->SetPseudoColor(mColorSetting)) {
    //         DLOG_ERROR("failed setting pseudo color");
    //     }
    // }

    //DLOG_INFO("color setting is %s", p2pro::ColorToString(mColorSetting.Get()).c_str() );

    mP2ProManager->SetPseudoColor(p2pro::ColorMode::kPseudoBlackHot);
}

void ThermalScopeApplication::Run() {
    if (mP2ProManager == nullptr) {
        DLOG_ERROR("p2 pro is not initialized correctly.");
        return;
    }

	if (mP2ProManager->StartVideoStream()) {
		// block here and let the app run
		mutex mtx;
		std::condition_variable cv;
		unique_lock<mutex> lk(mtx);
		cv.wait(lk);
	} else {
        DLOG_ERROR("P2 Pro failed to start");
	}
}

bool ThermalScopeApplication::OnCameraData(cv::Mat &frame, bool lastFrame) {
    if (lastFrame) {
        
    }

    size_t width = frame.cols;
    size_t height = frame.rows;

    // Resize the image to the size of the LCD screen 240x240
    cv::Mat resized;
    cv::resize(frame, resized, cv::Size(240, 240));
    cv::rotate(resized, resized, cv::ROTATE_90_COUNTERCLOCKWISE);

    // Convert the resized image to 32 bpp (8 bits each for R, G, B, and transparency)
    cv::Mat formatted;
    cv::cvtColor(resized, formatted, cv::COLOR_BGR2RGBA);

    // Apply the overlay. 
    mOverlay.Overlay(formatted);

    // Write frame to /dev/fb0 (this is where the image gets displayed)
    size_t dataSize = formatted.rows * formatted.cols * formatted.channels();
    if (dataSize == kExpectedFrameSize) {
        mFrameBuffer.Write(formatted.data, dataSize);
        return true;
    } else {
        DLOG_WARN("unexpected data size %u, should be %u", dataSize, kExpectedFrameSize);
        return false;
    }
}

void ThermalScopeApplication::OnRotateSide(Direction direction) {
    // incrememnt is +1 and decrememnt is -1
    int32_t adjustment = (direction == hw::Direction::kIncrement) ? +1 : -1;
    
    switch (mSideMode) {
    case SideMode::kYOffset: {
        constexpr const int32_t kMin = -50;
        constexpr const int32_t kMax = 50;
        mYOffsetSetting = std::clamp(mYOffsetSetting + adjustment, kMin, kMax);
        mYOffsetSetting.Save();
        mOverlay.SetY(mYOffsetSetting);
    } break;
    
    case SideMode::kZoom: {
        constexpr const uint32_t kMin = 0u;
        constexpr const uint32_t kMax = 100u;
        mZoomSetting = std::clamp(mZoomSetting + adjustment, kMin, kMax);
        mZoomSetting.Save();
        mOverlay.SetZoom(mZoomSetting);
    } break;

    case SideMode::kNone:
    default:
        break;
    }
}

void ThermalScopeApplication::OnRotateTop(hw::Direction direction) {
    // incrememnt is +1 and decrememnt is -1
    int32_t adjustment = (direction == hw::Direction::kIncrement) ? +1 : -1;
    
    switch (mTopMode) {
    case TopMode::kXOffset: {
        constexpr const int32_t kMin = -50;
        constexpr const int32_t kMax = 50;
        mXOffsetSetting = std::clamp(mXOffsetSetting + adjustment, kMin, kMax);
        mXOffsetSetting.Save();
        mOverlay.SetX(mXOffsetSetting);
    } break;

    case TopMode::kPickReticle: {
        mReticleSetting = utils::RotateEnum<ReticleType>(mReticleSetting, static_cast<int32_t>(ReticleType::kCount), adjustment);
        mReticleSetting.Save();
        mOverlay.SetReticleType(mReticleSetting);
    } break;
  
    case TopMode::kPickColor: {
        mColorSetting = utils::RotateEnum<p2pro::ColorMode>(mColorSetting, static_cast<int32_t>(p2pro::ColorMode::kCount), adjustment);
        mColorSetting.Save();
        mOverlay.SetColorMode(mColorSetting);
        mP2ProManager->SetPseudoColor(mColorSetting);
    } break;

    case TopMode::kNone:
    default:
        break;
    }
}

void ThermalScopeApplication::OnClickSide(bool level) {
    if (level == false) {
        auto old = mSideMode;
        mSideMode = utils::RotateEnum<SideMode>(mSideMode, static_cast<int32_t>(SideMode::kCount));
        DLOG_DEBUG("%s -> %s", SideModeToString(old), SideModeToString(mSideMode));
        mOverlay.SetSideMenuMode(mSideMode);
    }
}

void ThermalScopeApplication::OnClickTop(bool level) {
    if  (level == false) {
        auto old = mTopMode;
        mTopMode = utils::RotateEnum<TopMode>(mTopMode, static_cast<int32_t>(TopMode::kCount));
        DLOG_DEBUG("%s -> %s", TopModeToString(old), TopModeToString(mTopMode));
        mOverlay.SetTopMenuMode(mTopMode);
    }
}
    
} // namespace thermal
