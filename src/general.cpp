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

const char *soundfile[NUM_SOUND + 1] = {
   "card1",
   "card2",
   "card3",
   "win",
   "lose",
   "koi",
   "hint",
   "hint2",
   "",
};

CGeneral::CGeneral()
{
   m_fSndLoaded = false;
   LoadFonts();
   InitCursor();
   LoadImages();
   LoadSound();
}

CGeneral::~CGeneral()
{
   FreeCursor();
   FreeImages();
   FreeSound();
}

void CGeneral::ScreenFade(int duration, SDL_Surface *s)
{
   (void)duration;
   (void)s;
   if (gpRenderer != nullptr) {
      SDL_RenderPresent(gpRenderer);
   }
}

int CGeneral::ReadKey()
{
   SDL_Event event;

   while (1) {
      if (SDL_WaitEvent(&event)) {
         if (gpRenderer != nullptr) {
            SDL_ConvertEventToRenderCoordinates(gpRenderer, &event);
         }
         if (event.type == SDL_EVENT_KEY_DOWN) {
            return (int)event.key.key;
         } else if (event.type == SDL_EVENT_MOUSE_WHEEL) {
            if (event.wheel.y > 0.0f) {
               return (int)SDLK_UP;
            } else if (event.wheel.y < 0.0f) {
               return (int)SDLK_DOWN;
            }
         } else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
            if (event.button.button == SDL_BUTTON_RIGHT) {
               return SDLK_RIGHT;
            } else if (event.button.button == SDL_BUTTON_LEFT) {
               int id = CButton::GetButtonId((int)event.button.x, (int)event.button.y);
               if (id < 0) {
                  return SDLK_RETURN;
               }
               return 1000 + id;
            }
         } else if (event.type == SDL_EVENT_WINDOW_RESIZED || event.type == SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED) {
            UpdateScreen();
         }
      }
   }
}

static void BlitSurfaceToRenderer(SDL_Surface *surface, const SDL_Rect *dstrect)
{
   if (surface == nullptr || gpRenderer == nullptr) return;
   SDL_Texture *tex = SDL_CreateTextureFromSurface(gpRenderer, surface);
   if (tex != nullptr) {
      SDL_FRect frect;
      frect.x = dstrect ? (float)dstrect->x : 0.0f;
      frect.y = dstrect ? (float)dstrect->y : 0.0f;
      frect.w = dstrect ? (float)dstrect->w : (float)surface->w;
      frect.h = dstrect ? (float)dstrect->h : (float)surface->h;
      SDL_RenderTexture(gpRenderer, tex, NULL, &frect);
      SDL_DestroyTexture(tex);
   }
}

void CGeneral::UpdateScreen(int x, int y, int w, int h)
{
   (void)x; (void)y; (void)w; (void)h;
   if (gpRenderer != nullptr && gpScreen != nullptr) {
      if (gpScreenTexture == nullptr) {
         gpScreenTexture = SDL_CreateTextureFromSurface(gpRenderer, gpScreen);
         if (gpScreenTexture != nullptr) {
            SDL_SetTextureScaleMode(gpScreenTexture, SDL_SCALEMODE_LINEAR);
         }
      } else {
         if (!SDL_UpdateTexture(gpScreenTexture, NULL, gpScreen->pixels, gpScreen->pitch)) {
            SDL_DestroyTexture(gpScreenTexture);
            gpScreenTexture = SDL_CreateTextureFromSurface(gpRenderer, gpScreen);
            if (gpScreenTexture != nullptr) {
               SDL_SetTextureScaleMode(gpScreenTexture, SDL_SCALEMODE_LINEAR);
            }
         }
      }
      if (gpScreenTexture != nullptr) {
         SDL_RenderClear(gpRenderer);
         SDL_RenderTexture(gpRenderer, gpScreenTexture, NULL, NULL);
         SDL_RenderPresent(gpRenderer);
      }
   }
}

void CGeneral::ClearScreen(bool fadein, bool fadeout, bool bg)
{
   (void)fadein; (void)fadeout;
   if (gpScreen == nullptr) return;

   if (bg && m_imgBack != nullptr) {
      int w = gpScreen->w;
      while (w > 0) {
         int h = gpScreen->h;
         while (h > 0) {
            SDL_Rect dstrect;
            dstrect.x = gpScreen->w - w;
            dstrect.y = gpScreen->h - h;
            dstrect.w = m_imgBack->w;
            dstrect.h = m_imgBack->h;
            SDL_BlitSurface(m_imgBack, NULL, gpScreen, &dstrect);
            h -= m_imgBack->h;
         }
         w -= m_imgBack->w;
      }
   } else {
      UTIL_FillRect(gpScreen, 0, 0, 640, 480, 30, 130, 100);
   }

   UTIL_RectShade(gpScreen, 0, 0, 640, 480, 196, 196,
      0, 0, 196, 196, 196, 0, 196);
}

void CGeneral::ClearPromptArea()
{
   if (gpScreen != nullptr) {
      UTIL_FillRect(gpScreen, 20, 260, 595, 65, 30, 130, 100);
      UpdateScreen(20, 260, 595, 65);
   }
}

void CGeneral::DrawTextBrush(const char *t, int x, int y, int r, int g, int b, int size)
{
   SDL_Surface *s = m_fntBrush.Render(t, r, g, b, size, ((size < 32) ? false : true));
   if (s == nullptr || gpScreen == nullptr) return;

   SDL_Rect dstrect;
   dstrect.x = x;
   dstrect.y = y;
   dstrect.w = s->w;
   dstrect.h = s->h;

   SDL_BlitSurface(s, NULL, gpScreen, &dstrect);
   SDL_DestroySurface(s);

   UpdateScreen(x, y, dstrect.w, dstrect.h);
}

void CGeneral::DrawText(const char *t, int x, int y, int r, int g, int b, int size)
{
   SDL_Surface *s = m_fnt.Render(t, r, g, b, size);
   if (s == nullptr || gpScreen == nullptr) return;

   SDL_Rect dstrect;
   dstrect.x = x;
   dstrect.y = y;
   dstrect.w = s->w;
   dstrect.h = s->h;

   SDL_BlitSurface(s, NULL, gpScreen, &dstrect);
   SDL_DestroySurface(s);

   UpdateScreen(x, y, dstrect.w, dstrect.h);
}

void CGeneral::DrawTextInBox(const char *t, int box_x, int box_y, int box_w, int box_h, int r, int g, int b, int size)
{
   SDL_Surface *s = m_fnt.Render(t, r, g, b, size);
   if (s == nullptr || gpScreen == nullptr) return;

   SDL_Rect dstrect;
   dstrect.x = box_x + (box_w > s->w ? (box_w - s->w) / 2 : 5);
   dstrect.y = box_y + (box_h > s->h ? (box_h - s->h) / 2 : 5);
   dstrect.w = s->w;
   dstrect.h = s->h;

   SDL_BlitSurface(s, NULL, gpScreen, &dstrect);
   SDL_DestroySurface(s);

   UpdateScreen(box_x, box_y, box_w, box_h);
}

void CGeneral::DrawWrappedTextInBox(const char *t, int box_x, int box_y, int box_w, int box_h, int r, int g, int b, int size)
{
   if (t == nullptr || gpScreen == nullptr) return;

   SDL_Surface *s = m_fnt.RenderWrapped(t, r, g, b, size, box_w - 20);
   if (s != nullptr) {
      SDL_Rect dstrect;
      dstrect.x = box_x + 10;
      dstrect.y = box_y + (box_h > s->h ? (box_h - s->h) / 2 : 5);
      dstrect.w = s->w;
      dstrect.h = s->h;

      SDL_BlitSurface(s, NULL, gpScreen, &dstrect);
      SDL_DestroySurface(s);
      UpdateScreen(box_x, box_y, box_w, box_h);
   }
}

SDL_Surface *CGeneral::RenderTextWrapped(const char *t, int r, int g, int b, int size, int wrap_width)
{
   return m_fnt.RenderWrapped(t, r, g, b, size, wrap_width);
}

SDL_Surface *CGeneral::RenderCard(const CCard &c, int w, int h)
{
   SDL_Surface *s;
   int pw = m_imgCards->w / 4, ph = m_imgCards->h / 13;

   s = SDL_CreateSurface(w, h, SDL_PIXELFORMAT_RGBA8888);

   w--;
   h--;

   SDL_SetSurfaceColorKey(s, true, SDL_MapSurfaceRGBA(s, 255, 0, 255, 255));
   UTIL_FillRect(s, w, 1, 1, h - 1, 1, 1, 1);
   UTIL_FillRect(s, 1, h, w, 1, 1, 1, 1);

   SDL_Rect dstrect, dstrect2;
   if (c.GetValue() >= 48) {
      dstrect.x = pw * 2;
      dstrect.y = ph * 12;
   } else {
      dstrect.x = (c.GetValue() & 3) * pw;
      dstrect.y = (c.GetMonth() - 1) * ph;
   }
   dstrect.w = pw;
   dstrect.h = ph;
   dstrect2.x = dstrect2.y = 0;
   dstrect2.w = w;
   dstrect2.h = h;

   SDL_BlitSurfaceScaled(m_imgCards, &dstrect, s, &dstrect2, SDL_SCALEMODE_LINEAR);

   if (c.m_iRenderEffect & EF_DARK) {
      int i, j;
      unsigned char r, g, b;
      for (i = 0; i < s->w; i++) {
         for (j = 0; j < s->h; j++) {
            UTIL_GetPixel(s, i, j, &r, &g, &b);
            r = r * 2 / 3;
            g = g * 2 / 3;
            b = b * 2 / 3;
            UTIL_PutPixel(s, i, j, r, g, b);
         }
      }
   }

   if (c.m_iRenderEffect & EF_BOX) {
      UTIL_RectShade(s, 0, 0, w, h, 255, 255, 0, 0, 255, 255, 255, 0, 255);
      UTIL_RectShade(s, 1, 1, w - 2, h - 2, 255, 255, 0, 0, 255, 255, 255, 0, 255);
   }

   return s;
}

void CGeneral::DrawCard(const CCard &c, int x, int y, int w, int h, bool update)
{
   SDL_Surface *p = RenderCard(c, w, h);
   if (p == nullptr || gpScreen == nullptr) return;

   SDL_Rect dstrect;
   dstrect.x = x;
   dstrect.y = y;
   dstrect.w = w;
   dstrect.h = h;

   SDL_BlitSurface(p, NULL, gpScreen, &dstrect);
   SDL_DestroySurface(p);

   if (update) {
      UpdateScreen(x, y, dstrect.w, dstrect.h);
   }
}

void CGeneral::LoadFonts()
{
   TTF_Init();
   m_fntBrush.Load(FONTS_DIR "brush.fnt");
   m_fnt.Load(va("%s%s.fnt", FONTS_DIR, cfg.Get("OPTIONS", "Language", "eng")));
}

void CGeneral::LoadImages()
{
   m_imgCards = LoadBitmapFile(IMAGES_DIR "cards.bmp");
   m_imgBack = LoadBitmapFile(IMAGES_DIR "back.bmp");
}

void CGeneral::FreeImages()
{
   SDL_DestroySurface(m_imgCards);
   SDL_DestroySurface(m_imgBack);
}

void CGeneral::LoadSound()
{
   if (g_fNoSound) {
      memset(m_snd, 0, sizeof(m_snd));
      return;
   }

   if (m_fSndLoaded) {
      return;
   }

   m_fSndLoaded = true;

   int i;

   for (i = 0; i < NUM_SOUND; i++) {
      assert(*soundfile[i]);
      m_snd[i] = LoadSoundFile(va("%s%s.wav", SOUND_DIR, soundfile[i]));
   }
}

void CGeneral::FreeSound()
{
   if (g_fNoSound) {
      return;
   }

   int i;

   for (i = 0; i < NUM_SOUND; i++) {
      if (m_snd[i] != NULL) {
         SOUND_FreeWAV(m_snd[i]);
      }
   }
}

void CGeneral::PlaySound(int num)
{
   if (g_fNoSound) return;
   if (num < 0 || num >= NUM_SOUND) return;

   if (m_snd[num] != NULL) {
      SOUND_PlayWAV(m_snd[num]);
   }
}

SDL_Surface *CGeneral::LoadBitmapFile(const char *filename)
{
   SDL_Surface *pic = SDL_LoadBMP(filename);

   if (pic == NULL) {
      TerminateOnError("Cannot load bitmap file %s: %s",
         filename, SDL_GetError());
   }

   return pic;
}

SoundSample *CGeneral::LoadSoundFile(const char *filename)
{
   if (g_fNoSound) {
      return NULL;
   }

   SoundSample *s = SOUND_LoadWAV(filename);

   if (s == NULL) {
      TerminateOnError("Cannot load sound file %s: %s",
         filename, SDL_GetError());
   }

   return s;
}

void CGeneral::InitCursor()
{
   char cursor_data[22 + 1][18 + 1] = {
      "*****  ***********",
      "**** )) **********",
      "**** )) **********",
      "**** )) **********",
      "**** )) **********",
      "**** ))   ********",
      "**** )) ))   *****",
      "**** )) )) ))  ***",
      "**** )) )) )) ) **",
      "   ) )) )) )) )) *",
      " ))  )))))))) )) *",
      " ))) ))))))))))) *",
      "* )) ))))))))))) *",
      "** ) ))))))))))) *",
      "** ))))))))))))) *",
      "*** )))))))))))) *",
      "*** ))))))))))) **",
      "**** )))))))))) **",
      "**** )))))))))) **",
      "***** )))))))) ***",
      "***** )))))))) ***",
      "*****          ***"
   };

   Uint8 data[24 * 3], mask[24 * 3];
   int i, j, index = -1;

   memset(data, 0, sizeof(data));
   memset(mask, 0, sizeof(mask));

   for (i = 0; i < 22; i++) {
      for (j = 0; j < 24; j++) {
         if (j % 8 != 0) {
            data[index] <<= 1;
            mask[index] <<= 1;
         } else {
            index++;
         }

         if (j >= 18) {
            continue;
         }

         switch (cursor_data[i][j]) {
            case ')':
               mask[index] |= 1;
               break;

            case ' ':
               data[index] |= 1;
               mask[index] |= 1;
               break;
         }
      }
   }

   m_HandCursor = SDL_CreateCursor(data, mask, 24, 24, 0, 0);
   SDL_SetCursor(m_HandCursor);
}

void CGeneral::FreeCursor()
{
   if (m_HandCursor != nullptr) {
      SDL_DestroyCursor(m_HandCursor);
      m_HandCursor = nullptr;
   }
}

CBox::CBox(int x, int y, int w, int h, int r, int g, int b, int a, bool keep)
{
   if (r == 0 && g == 0 && b == 0) {
      m_fFakeBox = true;
      return;
   }

   m_fFakeBox = false;
   m_SavedRect.x = x;
   m_SavedRect.y = y;
   m_SavedRect.w = w;
   m_SavedRect.h = h;

   if (!keep && gpScreen != nullptr) {
      m_pSavedArea = SDL_CreateSurface(w, h, gpScreen->format);
      if (m_pSavedArea != nullptr) {
         SDL_Rect srcrect = {x, y, w, h};
         SDL_BlitSurface(gpScreen, &srcrect, m_pSavedArea, NULL);
      }
   } else {
      m_pSavedArea = nullptr;
   }

   if (gpScreen != nullptr) {
      UTIL_FillRectAlpha(gpScreen, x, y, w, h, r, g, b, a);
      UTIL_RectShade(gpScreen, x, y, w, h, 255, 255, 255, 0, 0, 0, 128, 128, 128);
   }

   if (gpGeneral) {
      gpGeneral->UpdateScreen(x, y, w, h);
   }
}

CBox::~CBox()
{
   if (m_fFakeBox) {
      return;
   }

   if (m_pSavedArea != nullptr && gpScreen != nullptr) {
      SDL_BlitSurface(m_pSavedArea, NULL, gpScreen, &m_SavedRect);
      SDL_DestroySurface(m_pSavedArea);
      m_pSavedArea = nullptr;
   }

   if (gpGeneral) {
      gpGeneral->UpdateScreen(m_SavedRect.x, m_SavedRect.y, m_SavedRect.w, m_SavedRect.h);
   }
}

int CButton::bx[MAX_BUTTONS], CButton::by[MAX_BUTTONS];
int CButton::bw[MAX_BUTTONS], CButton::bh[MAX_BUTTONS];
int CButton::bid[MAX_BUTTONS], CButton::num_buttons = 0;

CButton::CButton(int id, int x, int y, int w, int h, int r, int g, int b):
CBox(x, y, w, h, r, g, b, 128, false),
m_iId(id)
{
   if (num_buttons >= MAX_BUTTONS) {
      TerminateOnError("CButton::CButton(): num_buttons > MAX_BUTTONS!");
   }
   bid[num_buttons] = id;
   bx[num_buttons] = x;
   by[num_buttons] = y;
   bw[num_buttons] = w;
   bh[num_buttons] = h;
   num_buttons++;
}

CButton::~CButton()
{
   int i;
   for (i = 0; i < num_buttons; i++) {
      if (bid[i] == m_iId) {
         break;
      }
   }
   if (i >= num_buttons) {
      TerminateOnError("CButton::~CButton(): i >= num_buttons");
   }

   while (i < num_buttons - 1) {
      bid[i] = bid[i + 1];
      bx[i] = bx[i + 1];
      by[i] = by[i + 1];
      bw[i] = bw[i + 1];
      bh[i] = bh[i + 1];
      i++;
   }

   num_buttons--;
}

int CButton::GetButtonId(int x, int y)
{
   int i;

   for (i = 0; i < num_buttons; i++) {
      if (x > bx[i] && x < bx[i] + bw[i] &&
         y > by[i] && y < by[i] + bh[i]) {
         return bid[i];
      }
   }

   return -1; // not found
}

