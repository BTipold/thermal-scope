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

#include "FrameBuffer.h"

#include <opencv2/opencv.hpp>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include <condition_variable>
#include <string>

#include "Logger.h"

namespace thermal {
namespace hw {

FrameBuffer::FrameBuffer(std::string device)
	: mFileDescriptor(-1)
    , mDeviceName(device)
	, mBufferSize(0ull)
	, mFrameBufferPtr(nullptr) {

    // Open the file for reading and writing
    mFileDescriptor = ::open(device.c_str(), O_RDWR);
    if (!mFileDescriptor) {
        DLOG_ERROR("cannot open %s", device.c_str());
        return;
    }

    DLOG_NOTICE("%s was opened, fd=%d", device.c_str(), mFileDescriptor);

    if (::ioctl(mFileDescriptor, FBIOGET_FSCREENINFO, &mFInfo)) {
        DLOG_ERROR("Error reading fixed information.\n");
    }

    if (::ioctl(mFileDescriptor, FBIOGET_VSCREENINFO, &mVInfo)) {
        DLOG_ERROR("Error reading variable information.\n");
    }

    PrintInfo();

    // map framebuffer to user memory 
    mBufferSize = mFInfo.smem_len;
    mFrameBufferPtr = (char*)::mmap(0, mBufferSize, PROT_READ | PROT_WRITE, MAP_SHARED, mFileDescriptor, 0);

    if ((int)mFrameBufferPtr == -1) {
        DLOG_ERROR("Failed to mmap");
    } else {
        DLOG_DEBUG("Initialize mmap at %p", mFrameBufferPtr);
    }
    return;
}

FrameBuffer::~FrameBuffer() {
    ::munmap(mFrameBufferPtr, mBufferSize);
    ::close(mFileDescriptor);
	return;
}

bool FrameBuffer::Write(uint8_t* image, size_t size) {
    bool status = false;

    if (image != nullptr) {
        DLOG_DEBUG("writing image [%p] to /dev/fb0 [%p] with size %u\n", image, mFrameBufferPtr, size);
        if (size > mBufferSize) {
            DLOG_WARN("likely overflow, size %u > screen size: %u", size, mBufferSize);
            return false;
        } else {
            // This is where the data will be written to the LCD.
            // If the format doesn't match the framebuffer settings,
            // then the image will probably look fucked... 
            ::memcpy(mFrameBufferPtr, (char*)image, size);
            return true;
        }
    } else {
        DLOG_ERROR("image is nullptr");
        return false;
    }
}

void FrameBuffer::PrintInfo() {
    DLOG_DEBUG("Variable Screen Info:\n");
    DLOG_DEBUG("  Resolution: %dx%d\n", mVInfo.xres, mVInfo.yres);
    DLOG_DEBUG("  Virtual Resolution: %dx%d\n", mVInfo.xres_virtual, mVInfo.yres_virtual);
    DLOG_DEBUG("  Offset: %dx%d\n", mVInfo.xoffset, mVInfo.yoffset);
    DLOG_DEBUG("  Bits per Pixel: %d\n", mVInfo.bits_per_pixel);
    DLOG_DEBUG("  Red: %d:%d (%s)\n", mVInfo.red.offset, mVInfo.red.length, mVInfo.red.msb_right ? "MSB" : "LSB");
    DLOG_DEBUG("  Green: %d:%d (%s)\n", mVInfo.green.offset, mVInfo.green.length, mVInfo.green.msb_right ? "MSB" : "LSB");
    DLOG_DEBUG("  Blue: %d:%d (%s)\n", mVInfo.blue.offset, mVInfo.blue.length, mVInfo.blue.msb_right ? "MSB" : "LSB");
    DLOG_DEBUG("  Transp: %d:%d (%s)\n", mVInfo.transp.offset, mVInfo.transp.length, mVInfo.transp.msb_right ? "MSB" : "LSB");

    DLOG_DEBUG("Fixed Screen Info:\n");
    DLOG_DEBUG("  Line Length: %d\n", mFInfo.line_length);
    DLOG_DEBUG("  Memory Length: %d\n", mFInfo.smem_len);
    DLOG_DEBUG("  Type: %d\n", mFInfo.type);
    DLOG_DEBUG("  Visual: %d\n", mFInfo.visual);
}

} // namespace hw
} // namespace thermal
