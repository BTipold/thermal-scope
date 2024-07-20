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

#include "Reticle.h"

#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>

#include <iostream>
#include <fstream>

#include "Logger.h"
#include "UsbControl.h"

namespace thermal {

Reticle::Reticle(const std::string& path) 
    : mReticle()
    , mSource()
    , mXOffset(0)
    , mYOffset(0) {
    DLOG_DEBUG("loading reticle img: %s", path.c_str());

    // Load the reticle image
    mSource = cv::imread(path, cv::IMREAD_UNCHANGED);

    // Check if the file exists and can be opened
    if (mSource.empty()) {
        DLOG_ERROR("Failed to load reticle image from path: %s", path.c_str());
    }

    // Ensure the reticle image is of size 240x240
    if (mSource.size() != cv::Size(240, 240)) {
        cv::resize(mReticle, mReticle, cv::Size(240, 240));
    }

    // Ensure the reticle image is in RGBA format
    if (mSource.channels() == 3) {
        cv::cvtColor(mReticle, mReticle, cv::COLOR_BGR2RGBA);
    }
    return;
}

Reticle::~Reticle() {
    DLOG_DEBUG("");
}

void Reticle::Overlay(cv::Mat& frame) const {
    
}

void Reticle::SetOffset(size_t x, size_t y) {
    DLOG_DEBUG("changed reticle offset %ux%u", x, y);
    mXOffset = x;
    mYOffset = y;

    // use warp affine as per this post: 
    // https://stackoverflow.com/questions/19068085/shift-image-content-with-opencv/26766505#26766505
    cv::Mat transform = (cv::Mat_<double>(2,3) << 1, 0, x, 0, 1, y);
    cv::warpAffine(mSource, mReticle, transform, mSource.size());
}

} // namespace thermal
