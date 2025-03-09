cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64")
set(CMAKE_CXX_VISIBILITY_PRESET hidden)

if ("${GEODE_TARGET_PLATFORM}" STREQUAL "iOS" OR IOS)
    set(CMAKE_OSX_ARCHITECTURES "arm64")
    add_definitions(-DGLES_SILENCE_DEPRECATION)
else()
    set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64")
endif()

project(BetterPrideTrails VERSION 4.2.0)

add_library(${PROJECT_NAME} SHARED src/main.cpp
        src/hooks/PlayerObject.cpp
        src/bettercocos/CCMultiColorMotionStreak.cpp
        src/bettercocos/CCMultiColorMotionStreak.h
        src/utils/color.h
        src/utils/color.cpp
        src/utils/customSettings.cpp
        src/utils/customSettings.h
        src/colorTypes.h
        src/ui/colorPopup.cpp
        src/ui/colorPopup.h
        src/ui/colorCell.cpp
        src/ui/colorCell.h
        src/ui/GrabNodeLayer.cpp
        src/ui/GrabNodeLayer.h
        # src/main.hpp
)

if (NOT DEFINED ENV{GEODE_SDK})
    message(FATAL_ERROR "Unable to find Geode SDK! Please define GEODE_SDK environment variable to point to Geode")
else()
    message(STATUS "Found Geode: $ENV{GEODE_SDK}")
endif()

add_subdirectory($ENV{GEODE_SDK} ${CMAKE_CURRENT_BINARY_DIR}/geode)
include_directories($ENV{GEODE_SDK}/loader/include/Geode)

setup_geode_mod(${PROJECT_NAME})
