cmake_minimum_required(VERSION 3.25)
project(sdlhana VERSION 0.34 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Fetch Dependencies via FetchContent
include(FetchContent)

# 1. Fetch SDL2 (required backend for sdl12-compat)
set(BUILD_SHARED_LIBS ON CACHE BOOL "Build shared libraries" FORCE)
set(SDL_SHARED ON CACHE BOOL "Enable SDL2 shared library" FORCE)
set(SDL_STATIC OFF CACHE BOOL "Disable SDL2 static library" FORCE)
set(SDL_TEST OFF CACHE BOOL "Disable SDL2 tests" FORCE)

FetchContent_Declare(
    SDL2
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG        release-2.30.12
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(SDL2)

# Supply SDL2 variables required by sdl12-compat's find_package(SDL2)
set(SDL2_INCLUDE_DIR "${sdl2_SOURCE_DIR}/include" CACHE PATH "SDL2 Include Dir" FORCE)
set(SDL2_INCLUDE_DIRS "${sdl2_SOURCE_DIR}/include;${sdl2_BINARY_DIR}/include" CACHE PATH "SDL2 Include Dirs" FORCE)
set(SDL2_LIBRARY SDL2 CACHE STRING "SDL2 Library" FORCE)
set(SDL2_LIBRARIES SDL2 CACHE STRING "SDL2 Libraries" FORCE)
set(SDL2_FOUND TRUE CACHE BOOL "SDL2 Found" FORCE)

# 2. Fetch sdl12-compat (SDL 1.2 API translation layer)
set(SDL12_TESTS OFF CACHE BOOL "Disable sdl12-compat tests" FORCE)

FetchContent_Declare(
    sdl12_compat
    GIT_REPOSITORY https://github.com/libsdl-org/sdl12-compat.git
    GIT_TAG        main
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(sdl12_compat)

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

add_executable(sdlhana ${SDLHANA_SOURCES})

# Target Include Directories
target_include_directories(sdlhana PRIVATE
    src
    ${sdl12_compat_SOURCE_DIR}/include/SDL
    ${sdl12_compat_SOURCE_DIR}/include
    ${sdl2_SOURCE_DIR}/include
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

# Link against sdl12-compat and SDLmain (handles WinMain -> SDL_main redirection)
if(TARGET SDLmain)
    target_link_libraries(sdlhana PRIVATE SDLmain SDL)
elseif(TARGET SDL::SDL)
    target_link_libraries(sdlhana PRIVATE SDLmain SDL::SDL)
else()
    target_link_libraries(sdlhana PRIVATE SDLmain sdl12_compat)
endif()

# Copy asset directories, SDL.dll, and SDL2.dll into executable build directory post-build
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
            $<TARGET_FILE:SDL>
            $<TARGET_FILE_DIR:sdlhana>/SDL.dll
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:SDL2>
            $<TARGET_FILE_DIR:sdlhana>/$<TARGET_FILE_NAME:SDL2>
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:SDL2>
            $<TARGET_FILE_DIR:sdlhana>/SDL2.dll
    COMMENT "Copying game assets, SDL.dll, and SDL2.dll to target build output directory..."
)
