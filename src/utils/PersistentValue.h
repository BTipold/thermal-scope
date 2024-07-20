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

#ifndef _PERSISTENT_VALUE_H_
#define _PERSISTENT_VALUE_H_

#include <json/value.h>
#include <json/reader.h>
#include <json/writer.h>

#include <filesystem>
#include <fstream>
#include <functional>
#include <string>

#include "Logger.h"
#include "Utils.h"

namespace thermal {
namespace persistent {

inline constexpr const char * const kPersistentPath = "/var/data/persist/";

/**
 * @brief A base class for objects that can be saved and loaded persistently.
 *        Extend this class by implementing the Serialize and Deserialize methods.
 * 
 * @tparam T The type of the value to be saved and loaded.
 */
template <typename T>
class BaseSaveable {
public:

    /**
     * @brief Constructs a new BaseSaveable object.
     * 
     * @param key The key used for persistence (i.e., the filename).
     */
    BaseSaveable(std::string key) : mKey(key) {
        return;
    }

    /**
     * @brief Destroys the BaseSaveable object.
     */
    virtual ~BaseSaveable() = default;

    /**
     * @brief Loads the value from persistent storage.
     * 
     * @return true If the value was successfully loaded.
     * @return false If there was an error loading the value.
     */
    bool Load() {
        // Ensure directory exists
        std::string path = kPersistentPath + mKey;
        std::ifstream f(path, std::ios::in);

        // read from file
        if (f.is_open()) {
            Json::Value json;
            f >> json;
            Deserialize(json);
            f.close();
            DLOG_DEBUG("loaded %s", path.c_str());
            return true;

        } else {
            DLOG_WARN("error opening %s", path.c_str());
            return false;
        }
    }

    /**
     * @brief Saves the value to persistent storage.
     * 
     * @return true If the value was successfully saved.
     * @return false If there was an error saving the value.
     */
    bool Save() {
        // Ensure directory exists
        std::string path = kPersistentPath + mKey;
        utils::EnsureDirectoryExists(path);

        // write to file
        std::ofstream f(path, std::ios::trunc);
        if (f.is_open()) {
            Json::Value json = Serialize();
            f << json;
            f.close();
            DLOG_DEBUG("saving %s", path.c_str());
            return true;
            
        } else {
            DLOG_ERROR("error opening %s", path.c_str());
            return false;
        }
    }

    /**
     * @brief Gets the key used for persistence.
     * 
     * @return std::string The key.
     */
    std::string GetKey() const {
        return mKey;
    }

    /**
     * @brief Converts the object to a string representation.
     * 
     * @return std::string The string representation of the object.
     */
    std::string ToString() const {
        return mKey;
    }

    /**
     * @brief Serializes the object to a JSON value.
     * 
     * @return Json::Value The serialized JSON value.
     */
    virtual Json::Value Serialize() = 0;

    /**
     * @brief Deserializes the object from a JSON value.
     * 
     * @param json The JSON value to deserialize from.
     */
    virtual void Deserialize(Json::Value json) = 0;

protected:
    std::string mKey; ///< The key used for persistence.
};

/**
 * @brief A class for saving and loading primitive types. Can
 *        use as a wrapper for a primative type, or can be used
 *        with two template arguments with an automatic conversion.
 *        I.e. Value<int32_t, EnumType> -> enum will be converted to
 *        and saved as an int32_t.
 * 
 * @tparam TRawType The primitive type to be saved and loaded.
 * @tparam T The converted type to be used for storage.
 */
template <typename TRawType, typename T=TRawType> 
class Value : public BaseSaveable<T> {
public:

    /**
     * @brief Constructs a new Value object.
     * 
     * @param initialValue The initial value to be cached.
     * @param key The key used for persistence (i.e., the filename).
     */
    Value(T initialValue, std::string key) : BaseSaveable<T>(key), mCachedValue(initialValue) {
        return;
    }

    /**
     * @brief Destroys the Value object.
     */
    ~Value() = default;

    /**
     * @brief Assignment operator for setting the cached value.
     * 
     * @param other The new value to cache.
     * @return Value& Reference to the current object.
     */
    Value& operator=(T other) {
        mCachedValue = other;
        return *this;
    }

    /**
     * @brief Conversion operator for getting the cached value.
     * 
     * @return T The cached value.
     */
    operator T() const {
        return mCachedValue;
    }

    /**
     * @brief Serializes the object to a JSON value.
     * 
     * @return Json::Value The serialized JSON value.
     */
    Json::Value Serialize() override {
        Json::Value json;
        json["value"] = static_cast<TRawType>(mCachedValue);
        return json;
    }

    /**
     * @brief Deserializes the object from a JSON value.
     * 
     * @param json The JSON value to deserialize from.
     */
    void Deserialize(Json::Value json) override {
        if (json.isMember("value")) {
            TRawType primative = json["value"].as<TRawType>();
            mCachedValue = static_cast<T>(primative);
        }
    }

private:
    T mCachedValue; ///< The cached value to be saved or loaded.
};

} // namespace persistent
} // namespace thermal

#endif // _PERSISTENT_VALUE_H_
