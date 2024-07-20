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

#ifndef _RETICLE_H_
#define _RETICLE_H_

#include <opencv2/opencv.hpp>
#include <string>

namespace thermal {

enum class ReticleType : uint8_t {
    kDefault = 0,
    kCross = 1,
    kChevron = 2,
    kSmall = 3,
    kDot = 4,
    kEotech = 5,
    kCount
};

class Reticle {
public:
    Reticle(const std::string& path);
    ~Reticle();

    // Method to overlay the reticle on a given frame
    void Overlay(cv::Mat& frame) const;
    void SetOffset(size_t x, size_t y);

private:
    const size_t kWidth = 240u;
    const size_t kHeight = 240u;

    ReticleType mType;
    cv::Mat mSource;
    cv::Mat mReticle;
    size_t mXOffset;
    size_t mYOffset;
};

} // thermal

#endif // _RETICLE_H_