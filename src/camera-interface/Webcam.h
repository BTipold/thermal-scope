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

#ifndef _WEBCAM_H_
#define _WEBCAM_H_

#include <opencv2/videoio.hpp>
#include <stdint.h>

#include <vector>
#include <functional>
#include <thread>
#include <chrono>

namespace thermal {
namespace p2pro {

typedef std::function<bool(cv::Mat&, bool)> VideoCallback;

enum class WebcamState : uint8_t {
    kNotConnected,
    kConnectedAndStopped,
    kRunning
};

inline const char * WebcamStateToStr(WebcamState webcamState) {
    switch (webcamState) {
        case WebcamState::kNotConnected:
            return "NOT CONNECTED";
        case WebcamState::kConnectedAndStopped:
            return "CONNECTED+STOPPED";
        case WebcamState::kRunning:
            return "CONNECTED+RUNNING";
        default:
            return "ERR";
    }
}

class Webcam {
public:
    Webcam(size_t w, size_t h, int32_t fps, int32_t devId);

    void RegisterOnDataCallback(VideoCallback fptr);
    void UnregisterCallback(VideoCallback fptr);
    bool Open();
    bool Start();
    bool Stop();
    void ReleaseCamera();
    WebcamState GetState() const;

private:
    std::vector<VideoCallback> mDataCallbacks;
    cv::VideoCapture mCameraSource;
    std::thread mReadThread;
    WebcamState mState;
    size_t mWidth;
    size_t mHeight;
    int32_t mFrameRate;
    int32_t mDeviceId;
    bool mRunFlag;

    void Runloop();
};

} // namespace webcam
} // namespace thermal

#endif // _WEBCAM_H_
