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

#include "Webcam.h"

#include <thread>
#include <condition_variable>
#include <chrono>
#include <mutex>
#include <chrono>

#include "Logger.h"

namespace thermal {
namespace p2pro {

Webcam::Webcam(size_t w, size_t h, int32_t fps, int32_t devId) 
    : mCameraSource()
    , mState(WebcamState::kNotConnected)
    , mWidth(w)
    , mHeight(h)
    , mFrameRate(fps)
    , mDeviceId(devId)
    , mRunFlag(false) {
    return;
}

void Webcam::RegisterOnDataCallback(VideoCallback fptr) {
    DLOG_DEBUG("registering callback to webcam");
    mDataCallbacks.push_back(fptr);
    return;
}

void Webcam::UnregisterCallback(VideoCallback fptr) {
    auto it = std::remove_if(mDataCallbacks.begin(), mDataCallbacks.end(), 
        [&fptr](const VideoCallback& callback) {
            return callback.target<bool(cv::Mat&)>() == fptr.target<bool(cv::Mat&)>();
        });

    if (it != mDataCallbacks.end()) {
        mDataCallbacks.erase(it, mDataCallbacks.end());
    }
    return;
}

bool Webcam::Start() {
    if (mState != WebcamState::kConnectedAndStopped) {
        DLOG_ERROR("Err: cannot transition from %s to Running", WebcamStateToStr(mState));
        return false;
    }

    DLOG_INFO("Starting webcam");
    mRunFlag = true;
    mReadThread = std::thread(&Webcam::Runloop, this);
    mState = WebcamState::kRunning;
    return true;
}

bool Webcam::Stop() {
    bool status = true;
    if (mState != WebcamState::kRunning) {
        DLOG_ERROR("Err: cannot transition from %s to Connected and Stopped", WebcamStateToStr(mState));
        status = false;
        return status;
    }

    mRunFlag = false;
    mReadThread.join();

    mState = WebcamState::kConnectedAndStopped;
    return status;
}

bool Webcam::Open() {
    DLOG_NOTICE("opening /dev/video%d", mDeviceId);
    mCameraSource.open(mDeviceId);

    std::mutex mtx;
    std::unique_lock<std::mutex> lock(mtx);
    std::condition_variable cv;

    bool result = cv.wait_for(lock, std::chrono::seconds(3), [this]() {
        return mCameraSource.isOpened();
    });

    if (mCameraSource.isOpened()) {
        DLOG_NOTICE("opened /dev/video%d", mDeviceId);
        mCameraSource.set(cv::CAP_PROP_FPS, static_cast<double>(mFrameRate));
        mCameraSource.set(cv::CAP_PROP_FRAME_WIDTH, static_cast<double>(mWidth));
        mCameraSource.set(cv::CAP_PROP_FRAME_HEIGHT, static_cast<double>(mHeight));
        mState = WebcamState::kConnectedAndStopped;
        DLOG_DEBUG("finished setting camera props");

    } else {
        DLOG_NOTICE("failed to open /dev/video%d", mDeviceId);
        mState = WebcamState::kNotConnected;
    }

    return (mState == WebcamState::kConnectedAndStopped);
}

void Webcam::ReleaseCamera() {
    DLOG_INFO("releasing /dev/video%d", mDeviceId);

    if (mCameraSource.isOpened()) {
        mCameraSource.release();
        mState = WebcamState::kNotConnected;
        DLOG_NOTICE("Released /dev/video%d", mDeviceId);

    } else {
        DLOG_WARN("did not release camera because it was not open.");
    }
    return;
}

WebcamState Webcam::GetState() const {
    return mState;
}

void Webcam::Runloop() {
    while (mRunFlag == true) {
        cv::Mat imgData;
        mCameraSource >> imgData;
        
        for (auto callback : mDataCallbacks) {
            callback(imgData, mRunFlag);
        }
    }
    return;
}

} // p2pro
} // thermal