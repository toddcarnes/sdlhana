//
// Copyright (c) 2005, 2006 Wei Mingzhi <whistler@openoffice.org>
// Copyright (c) 2026 Todd Carnes <toddcarnes@gmail.com>
// All Rights Reserved.
//

#include "main.h"
#include <cstring>

CFont::CFont() = default;

CFont::CFont(const char *filename)
{
   Load(filename);
}

CFont::~CFont()
{
   FreeAllTheStuff();
}

int CFont::Load(const char *filename)
{
   if (IsLoaded()) {
      FreeAllTheStuff();
   }

   const char *ttfPath = Config().Get("OPTIONS", "TTFFontPath", "");

   if (filename != nullptr && std::strlen(filename) > 0 && std::filesystem::exists(filename)) {
      std::string fn = filename;
      if (fn.size() >= 4 && (fn.substr(fn.size() - 4) == ".ttf" || fn.substr(fn.size() - 4) == ".ttc")) {
         ttfPath = filename;
      }
   }

   if (ttfPath != nullptr && std::strlen(ttfPath) > 0) {
      m_pTTFFont = TTF_OpenFont(ttfPath, 32.0f);
      if (m_pTTFFont != nullptr) return 0;
      std::println(stderr, "WARNING: Could not open TTFFontPath {}: {}", ttfPath, SDL_GetError());
   }

   // Auto-detect cross-platform system CJK & vector fonts
   static const char *sysFonts[] = {
      "C:\\Windows\\Fonts\\msyh.ttc",       // Windows Microsoft YaHei
      "C:\\Windows\\Fonts\\msjh.ttc",       // Windows Microsoft JhengHei
      "C:\\Windows\\Fonts\\meiryo.ttc",     // Windows Meiryo
      "C:\\Windows\\Fonts\\arial.ttf",      // Windows Arial
      "C:\\Windows\\Fonts\\segoeui.ttf",    // Windows Segoe UI
      "/System/Library/Fonts/PingFang.ttc", // macOS PingFang
      "/System/Library/Fonts/STHeiti Light.ttc",
      "/System/Library/Fonts/Supplemental/Arial Unicode.ttf",
      "/usr/share/fonts/truetype/noto/NotoSansCJK-Regular.ttc",
      "/usr/share/fonts/opentype/noto/NotoSansCJK-Regular.ttc",
      "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf"
   };
   for (const char *path : sysFonts) {
      if (std::filesystem::exists(path)) {
         m_pTTFFont = TTF_OpenFont(path, 32.0f);
         if (m_pTTFFont != nullptr) {
            return 0;
         }
      }
   }

   TerminateOnError("Cannot load any TTF font. Please set TTFFontPath in sdlhana.ini");
   return -1;
}

SDL_Surface *CFont::RenderWrapped(const char *sz, int r, int g, int b, int size, int wrap_width)
{
   if (m_pTTFFont != nullptr) {
      TTF_SetFontSize(m_pTTFFont, (float)size);
      SDL_Color color = { (Uint8)r, (Uint8)g, (Uint8)b, 255 };
      SDL_Surface *surface = TTF_RenderText_Blended_Wrapped(m_pTTFFont, sz, 0, color, wrap_width);
      if (surface != nullptr) {
         return surface;
      }
   }
   std::println(stderr, "WARNING: TTF_RenderText_Blended_Wrapped failed: {}", SDL_GetError());
   return nullptr;
}

SDL_Surface *CFont::Render(const char *sz, int r, int g, int b, int size, bool shadow)
{
   (void)shadow;
   if (m_pTTFFont != nullptr) {
      TTF_SetFontSize(m_pTTFFont, (float)size);
      SDL_Color color = { (Uint8)r, (Uint8)g, (Uint8)b, 255 };
      SDL_Surface *surface = TTF_RenderText_Blended(m_pTTFFont, sz, 0, color);
      if (surface == nullptr) {
         std::println(stderr, "WARNING: TTF_RenderText_Blended failed: {}", SDL_GetError());
      }
      return surface;
   }
   return nullptr;
}

void CFont::FreeAllTheStuff()
{
   if (m_pTTFFont != nullptr) {
      TTF_CloseFont(m_pTTFFont);
      m_pTTFFont = nullptr;
   }
}
