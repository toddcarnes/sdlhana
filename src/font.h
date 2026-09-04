//
// Copyright (c) 2005, 2006 Wei Mingzhi <whistler@openoffice.org>
// Copyright (c) 2026 Todd Carnes <toddcarnes@gmail.com>
// All Rights Reserved.
//

#ifndef FNTFILE_H
#define FNTFILE_H

#include <SDL3_ttf/SDL_ttf.h>

class CFont
{
public:
   CFont();
   explicit CFont(const char *filename);
   ~CFont();

   bool IsLoaded() const { return m_pTTFFont != nullptr; }

   int Load(const char *filename = nullptr);
   SDL_Surface *Render(const char *sz, int r = 255, int g = 255, int b = 255, int size = 32, bool shadow = true);
   SDL_Surface *RenderWrapped(const char *sz, int r = 255, int g = 255, int b = 255, int size = 32, int wrap_width = 500);

private:
   void FreeAllTheStuff();

   TTF_Font *m_pTTFFont = nullptr;
};

#endif
