cmake_minimum_required(VERSION 3.19)

if(APPLE AND NOT DEFINED CMAKE_OSX_ARCHITECTURES)
    set(CMAKE_OSX_ARCHITECTURES "x86_64;arm64" CACHE STRING "Build universal macOS binaries")
endif()

project(amoeba-dsp LANGUAGES C CXX)

option(AMOEBA_BUILD_EXTERNALS "Build the Max/MSP externals" ON)
option(AMOEBA_BUILD_TESTS "Build the standalone DSP tests" ON)
set(MAX_SDK_BASE_DIR "" CACHE PATH "Path to a local Cycling '74 max-sdk-base checkout")

if(AMOEBA_BUILD_EXTERNALS)
    if(NOT MAX_SDK_BASE_DIR)
        set(_amoeba_sdk_candidates
            "${CMAKE_CURRENT_SOURCE_DIR}/source/max-sdk-base"
            "/Applications/max-sdk-main/source/max-sdk-base"
            "/Applications/max-sdk-main/build/_deps/max-sdk-base-src"
            "$ENV{HOME}/Documents/Max 9/Packages/max-sdk/source/max-sdk-base"
            "$ENV{HOME}/Documents/Max 8/Packages/max-sdk/source/max-sdk-base"
        )

        foreach(_candidate IN LISTS _amoeba_sdk_candidates)
            if(EXISTS "${_candidate}/script/max-pretarget.cmake")
                set(MAX_SDK_BASE_DIR "${_candidate}")
                message(STATUS "Using local max-sdk-base: ${MAX_SDK_BASE_DIR}")
                break()
            endif()
        endforeach()

        if(NOT MAX_SDK_BASE_DIR)
            include(FetchContent)
            message(STATUS "Fetching Cycling '74 max-sdk-base")
            FetchContent_Declare(
                max_sdk_base
                GIT_REPOSITORY https://github.com/Cycling74/max-sdk-base.git
                GIT_TAG main
                GIT_SHALLOW TRUE
            )
            FetchContent_MakeAvailable(max_sdk_base)
            set(MAX_SDK_BASE_DIR "${max_sdk_base_SOURCE_DIR}")
        endif()
    endif()

    if(NOT EXISTS "${MAX_SDK_BASE_DIR}/script/max-pretarget.cmake")
        message(FATAL_ERROR "MAX_SDK_BASE_DIR does not contain max-sdk-base")
    endif()

    add_subdirectory(source/projects/amoeba.outputs.orbit~)
    add_subdirectory(source/projects/amoeba.outputs.swarm~)
    add_subdirectory(source/projects/amoeba.outputs.flow~)
    add_subdirectory(source/projects/amoeba.outputs.growth~)
    add_subdirectory(source/projects/amoeba.outputs.physarum~)
    add_subdirectory(source/projects/amoeba.outputs.membrane~)
    add_subdirectory(source/projects/amoeba.outputs.vortex~)
    add_subdirectory(source/projects/amoeba.outputs.brownian~)
    add_subdirectory(source/projects/amoeba.outputs.granular~)
    add_subdirectory(source/projects/amoeba.outputs.constellation~)
endif()

if(AMOEBA_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
