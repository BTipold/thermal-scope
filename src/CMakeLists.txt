cmake_minimum_required(VERSION 3.0)
project(thermal-scope-app VERSION 1.0.0 LANGUAGES C CXX)

# Set the C++ standard to C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# define some variables to help
cmake_path(GET PROJECT_SOURCE_DIR PARENT_PATH THERMAL_SCOPE_PACKAGE)
set(BUILDROOT_ROOT_DIR ${THERMAL_SCOPE_PACKAGE}/../../../)
set(BUILD_DIR ${BUILDROOT_ROOT_DIR}/output/build/)
set(TARGET_DIR ${BUILDROOT_ROOT_DIR}/output/target/)
set(MAIN_SRC_DIR ${THERMAL_SCOPE_PACKAGE}/src)
set(THIRD_PARTY ${MAIN_SRC_DIR}/third-party/)
set(RESOURCES ${THERMAL_SCOPE_PACKAGE}/resources/)

# declare libs + dependencies
add_library(opencv_core SHARED IMPORTED)
set_target_properties(opencv_core PROPERTIES
    IMPORTED_LOCATION ${TARGET_DIR}/usr/lib/libopencv_core.so
    INTERFACE_INCLUDE_DIRECTORIES ${BUILD_DIR}/opencv3-3.4.19/include/opencv2)

add_library(opencv_videoio SHARED IMPORTED)
set_target_properties(opencv_videoio PROPERTIES
    IMPORTED_LOCATION  ${TARGET_DIR}/usr/lib/libopencv_videoio.so
    INTERFACE_INCLUDE_DIRECTORIES ${BUILD_DIR}/opencv3-3.4.19/modules/videoio/include/)

add_library(opencv_imgproc SHARED IMPORTED)
set_target_properties(opencv_imgproc PROPERTIES
    IMPORTED_LOCATION  ${TARGET_DIR}/usr/lib/libopencv_imgproc.so
    INTERFACE_INCLUDE_DIRECTORIES ${BUILD_DIR}/opencv3-3.4.19/modules/imgproc/include/)

add_library(opencv_imgcodecs SHARED IMPORTED)
set_target_properties(opencv_imgcodecs PROPERTIES
    IMPORTED_LOCATION  ${TARGET_DIR}/usr/lib/libopencv_imgcodecs.so
    INTERFACE_INCLUDE_DIRECTORIES ${BUILD_DIR}/opencv3-3.4.19/modules/imgcodecs/include/)

add_library(lgpio SHARED IMPORTED)
set_target_properties(lgpio PROPERTIES
    IMPORTED_LOCATION ${THIRD_PARTY}/liblgpio/lib/liblgpio.so
    INTERFACE_INCLUDE_DIRECTORIES ${THIRD_PARTY}/liblgpio/include)

add_library(usb-1.0 SHARED IMPORTED)
set_target_properties(usb-1.0 PROPERTIES
    IMPORTED_LOCATION  ${TARGET_DIR}/usr/lib/libusb-1.0.so
    INTERFACE_INCLUDE_DIRECTORIES ${BUILD_DIR}/libusb-1.0.27/libusb/)

add_library(jsoncpp SHARED IMPORTED)
set_target_properties(jsoncpp PROPERTIES
    IMPORTED_LOCATION  ${TARGET_DIR}/usr/lib/libjsoncpp.so
    INTERFACE_INCLUDE_DIRECTORIES ${BUILD_DIR}/jsoncpp-1.9.5/include/)
        
# include paths
include_directories(${MAIN_SRC_DIR}/application/)
include_directories(${MAIN_SRC_DIR}/camera-interface/)
include_directories(${MAIN_SRC_DIR}/hw/)
include_directories(${MAIN_SRC_DIR}/utils/)
include_directories(${MAIN_SRC_DIR}/common/)

# sources to compile
set(SRC_FILES_TO_COMPILE
    ${MAIN_SRC_DIR}/application/main.cpp
    ${MAIN_SRC_DIR}/application/ThermalScopeApplication.cpp
    ${MAIN_SRC_DIR}/application/Reticle.cpp
    ${MAIN_SRC_DIR}/application/VideoOverlay.cpp
    ${MAIN_SRC_DIR}/camera-interface/Webcam.cpp
    ${MAIN_SRC_DIR}/camera-interface/UsbControl.cpp
    ${MAIN_SRC_DIR}/camera-interface/P2ProManager.cpp
    ${MAIN_SRC_DIR}/utils/Logger.cpp
    ${MAIN_SRC_DIR}/utils/DelayedWriter.cpp
    ${MAIN_SRC_DIR}/hw/FrameBuffer.cpp
    ${MAIN_SRC_DIR}/hw/Encoder.cpp
    ${MAIN_SRC_DIR}/hw/GpioWatcher.cpp
)

# this builds the actual binary
add_executable(${CMAKE_PROJECT_NAME} ${SRC_FILES_TO_COMPILE})
target_link_libraries(${CMAKE_PROJECT_NAME} opencv_core opencv_videoio opencv_imgproc opencv_imgcodecs lgpio usb-1.0 jsoncpp)

# Install the files
install(TARGETS ${CMAKE_PROJECT_NAME} RUNTIME DESTINATION bin)
install(FILES ${THIRD_PARTY}/liblgpio/lib/liblgpio.so DESTINATION lib)
install(FILES ${THIRD_PARTY}/liblgpio/lib/liblgpio.so.1 DESTINATION lib)
install(FILES ${RESOURCES}/reticles/default.png DESTINATION /etc/thermal-scope/reticles/)
install(FILES ${RESOURCES}/reticles/cross.png DESTINATION /etc/thermal-scope/reticles/)
install(FILES ${RESOURCES}/reticles/chevron.png DESTINATION /etc/thermal-scope/reticles/)
install(FILES ${RESOURCES}/reticles/small.png DESTINATION /etc/thermal-scope/reticles/)
install(FILES ${RESOURCES}/reticles/dot.png DESTINATION /etc/thermal-scope/reticles/)
install(FILES ${RESOURCES}/reticles/eotech.png DESTINATION /etc/thermal-scope/reticles/)
