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

Add a new language without recompiling by dropping `data/i18n/<code>.json` (copy `eng.json`, translate values with `{}` placeholders) — it appears automatically in Settings via filesystem autodiscovery.

---

## Acknowledgments & Project History

This project is a modern C++23 / SDL3 refactor and evolution of the original SDLHana codebase created by Wei Mingzhi and hosted on GNU Savannah:

- **Original Project Homepage**: [https://savannah.nongnu.org/projects/sdlhana/](https://savannah.nongnu.org/projects/sdlhana/)

---

## Game Rules & Documentation

- **[Hanafuda Rules & How to Play Guide (`RULES.md`)](RULES.md)**: Detailed guide covering deck structure, *Mekuri* dealer selection, Japanese Koi-Koi Yaku combinations, Korean Go-Stop scoring/penalties, and Bet Mode.
- **[Project ChangeLog (`ChangeLog`)](ChangeLog)**: Detailed record of all modern C++23/SDL3 refactoring updates, features, and fixes.

---

## Key Features & Modernization

### Authentic Game Mechanics & Rules

- **In-Game Scrollable Rules Viewer**: Full offscreen canvas rules viewer displaying 100% of the text, month-by-month card breakdowns, Yaku combinations, and card sprite illustrations from `RULES.md` with interactive scrollbar dragging, mouse wheel, keyboard arrows, and viewport click navigation.
- **Traditional *Mekuri* Dealer Selection**: First-round dealer selection (*Oya* / *Sun*) uses authentic random card draws, selecting the lowest month card (with card tier breaking ties).
- **Winner-Becomes-Dealer Retention**: The winner of each round becomes dealer for the subsequent round (or current dealer retains deal on draws), restoring traditional Koi-Koi and Go-Stop rules.
- **12-Round Match System**: Full 12-round match progression (Jan to Dec) with match victory screens and score tracking.
- **Authentic Japanese Koi-Koi Rules**: Full Yaku combinations (Lights, Ribbons, Ino-Shika-Chou, Animals, Kasu, Sake Cup) with Koi-Koi call multipliers.
- **Authentic Korean Go-Stop Rules**: 3-point threshold, Godori, Hongdan, Cheongdan, Chodan, Ssangpi double junks, initial triplets (*Heun-deul-im*), Go multipliers, and *Gwang-bak* / *Pi-bak* penalty multipliers.

### Technical & Engine Modernization

- **Modern C++23 Architecture**: Built with standard library features (`<print>`, `<format>`, `<filesystem>`, `std::optional`, `std::unique_ptr`).
- **Hardware-Accelerated SDL3**: Modern rendering engine using `SDL_Renderer` and `SDL_Texture`, paired with `SDL3_mixer` for audio and `SDL3_ttf` for font rendering.
- **Cross-Platform Auto-Font & Scaling Engine**: TTF-only rendering via `SDL3_ttf` with dynamic system CJK vector font auto-detection across Windows, macOS, and Linux, paired with high-DPI resizable window scaling.
- **Audio Volume Control**: Adjustable 0–100% master gain slider in Settings (grayed when sound disabled) via `MIX_SetMixerGain`, persisted in `sdlhana.ini`.
- **CMake 3.25+ Build System**: Clean FetchContent integration that automatically resolves and builds dependencies across all operating systems.
- **Cross-Platform Compatibility**: Tested and verified on **Windows 11 (MSVC)**, **macOS (Clang)**, and **Linux (GCC)**.

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

The compiled binary (`sdlhana` or `sdlhana.exe`) will be generated inside the `build/` directory alongside all required game assets (`data/`, `images/`, `sound/`).

### Assets

Card artwork references are preserved in `assets/cards_trad/` for documentation purposes.

---

## Downloads & Standalone Releases

Pre-compiled, self-contained standalone game bundles for **Windows**, **macOS**, and **Linux** are automatically built and published on the [GitHub Releases Page](https://github.com/toddcarnes/sdlhana/releases).
