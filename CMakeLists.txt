cmake_minimum_required(VERSION 3.25)
project(sdlhana VERSION 0.34 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Fetch Dependencies via FetchContent
include(FetchContent)

# 1. Fetch SDL3 (native hardware rendering framework)
set(SDL_SHARED ON CACHE BOOL "Enable SDL3 shared library" FORCE)
set(SDL_STATIC OFF CACHE BOOL "Disable SDL3 static library" FORCE)
set(SDL_TEST OFF CACHE BOOL "Disable SDL3 tests" FORCE)
set(SDL_JOYSTICK_GAMEINPUT OFF CACHE BOOL "Disable GameInput driver" FORCE)
set(SDL_LIBC ON CACHE BOOL "Use standard C library" FORCE)

FetchContent_Declare(
    SDL3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG        main
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(SDL3)

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
    add_executable(sdlhana WIN32 ${SDLHANA_SOURCES})
elseif(APPLE)
    add_executable(sdlhana MACOSX_BUNDLE ${SDLHANA_SOURCES})
else()
    add_executable(sdlhana ${SDLHANA_SOURCES})
endif()

# Target Include Directories
target_include_directories(sdlhana PRIVATE
    src
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

# Link against native SDL3
if(TARGET SDL3::SDL3_main)
    target_link_libraries(sdlhana PRIVATE SDL3::SDL3 SDL3::SDL3_main)
elseif(TARGET SDL3_main)
    target_link_libraries(sdlhana PRIVATE SDL3::SDL3 SDL3_main)
else()
    target_link_libraries(sdlhana PRIVATE SDL3::SDL3)
endif()

# Copy asset directories and SDL3.dll into executable build directory post-build
add_custom_command(TARGET sdlhana POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/src/data
            $<TARGET_FILE_DIR:sdlhana>/data
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
    COMMENT "Copying game assets and SDL3.dll to target build output directory..."
)
