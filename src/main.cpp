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

#include <SDL3/SDL_main.h>
#include <SDL3_ttf/SDL_ttf.h>
#include "main.h"
#include "sdlhana.xpm"

static void SetAppWindowIcon(SDL_Window *window)
{
   if (window == nullptr) return;

   SDL_Surface *icon = SDL_CreateSurface(32, 32, SDL_PIXELFORMAT_RGBA32);
   if (icon == nullptr) return;

   if (SDL_MUSTLOCK(icon)) {
      SDL_LockSurface(icon);
   }

   Uint8 *pixels = (Uint8 *)icon->pixels;
   for (int y = 0; y < 32; y++) {
      const char *line = sdlhana_xpm[7 + y];
      for (int x = 0; x < 32; x++) {
         char c = line[x];
         Uint8 r = 0, g = 0, b = 0, a = 255;
         switch (c) {
            case '.': r = 0;   g = 128; b = 0;   break;
            case '+': r = 0;   g = 0;   b = 0;   break;
            case '@': r = 255; g = 0;   b = 0;   break;
            case '#': r = 255; g = 255; b = 255; break;
            case '$': r = 0;   g = 255; b = 0;   break;
            case ' ': default: a = 0;            break;
         }
         Uint8 *p = pixels + y * icon->pitch + x * 4;
         p[0] = r;
         p[1] = g;
         p[2] = b;
         p[3] = a;
      }
   }

   if (SDL_MUSTLOCK(icon)) {
      SDL_UnlockSurface(icon);
   }

   SDL_SetWindowIcon(window, icon);
   SDL_DestroySurface(icon);
}

Application::Application() = default;
Application::~Application() = default;

Application &Application::GetInstance()
{
   static Application instance;
   return instance;
}

std::filesystem::path GetUserConfigPath()
{
   char *pref_path = SDL_GetPrefPath("CarneSoft", "SDLHana");
   std::filesystem::path path;
   if (pref_path != nullptr) {
      path = std::filesystem::path(pref_path) / "sdlhana.ini";
      SDL_free(pref_path);
   } else {
      path = std::filesystem::path("sdlhana.ini");
   }
   return path;
}

void LoadCfg()
{
   std::filesystem::path user_cfg = GetUserConfigPath();
   if (cfg.Load(user_cfg.string().c_str()) != 0) {
      cfg.Load(DATA_DIR "sdlhana.ini"); // load default config file if user config does not exist
   }
}

void SaveCfg()
{
   std::filesystem::path user_cfg = GetUserConfigPath();
   if (user_cfg.has_parent_path()) {
      std::filesystem::create_directories(user_cfg.parent_path());
   }
   cfg.Save(user_cfg.string().c_str());
}

static bool SDLCALL EventFilter(void *userdata, SDL_Event *event)
{
   (void)userdata;
   if (event->type == SDL_EVENT_KEY_DOWN) {
      if (event->key.key == SDLK_RETURN && (event->key.mod & SDL_KMOD_ALT)) {
         UTIL_ToggleFullScreen();
         return false;
      } else if (event->key.key == SDLK_ESCAPE) {
         UserQuit();
         return false;
      }
   } else if (event->type == SDL_EVENT_QUIT) {
      UserQuit();
      return false;
   }

   return true;
}

int main(int argc, char *argv[])
{
   (void)argc;
   (void)argv;

   const char *base_path = SDL_GetBasePath();
   if (base_path != nullptr) {
      std::filesystem::current_path(base_path);
   }

   LoadCfg(); // load the configuration file

   // Initialize SDL3 defaults, video and audio
   if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) { 
      std::println(stderr, "FATAL ERROR: Could not initialize SDL3: {}.", SDL_GetError());
      exit(1);
   }

   cfg.Set("OPTIONS", "FullScreen", "0");
   SDL_WindowFlags window_flags = SDL_WINDOW_RESIZABLE;

   gpWindow = SDL_CreateWindow("SDLHana", 1024, 768, window_flags);
   if (gpWindow == nullptr) {
      std::println(stderr, "FATAL ERROR: Could not create SDL3 window: {}", SDL_GetError());
      exit(1);
   }
   SetAppWindowIcon(gpWindow);

   gpRenderer = SDL_CreateRenderer(gpWindow, NULL);
   if (gpRenderer == nullptr) {
      std::println(stderr, "FATAL ERROR: Could not create SDL3 renderer: {}", SDL_GetError());
      exit(1);
   }

   SDL_SetRenderLogicalPresentation(gpRenderer, 640, 480, SDL_LOGICAL_PRESENTATION_STRETCH);

   gpScreen = SDL_CreateSurface(640, 480, SDL_PIXELFORMAT_XRGB8888);
   if (gpScreen == nullptr) {
      std::println(stderr, "FATAL ERROR: Could not create main screen surface: {}", SDL_GetError());
      exit(1);
   }
   gpScreenTexture = nullptr;

   g_fNoSound = (atoi(cfg.Get("OPTIONS", "NoSound", "0")) > 0);

   // Open the audio device
   if (!g_fNoSound) {
      if (SOUND_OpenAudio(22050, SDL_AUDIO_S16, 1, 1024)) {
         std::println(stderr, "WARNING: Couldn't open audio: {}", SDL_GetError());
         g_fNoSound = true;
      }
   }

   InitTextMessage();

   gpGeneral = std::make_unique<CGeneral>();
   gpGame = std::make_unique<CGame>();

   SDL_SetEventFilter(EventFilter, NULL);
   gpGame->MainMenu();

   UserQuit();

   return 0;
}

void UserQuit()
{
   gpGame.reset();
   gpGeneral.reset();

   if (gpScreenTexture != nullptr) {
      SDL_DestroyTexture(gpScreenTexture);
      gpScreenTexture = nullptr;
   }

   if (gpScreen != nullptr) {
      SDL_DestroySurface(gpScreen);
      gpScreen = nullptr;
   }

   if (gpRenderer != nullptr) {
      SDL_DestroyRenderer(gpRenderer);
      gpRenderer = nullptr;
   }

   if (gpWindow != nullptr) {
      SDL_DestroyWindow(gpWindow);
      gpWindow = nullptr;
   }

   SDL_Quit();

   FreeTextMessage();
   SaveCfg();

   exit(0);
}

