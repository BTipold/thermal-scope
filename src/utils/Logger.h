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

#ifndef _LOGGER_H_
#define _LOGGER_H_

#include <syslog.h>
#include <stdint.h>

#include <cstdarg>
/**
 * @brief Logs a debug message with the specified format.
 *
 * @param fmt The format string.
 * @param ... The format arguments.
 */
#define DLOG_DEBUG(fmt, ...) LogInternal(thermal::log::LogLevel::kDebug, __LINE__, __FILE__, __func__, (fmt), ##__VA_ARGS__)

/**
 * @brief Logs an informational message with the specified format.
 *
 * @param fmt The format string.
 * @param ... The format arguments.
 */
#define DLOG_INFO(fmt, ...) LogInternal(thermal::log::LogLevel::kInformational, __LINE__, __FILE__, __func__, (fmt), ##__VA_ARGS__)

/**
 * @brief Logs a notice message with the specified format.
 *
 * @param fmt The format string.
 * @param ... The format arguments.
 */
#define DLOG_NOTICE(fmt, ...) LogInternal(thermal::log::LogLevel::kNotice, __LINE__, __FILE__, __func__, (fmt), ##__VA_ARGS__)

/**
 * @brief Logs a warning message with the specified format.
 *
 * @param fmt The format string.
 * @param ... The format arguments.
 */
#define DLOG_WARN(fmt, ...) LogInternal(thermal::log::LogLevel::kWarning, __LINE__, __FILE__, __func__, (fmt), ##__VA_ARGS__)

/**
 * @brief Logs an error message with the specified format.
 *
 * @param fmt The format string.
 * @param ... The format arguments.
 */
#define DLOG_ERROR(fmt, ...) LogInternal(thermal::log::LogLevel::kError, __LINE__, __FILE__, __func__, (fmt), ##__VA_ARGS__)

/**
 * @brief Logs a critical message with the specified format.
 *
 * @param fmt The format string.
 * @param ... The format arguments.
 */
#define DLOG_CRIT(fmt, ...) LogInternal(thermal::log::LogLevel::kCritical, __LINE__, __FILE__, __func__, (fmt), ##__VA_ARGS__)

/**
 * @brief Logs an alert message with the specified format.
 *
 * @param fmt The format string.
 * @param ... The format arguments.
 */
#define DLOG_ALERT(fmt, ...) LogInternal(thermal::log::LogLevel::kAlert, __LINE__, __FILE__, __func__, (fmt), ##__VA_ARGS__)

/**
 * @brief Logs an emergency message with the specified format.
 *
 * @param fmt The format string.
 * @param ... The format arguments.
 */
#define DLOG_EMERGENCY(fmt, ...) LogInternal(thermal::log::LogLevel::kEmergency, __LINE__, __FILE__, __func__, (fmt), ##__VA_ARGS__)

namespace thermal {
namespace log {

/**
 * @brief Enumeration representing different log levels.
 */
enum class LogLevel : uint8_t {
    kDebug = 7,        ///< Debug level messages.
    kInformational = 6,///< Informational messages.
    kNotice = 5,       ///< Normal but significant conditions.
    kWarning = 4,      ///< Warning conditions.
    kError = 3,        ///< Error conditions.
    kCritical = 2,     ///< Critical conditions.
    kAlert = 1,        ///< Action must be taken immediately.
    kEmergency = 0     ///< System is unusable.
};

/**
 * @brief Initializes the logging system.
 */
void LogInit();

/**
 * @brief Deinitializes the logging system.
 */
void LogDeinit();

/**
 * @brief Gets the current log level.
 *
 * @return The current log level.
 */
LogLevel GetLogLevel();

/**
 * @brief Sets the log level.
 *
 * @param logLevel The log level to set.
 */
void SetLogLevel(LogLevel logLevel);

/**
 * @brief Internal logging function.
 *
 * This function is used by the logging macros to log messages at the specified
 * log level with additional context information such as line number, filename,
 * and function name.
 *
 * @param level The log level.
 * @param lineNumber The line number where the log is generated.
 * @param filename The filename where the log is generated.
 * @param function The function name where the log is generated.
 * @param fmt The format string.
 * @param ... The format arguments.
 */
void LogInternal(LogLevel level, int32_t lineNumber, const char * filename, const char * function, const char * fmt, ...);

} // log
} // thermal

#endif // _LOGGER_H_