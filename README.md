# SDLHana

[![Windows Build](https://github.com/toddcarnes/sdlhana/actions/workflows/build-windows.yml/badge.svg)](https://github.com/toddcarnes/sdlhana/actions/workflows/build-windows.yml)
[![macOS Build](https://github.com/toddcarnes/sdlhana/actions/workflows/build-macos.yml/badge.svg)](https://github.com/toddcarnes/sdlhana/actions/workflows/build-macos.yml)
[![Linux Build](https://github.com/toddcarnes/sdlhana/actions/workflows/build-linux.yml/badge.svg)](https://github.com/toddcarnes/sdlhana/actions/workflows/build-linux.yml)
[![Release Packaging](https://github.com/toddcarnes/sdlhana/actions/workflows/release.yml/badge.svg)](https://github.com/toddcarnes/sdlhana/actions/workflows/release.yml)
[![Latest Release](https://img.shields.io/github/v/release/toddcarnes/sdlhana)](https://github.com/toddcarnes/sdlhana/releases/latest)

SDLHana is a modern C++23 / SDL3 cross-platform Hanafuda game. Hanafuda is a traditional Japanese card game played in Japan and Korea (where it is known as *"Hwa-T'u"*).

---

## Game Modes & Supported Languages

### Supported Game Modes

- **Koi-Koi (こいこい)**: The traditional Japanese 2-player Hanafuda game. Form combination yaku and choose to call *"Koi-Koi"* to double your points or end the hand early.
- **Go-Stop (고스톱)**: The popular Korean Hanafuda variant featuring Korean scoring rules (*"Sang-Puh"*, *"Leave Three"*), multipliers, and double-up opportunities.
- **Bet Mode**: Fast-paced scoring round mode.

### Supported Languages

The game interface supports full real-time language switching in the Settings menu:

- **English** (`eng`)
- **Japanese** (`jpn` / 日本語)
- **Simplified Chinese** (`chs` / 简体中文)
- **French** (`fra` / Français)

---

## Acknowledgments & Project History

This project is a modern C++23 / SDL3 refactor and evolution of the original SDLHana codebase created by Wei Mingzhi and hosted on GNU Savannah:

- **Original Project Homepage**: [https://savannah.nongnu.org/projects/sdlhana/](https://savannah.nongnu.org/projects/sdlhana/)

---

## Key Features & Modernization

- **Modern C++23 Architecture**: Built with standard library features (`<print>`, `<format>`, `<filesystem>`, `std::optional`, `std::unique_ptr`).
- **Hardware-Accelerated SDL3**: Modern rendering engine using `SDL_Renderer` and `SDL_Texture`, paired with `SDL3_mixer` for audio and `SDL3_ttf` for font rendering.
- **CMake 3.25+ Build System**: Clean FetchContent integration that automatically resolves and builds dependencies across all operating systems.
- **Cross-Platform Compatibility**: Tested and verified independently on **Windows 11 (MSVC)**, **macOS (Clang)**, and **Linux (GCC)**.

---

## Building from Source

### Prerequisites

- [CMake](https://cmake.org/) 3.25 or newer
- C++23 compatible compiler (Visual Studio 2026 / MSVC, GCC 13+, or Clang 16+)

### Build Instructions

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

The compiled binary (`sdlhana` or `sdlhana.exe`) will be generated inside the `build/` directory alongside all required game assets (`data/`, `images/`, `fonts/`, `sound/`).

### Optional Developer Tools

To build the font generation and character list utilities (`title2cl`), pass `-DSDLHANA_BUILD_TOOLS=ON` to CMake:

```bash
cmake -B build -DSDLHANA_BUILD_TOOLS=ON
cmake --build build --target title2cl
```

---

## Downloads & Standalone Releases

Pre-compiled, self-contained standalone game bundles for **Windows**, **macOS**, and **Linux** are automatically built and published on the [GitHub Releases Page](https://github.com/toddcarnes/sdlhana/releases).
