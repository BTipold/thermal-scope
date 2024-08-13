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

inline constexpr const char * ReticleTypeToStr(ReticleType color) {
    switch (color) {
        case ReticleType::kDefault:
            return "Red";
        case ReticleType::kCross:
            return "Cross";
        case ReticleType::kChevron:
            return "Chevron";
        case ReticleType::kSmall:
            return "Small";
        case ReticleType::kDot:
            return "Dot";
        case ReticleType::kEotech:
            return "EOTech";
        case ReticleType::kCount:
        default:
            return "ERROR";
    }
}

class Reticle {
public:
    Reticle();
    Reticle(const std::string& path);
    ~Reticle();

    cv::Mat& GetOverlay();
    void SetImagePath(const std::string& path);
    void SetOffset(int32_t x, int32_t y);
    void SetX(int32_t x);
    void SetY(int32_t y);

private:
    const size_t kWidth = 240u;
    const size_t kHeight = 240u;

    ReticleType mType;
    cv::Mat mSource;
    cv::Mat mReticle;
    int32_t mXOffset;
    int32_t mYOffset;
};

} // thermal

#endif // _RETICLE_H_