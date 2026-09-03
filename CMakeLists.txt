cmake_minimum_required(VERSION 3.25)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5 CACHE STRING "" FORCE)
project(sdlhana VERSION 1.0.2 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(APPLE)
    set(CMAKE_OSX_ARCHITECTURES "x86_64;arm64" CACHE STRING "Build architectures for macOS" FORCE)
endif()

# Fetch Dependencies via FetchContent
include(FetchContent)

# 1. Fetch SDL3 (native hardware rendering framework)
set(SDL_SHARED ON CACHE BOOL "Enable SDL3 shared library" FORCE)
set(SDL_STATIC OFF CACHE BOOL "Disable SDL3 static library" FORCE)
set(SDL_TEST OFF CACHE BOOL "Disable SDL3 tests" FORCE)
set(SDL_X11_XTEST OFF CACHE BOOL "Disable X11 XTest support" FORCE)
set(SDL_JOYSTICK_GAMEINPUT OFF CACHE BOOL "Disable GameInput driver" FORCE)
set(SDL_LIBC ON CACHE BOOL "Use standard C library" FORCE)

FetchContent_Declare(
    SDL3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG        main
    GIT_SHALLOW    TRUE
)

# 2. Fetch SDL3_mixer (WAV PCM playback)
set(SDLMIXER_FLAC OFF CACHE BOOL "" FORCE)
set(SDLMIXER_GME OFF CACHE BOOL "" FORCE)
set(SDLMIXER_MOD OFF CACHE BOOL "" FORCE)
set(SDLMIXER_MP3 OFF CACHE BOOL "" FORCE)
set(SDLMIXER_MIDI OFF CACHE BOOL "" FORCE)
set(SDLMIXER_OPUS OFF CACHE BOOL "" FORCE)
set(SDLMIXER_WAVPACK OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    SDL3_mixer
    GIT_REPOSITORY https://github.com/libsdl-org/SDL_mixer.git
    GIT_TAG        release-3.2.4
    GIT_SHALLOW    TRUE
)

set(SDLTTF_VENDORED ON CACHE BOOL "Use vendored dependencies for SDL3_ttf" FORCE)
set(SDL3TTF_VENDORED ON CACHE BOOL "Use vendored dependencies for SDL3_ttf" FORCE)

FetchContent_Declare(
    SDL3_ttf
    GIT_REPOSITORY https://github.com/libsdl-org/SDL_ttf.git
    GIT_TAG        release-3.2.2
    GIT_SHALLOW    TRUE
)

FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG        v3.11.3
    GIT_SHALLOW    TRUE
)

FetchContent_MakeAvailable(SDL3 SDL3_mixer SDL3_ttf nlohmann_json)

message(STATUS "SDL3_mixer SOURCE_DIR: ${sdl3_mixer_SOURCE_DIR}")
message(STATUS "SDL3_ttf SOURCE_DIR: ${sdl3_ttf_SOURCE_DIR}")

# Source files
set(SDLHANA_SOURCES
    src/main.cpp
    src/bot.cpp
    src/card.cpp
    src/compress.cpp
    src/font.cpp
    src/game.cpp
    src/general.cpp
    src/ini.cpp
    src/player.cpp
    src/sound.cpp
    src/text.cpp
    src/util.cpp
)

if(WIN32)
    list(APPEND SDLHANA_SOURCES src/sdlhana.rc)
    add_executable(sdlhana WIN32 ${SDLHANA_SOURCES})
elseif(APPLE)
    add_executable(sdlhana MACOSX_BUNDLE ${SDLHANA_SOURCES})
    set_target_properties(sdlhana PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_BUNDLE_NAME "SDLHana"
        MACOSX_BUNDLE_GUI_IDENTIFIER "com.carnesoft.sdlhana"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "1.0.2"
        MACOSX_BUNDLE_BUNDLE_VERSION "1.0.2"
        MACOSX_BUNDLE_ICON_FILE "sdlhana.icns"
        MACOSX_RPATH TRUE
        BUILD_WITH_INSTALL_RPATH TRUE
        INSTALL_RPATH "@executable_path;@loader_path"
    )
else()
    add_executable(sdlhana ${SDLHANA_SOURCES})
endif()

# Target Include Directories
target_include_directories(sdlhana PRIVATE
    src
    ${sdl3_mixer_SOURCE_DIR}/include
    ${sdl3_ttf_SOURCE_DIR}/include
    ${CMAKE_CURRENT_BINARY_DIR}/_deps/sdl3_mixer-src/include
    ${CMAKE_CURRENT_BINARY_DIR}/_deps/sdl3_ttf-src/include
    ${nlohmann_json_SOURCE_DIR}/include
    ${nlohmann_json_SOURCE_DIR}/single_include
)

# Compiler definitions for legacy path resolutions and compatibility
target_compile_definitions(sdlhana PRIVATE
    DATA_ROOT="./"
    _CRT_SECURE_NO_WARNINGS
    _CRT_NONSTDC_NO_WARNINGS
)

# Strict C++ compiler flags for MSVC, GCC, and Clang (Windows/Linux/macOS parity)
if(MSVC)
    target_compile_options(sdlhana PRIVATE /permissive- /W4)
else()
    target_compile_options(sdlhana PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Link against native SDL3, SDL3_mixer, and SDL3_ttf
if(TARGET SDL3::SDL3_main)
    target_link_libraries(sdlhana PRIVATE SDL3::SDL3 SDL3::SDL3_main SDL3_mixer::SDL3_mixer-shared SDL3_ttf::SDL3_ttf-shared)
elseif(TARGET SDL3_main)
    target_link_libraries(sdlhana PRIVATE SDL3::SDL3 SDL3_main SDL3_mixer::SDL3_mixer-shared SDL3_ttf::SDL3_ttf-shared)
else()
    target_link_libraries(sdlhana PRIVATE SDL3::SDL3 SDL3_mixer::SDL3_mixer-shared SDL3_ttf::SDL3_ttf-shared)
endif()

# Copy asset directories and SDL3.dll into executable build directory post-build
# src/data copy includes data/i18n/*.json (autodiscovered languages)
add_custom_command(TARGET sdlhana POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/src/data
            $<TARGET_FILE_DIR:sdlhana>/data
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/src/data/i18n
            $<TARGET_FILE_DIR:sdlhana>/data/i18n
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/src/images
            $<TARGET_FILE_DIR:sdlhana>/images
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/src/sound
            $<TARGET_FILE_DIR:sdlhana>/sound
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/src/fonts
            $<TARGET_FILE_DIR:sdlhana>/fonts
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:SDL3::SDL3>
            $<TARGET_FILE_DIR:sdlhana>/$<TARGET_FILE_NAME:SDL3::SDL3>
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:SDL3_mixer::SDL3_mixer-shared>
            $<TARGET_FILE_DIR:sdlhana>/$<TARGET_FILE_NAME:SDL3_mixer::SDL3_mixer-shared>
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:SDL3_ttf::SDL3_ttf-shared>
            $<TARGET_FILE_DIR:sdlhana>/$<TARGET_FILE_NAME:SDL3_ttf::SDL3_ttf-shared>
    COMMENT "Copying game assets and SDL3 DLLs to target build output directory..."
)

# Optional developer tools build
option(SDLHANA_BUILD_TOOLS "Build developer font tools (title2cl)" OFF)
if(SDLHANA_BUILD_TOOLS)
    add_subdirectory(tools)
endif()
