//
// Copyright (c) 2005, 2006 Wei Mingzhi <whistler@openoffice.org>
// Copyright (c) 2026 Todd Carnes <toddcarnes@gmail.com>
// All Rights Reserved.
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation; either version 2 of
// the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
// 02110-1301, USA
//

#ifndef MAIN_H_
#define MAIN_H_

#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <ctime>
#include <cmath>
#include <climits>
#include <cassert>
#include <format>
#if defined(__has_include) && __has_include(<print>)
#include <print>
#endif
#include <string>
#include <string_view>
#include <random>
#include <memory>
#include <atomic>

#if !defined(__cpp_lib_print)
namespace std {
   template<typename... Args>
   inline void println(std::FILE *stream, std::format_string<Args...> fmt, Args&&... args) {
      std::string s = std::format(fmt, std::forward<Args>(args)...);
      std::fputs(s.c_str(), stream);
      std::fputc('\n', stream);
   }

   template<typename... Args>
   inline void println(std::format_string<Args...> fmt, Args&&... args) {
      std::println(stdout, fmt, std::forward<Args>(args)...);
   }
}
#endif

#ifdef _WIN32
#include <io.h>
#include <windows.h>
#else
#include <unistd.h>
#endif

#include <SDL3/SDL.h>

#ifndef PREFIX
#define PREFIX "./"
#endif

#ifndef DATA_ROOT
#define DATA_ROOT PREFIX
#endif

#ifndef DATA_DIR
#define DATA_DIR DATA_ROOT "data/"
#endif

#ifndef CONFIG_FILE
#define CONFIG_FILE DATA_DIR "sdlhana.ini"
#endif

#ifndef SOUND_DIR
#define SOUND_DIR DATA_ROOT "sound/"
#endif

#ifndef IMAGES_DIR
#define IMAGES_DIR DATA_ROOT "images/"
#endif

#ifndef FONTS_DIR
#define FONTS_DIR DATA_ROOT "fonts/"
#endif

#include <filesystem>
#include "ini.h"

class CGeneral;
class CGame;

class Application {
public:
   Application();
   ~Application();
   static Application &GetInstance();

   SDL_Window               *window = nullptr;
   SDL_Renderer             *renderer = nullptr;
   SDL_Surface              *screen = nullptr;
   SDL_Texture              *screenTexture = nullptr;
   bool                      noSound = false;
   CIniFile                  config;
   std::unique_ptr<CGeneral> general;
   std::unique_ptr<CGame>    game;
};

inline Application& App() { return Application::GetInstance(); }
inline SDL_Window*& Window() { return App().window; }
inline SDL_Renderer*& Renderer() { return App().renderer; }
inline SDL_Surface*& Screen() { return App().screen; }
inline SDL_Texture*& ScreenTexture() { return App().screenTexture; }
inline bool& NoSound() { return App().noSound; }
inline CIniFile& Config() { return App().config; }
inline std::unique_ptr<CGeneral>& General() { return App().general; }
inline std::unique_ptr<CGame>& Game() { return App().game; }

// Recommended for new code: pass dependencies explicitly or via AppContext:
//   struct AppContext { SDL_Window* window; SDL_Renderer* renderer; SDL_Surface* screen; CGeneral* general; CGame* game; CIniFile* config; };
// Keeps leaf functions testable and avoids hidden macro coupling.

// Deprecated macro aliases — prefer inline accessors above for type safety
// and testability. Kept for incremental migration.
#define g_App (Application::GetInstance())
#define gpWindow (Application::GetInstance().window)
#define gpRenderer (Application::GetInstance().renderer)
#define gpScreen (Application::GetInstance().screen)
#define gpScreenTexture (Application::GetInstance().screenTexture)
#define g_fNoSound (Application::GetInstance().noSound)
#define cfg (Application::GetInstance().config)
#define gpGeneral (Application::GetInstance().general)
#define gpGame (Application::GetInstance().game)

// main.cpp functions...
void UserQuit();
std::filesystem::path GetUserConfigPath();

// util.cpp functions...
void trim(char *str);
char *va(const char *format, ...);
int RandomLong(int from, int to);
float RandomFloat(float from, float to);
int log2(int val);
void TerminateOnError(const char *fmt, ...);

char *UTIL_StrGetLine(const char *buf, int width, int &length);
unsigned int UTIL_GetPixel(SDL_Surface *surface, int x, int y);
void UTIL_PutPixel(SDL_Surface *surface, int x, int y, unsigned int pixel);
int UTIL_GetPixel(SDL_Surface *f, int x, int y, unsigned char *r, unsigned char *g, unsigned char *b);
int UTIL_PutPixel(SDL_Surface *f, int x, int y, unsigned char r, unsigned char g, unsigned char b);
int UTIL_PutPixelAlpha(SDL_Surface *f, int x, int y, unsigned char r, unsigned char g, unsigned char b, unsigned char a);
void UTIL_RevertSurfaceX(SDL_Surface *s);
void UTIL_RevertSurfaceY(SDL_Surface *s);
void UTIL_RevertSurfaceXY(SDL_Surface *s);
SDL_Surface *UTIL_ScaleSurface(SDL_Surface *s, int w, int h);
int UTIL_ScaleBlit(SDL_Surface *src, SDL_Rect *sr, SDL_Surface *dst, SDL_Rect *dr);
void UTIL_Scale2X(SDL_Surface *src, SDL_Surface *dst);
void UTIL_HorzLine(SDL_Surface *surface, short x, short y, short l, unsigned char r, unsigned char g, unsigned char b);
void UTIL_VertLine(SDL_Surface *surface, short x, short y, short l, unsigned char r, unsigned char g, unsigned char b);
void UTIL_Rect(SDL_Surface *surface, int x1, int y1, int w, int h, int r, int g, int b);
void UTIL_VertLineShade(SDL_Surface *surface, short x, short y, short l, unsigned char r1, unsigned char g1, unsigned char b1, unsigned char r2, unsigned char g2, unsigned char b2);
void UTIL_HorzLineShade(SDL_Surface *surface, short x, short y, short l, unsigned char r1, unsigned char g1, unsigned char b1, unsigned char r2, unsigned char g2, unsigned char b2);
void UTIL_RectShade(SDL_Surface *surface, int x, int y, int w, int h, int r1, int g1, int b1, int r2, int g2, int b2, int rt, int gt, int bt);
void UTIL_FillRect(SDL_Surface *surface, int x, int y, int w, int h, int r, int g, int b);
void UTIL_FillRectAlpha(SDL_Surface *surface, int x, int y, int w, int h, int r, int g, int b, int a);
void UTIL_Delay(int duration);
void UTIL_ToggleFullScreen();

// text.cpp functions...
void InitTextMessage();
void FreeTextMessage();
const char *msg(const char *name);

// compress.cpp functions...
int Decode(const char *filename, int headersize, unsigned char *buffer, int bufsize);
int Encode(const char *filename, unsigned char *header, int headersize, unsigned char *buffer, int bufsize);

// config.cpp functions...
void LoadCfg();
void SaveCfg();

#include <SDL3/SDL.h>
#include <SDL3_mixer/SDL_mixer.h>

struct SoundSample {
   MIX_Audio *audio = nullptr;
};

// sound.cpp functions...
extern std::atomic<bool> g_fAudioOpened; // main-thread flag, atomic for visibility
int SOUND_OpenAudio(int freq, int format, int channels, int samples);
void SOUND_FillAudio(void *udata, unsigned char *stream, int len);
void SOUND_PlayWAV(SoundSample *audio);
void SOUND_FreeWAV(SoundSample *audio);
SoundSample *SOUND_LoadWAV(const char *filename);

#include "font.h"
#include "general.h"
#include "card.h"
#include "player.h"
#include "game.h"
#include "bot.h"

#endif
