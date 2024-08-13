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

#ifndef _FRAME_BUFFER_H_
#define _FRAME_BUFFER_H_

#include <stdint.h>
#include <linux/fb.h>

#include <cstddef>
#include <string>
#include <thread>

namespace thermal {
namespace hw {

class FrameBuffer {
public:
    FrameBuffer(std::string device);
    ~FrameBuffer();
    bool Write(uint8_t* image, size_t size);

private:
    int32_t mFileDescriptor;
    std::string mDeviceName;
    size_t mBufferSize;
    fb_fix_screeninfo mFInfo;
    fb_var_screeninfo mVInfo;
    char* mFrameBufferPtr;

    void PrintInfo();
};

} // namespace display
} // namespace hw

#endif // _FRAME_BUFFER_H_