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

#include "DelayedWriter.h"

#include <fstream>
#include <sstream>
#include <chrono>
#include <thread>
#include <mutex>
#include <condition_variable>

#include "Logger.h"

namespace thermal {
namespace utils {

DelayedWriterBuffer::DelayedWriterBuffer(std::chrono::seconds delay, std::string path)
    : mLastData()
    , mDelay(delay)
    , mPath(path)
    , mThreadRunning(false)
    , mPendingData(false) {
    return;
}

DelayedWriterBuffer::~DelayedWriterBuffer() {
    Flush();
}

void DelayedWriterBuffer::Flush() {
    DLOG_DEBUG("Flushing buffer to file %s", mPath.c_str());
    if (!mBuffer.str().empty()) {
        std::ofstream file(mPath, std::ios::out | std::ios::trunc);
        if (file.is_open()) {
            file << mBuffer.str();
            file.close();
        } else {
            DLOG_WARN("Failed to open %s", mPath.c_str());
        }
        mBuffer.str("");
    }

    mPendingData = false;
    return;
}

void DelayedWriterBuffer::Clear() {
    DLOG_DEBUG("Clearing");
    mBuffer.str("");
    mPendingData = false;
    return;
}

int DelayedWriterBuffer::overflow(int c) {
    if (c != EOF) {
        mBuffer.sputc(c);
        OnWrite();
    }
    return c;
}

std::streamsize DelayedWriterBuffer::xsputn(const char* s, std::streamsize n) {
    std::streamsize written = mBuffer.sputn(s, n);
    if (written > 0) {
        OnWrite();
    }

    return written;
}

int DelayedWriterBuffer::sync() {
    OnWrite();
    return 0; // return 0 to indicate success
}

void DelayedWriterBuffer::OnWrite() {
    std::unique_lock<std::mutex> lock(mMutex);
    mLastData = std::chrono::system_clock::now();
    mPendingData = true;

    // if timer thread is not running, we need to start it
    if (!mThreadRunning) {

        // Check if thread needs to be joined
        if (mWriterThread.joinable()) {
            mWriterThread.join();
        }
        mWriterThread = std::thread(&DelayedWriterBuffer::WriteThread, this);

    // if timer thread is already running, notify CV so that
    // the timeout restarts. When the CV times out, the buffer will
    // be flushed.
    } else {
        mCv.notify_all();
    }

    mThreadRunning = true;
    return;
}

void DelayedWriterBuffer::WriteThread() {
    DLOG_DEBUG("Starting delayed write thread");

    // loop here until the pending data is flushed
    while(mPendingData) {
        std::unique_lock<std::mutex> lock(mWriterThreadMutex);
        auto cvStatus = mCv.wait_until(lock, mLastData + mDelay);

        // if the CV was never notified it will timeout. This means we had no new data 
        // in past period, and we can flush the buffer. If the CV is notified, then this
        // means we got new data, and we should loop one more time to restart the timer.
        if (cvStatus == std::cv_status::timeout) {
            Flush();
        }
    }

    mThreadRunning = false;
    return;
}

DelayedWriter::DelayedWriter(std::chrono::seconds delay, std::string path)
    : std::ostream(&mBuffer), mBuffer(delay, path) {};

DelayedWriter::~DelayedWriter() {
    mBuffer.Flush();
    return;
}

void DelayedWriter::Clear() {
    mBuffer.Clear();
    return;
}

} // namespace utils
} // namespace thermal
