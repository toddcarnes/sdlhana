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

#include "main.h"

CFont::CFont():
m_iNumChar(0),
m_pChars(NULL),
m_pTTFFont(NULL)
{
}

CFont::CFont(const char *filename):
m_iNumChar(0),
m_pChars(NULL),
m_pTTFFont(NULL)
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

   int useTTF = atoi(cfg.Get("OPTIONS", "UseTTF", "0"));
   const char *ttfPath = cfg.Get("OPTIONS", "TTFFontPath", "");

   if (useTTF && ttfPath != nullptr && strlen(ttfPath) > 0) {
      m_pTTFFont = TTF_OpenFont(ttfPath, 32.0f);
      if (m_pTTFFont != nullptr) {
         return 0;
      }
      std::println(stderr, "WARNING: Could not open TTF font {}, falling back to bitmap font", ttfPath);
   }

   FILE *fp = fopen(filename, "rb");
   if (fp == NULL) {
      TerminateOnError("Cannot load font file %s", filename);
   }

   fntfile_header_t header;

   fread(&header, 1, sizeof(fntfile_header_t), fp);

   if (strcmp(header.magic, "FNT") != 0) {
      fclose(fp);
      TerminateOnError("Invalid font file %s", filename);
   }

   m_iNumChar = header.num_chars;

   m_pChars = (fntchar_t *)malloc(sizeof(fntchar_t) * header.num_chars);
   if (m_pChars == NULL) {
      TerminateOnError("Memory allocation Error!");
   }

   fclose(fp);

   // Decompress the font data
   Decode(filename, sizeof(fntfile_header_t), (unsigned char *)m_pChars, sizeof(fntchar_t) * header.num_chars);

   Sort(); // sort the font data
   return 0;
}

SDL_Surface *CFont::Render(const char *sz, int r, int g, int b, int size, bool shadow)
{
   if (m_pTTFFont != nullptr) {
      TTF_SetFontSize(m_pTTFFont, (float)size);
      SDL_Color color = { (Uint8)r, (Uint8)g, (Uint8)b, 255 };
      SDL_Surface *surface = TTF_RenderText_Blended(m_pTTFFont, sz, 0, color);
      if (surface == nullptr) {
         std::println(stderr, "WARNING: TTF_RenderText_Blended failed: {}", SDL_GetError());
      }
      return surface;
   }
   int length = 0, i, j, cur = 0;
   const char *p = sz;
   SDL_Surface *s = NULL;

   while (*p) {
      if (*p < 0) {
         p += 3;
         length += 2;
      } else {
         p++;
         length++;
      }
   }

   s = SDL_CreateSurface(size / 2 * (length + 2), size, SDL_PIXELFORMAT_RGBA8888);

   SDL_SetSurfaceColorKey(s, true, SDL_MapSurfaceRGBA(s, 0, 0, 0, 0));
   SDL_FillSurfaceRect(s, NULL, SDL_MapSurfaceRGBA(s, 0, 0, 0, 0));

   // HACKHACK: to make black color not transparent
   if (r == 0 && g == 0 && b == 0) {
      r = 1;
   }

   p = sz;
   while (*p) {
      union {
         unsigned int i;
         char c[4];
      } code;
      code.i = 0;

      int bb = 1;
      if ((unsigned char)*p & 0x80) {
         // This is an multi-byte character
         // FIXME: four-or-more-byte sequences not supported
         if (((unsigned char)*p & 0xE0) == 0xC0) {
            // Two-byte sequence.
            code.c[0] = *p++;
            code.c[1] = *p++;
            code.c[2] = '\0';
            code.c[3] = '\0';
            bb = 2;
         } else if (((unsigned char)*p & 0xF0) == 0xE0) {
            // Three-byte sequence.
            code.c[0] = *p++;
            code.c[1] = *p++;
            code.c[2] = *p++;
            code.c[3] = '\0';
            bb = 3;
         } else {
            TerminateOnError("CFont::Render(): four-or-more-byte sequence not supported");
         }
      } else {
         // This is a normal ASCII character
         code.c[0] = *p++;
         code.c[1] = code.c[2] = code.c[3] = '\0';
         bb = 1;
      }

      fntchar_t *c = FindChar(code.i);

      if (c != NULL) {
         SDL_Rect dstrect;

         dstrect.x = cur;
         dstrect.y = 0;
         dstrect.w = size;
         dstrect.h = size;

         SDL_Surface *char_surface = SDL_CreateSurface(shadow ? 66 : 64, shadow ? 66 : 64, SDL_PIXELFORMAT_RGBA8888);

         SDL_SetSurfaceColorKey(char_surface, true, SDL_MapSurfaceRGBA(char_surface, 0, 0, 0, 0));
         SDL_FillSurfaceRect(char_surface, NULL, SDL_MapSurfaceRGBA(char_surface, 0, 0, 0, 0));

         if (shadow) {
            for (i = 0; i < 64; i++) {
               for (j = 0; j < 64; j++) {
                  if (c->pixeldata[i][j / 8] & (1 << (j & 7))) {
                     UTIL_PutPixel(char_surface, j + 2, i + 2, 1, 1, 1);
                  }
               }
            }
         }

         for (i = 0; i < 64; i++) {
            for (j = 0; j < 64; j++) {
               if (c->pixeldata[i][j / 8] & (1 << (j & 7))) {
                  UTIL_PutPixel(char_surface, j, i, (unsigned char)r, (unsigned char)g, (unsigned char)b);
               }
            }
         }

         SDL_BlitSurfaceScaled(char_surface, NULL, s, &dstrect, SDL_SCALEMODE_LINEAR);
         SDL_DestroySurface(char_surface);
      }

      cur += ((bb == 3) ? size : size / 2);
   }

   return s;
}

void CFont::Sort(fntchar_t *begin, fntchar_t *end)
{
   if (begin == NULL && end == NULL) {
      begin = m_pChars;
      end = &m_pChars[m_iNumChar - 1];
   }

   if (end <= begin)
      return;

   fntchar_t pivot = *begin, *a = begin, *b = end;

   while (a < b) {
      while (a < b && *b >= pivot)
         b--;
      *a = *b;
      while (a < b && *a <= pivot)
         a++;
      *b = *a;
   }

   *a = pivot;

   Sort(begin, a - 1);
   Sort(a + 1, end);
}

fntchar_t *CFont::FindChar(unsigned int code)
{
   int low = 0, high = m_iNumChar - 1;

   while (low <= high) {
      int mid = (low + high) / 2;
      if (m_pChars[mid].code < code) {
         low = mid + 1;
      } else if (m_pChars[mid].code > code) {
         high = mid - 1;
      } else {
         return &m_pChars[mid];
      }
   }

   return NULL; // not found
}

void CFont::FreeAllTheStuff()
{
   if (m_pTTFFont != nullptr) {
      TTF_CloseFont(m_pTTFFont);
      m_pTTFFont = nullptr;
   }
   if (m_pChars != NULL)
      free(m_pChars);
   m_pChars = NULL;
   m_iNumChar = 0;
}

