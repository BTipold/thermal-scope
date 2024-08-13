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

#ifndef _DELAYED_WRITER_H_
#define _DELAYED_WRITER_H_

#include <stdint.h>

#include <fstream>
#include <sstream>
#include <chrono>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>

namespace thermal {
namespace utils {

class DelayedWriterBuffer : public std::streambuf {
public:
    DelayedWriterBuffer(std::chrono::seconds delay, std::string path);
    ~DelayedWriterBuffer();

    void Flush();
    void Clear();

protected:
    int overflow(int c) override;
    std::streamsize xsputn(const char* s, std::streamsize n) override;
    int sync() override;

private:
    std::thread mWriterThread;
    std::mutex mMutex;
    std::mutex mWriterThreadMutex;
    std::condition_variable mCv;
    std::stringbuf mBuffer;
    std::chrono::system_clock::time_point mLastData;
    std::chrono::seconds mDelay;
    std::string mPath;
    bool mThreadRunning;
    bool mPendingData;

    void OnWrite();
    void WriteThread();
};

class DelayedWriter : public std::ostream {
public:
    DelayedWriter(std::chrono::seconds delay, std::string path);
    ~DelayedWriter();

    void Clear();

private:
    DelayedWriterBuffer mBuffer;
};

} // namespace utils
} // namespace thermal

#endif // _DELAYED_WRITER_H_
