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

#include "Logger.h"

#include <sys/syslog.h>

#include <string>

namespace thermal {
namespace log {

LogLevel logLevelSetting = LogLevel::kDebug;
inline constexpr const char * const kAppName = "thermal-scope-app"; 

void LogInit() {
    openlog(kAppName, LOG_PID | LOG_CONS, LOG_USER);
}

void LogDeinit() {
    closelog();
}

LogLevel GetLogLevel() {
    return logLevelSetting;
}

void SetLogLevel(LogLevel logLevel) {
    logLevelSetting = logLevel;
}

void LogInternal(LogLevel level, int32_t lineNumber, const char * filename, const char * function, const char * fmt, ...) {
    if (level <= logLevelSetting) {
        const char* begin = filename + std::char_traits<char>::length(filename);

        while ((begin > filename) && (*(begin - 1) != '/') && (*(begin - 1) != '\\')) {
            begin--;
        }

        const char* ptr = begin;
        while ((*ptr != '\0') && (*ptr != '.')) {
            ptr++;
        }

        std::string message(begin, ptr - begin);
        message = message + ":" + std::to_string(lineNumber) + " " + function + "(): " + fmt;

        va_list args;
        va_start(args, fmt);
        vsyslog(static_cast<int32_t>(level), message.c_str(), args);
        va_end(args);
    }
}

} // log
} // thermal
