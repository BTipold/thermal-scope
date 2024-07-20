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

#include <iostream>

#include "Logger.h"
#include "ThermalScopeApplication.h"

int main(int argc, char* argv[]) {
    try {
        thermal::log::LogInit();
        DLOG_NOTICE("Starting thermal scope application");
        
        thermal::ThermalScopeApplication app;
        app.Init();
        app.Run();
        return 0;
    
    } catch (const std::exception &e) {
        DLOG_ALERT("unhandled runtime exception %s", e.what());
        std::cout << e.what();

    } catch (...) {
        DLOG_ALERT("Unhandled exception in thermal camera application");
    }
}