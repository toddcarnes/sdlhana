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
#include <vector>
#include <sstream>

CGame::CGame()
{
   m_iGameMode = atoi(Config().Get("GAME", "GameMode", "0"));
   if (m_iGameMode == GAMEMODE_BET) {
      SetMaxHandCards(6); // only 6 cards in bet mode
   }

   m_iScore = atoi(Config().Get("GAME", "Score", "0"));
   m_flAnimDuration = atof(Config().Get("OPTIONS", "AnimSpeed", "180"));
   m_pPlayers[0] = nullptr;
   m_pPlayers[1] = nullptr;

   // don't allow more than 99999 points or less than -99999 points
   if (abs(m_iScore) > 99999) {
      m_iScore = 99999 * (m_iScore / abs(m_iScore));
   }
}

CGame::~CGame()
{
   m_pPlayers[0].reset();
   m_pPlayers[1].reset();
}

void CGame::MainMenu()
{
   while (true) {
      General()->ClearScreen(true, false, true);

      auto mainbox = std::make_unique<CBox>(140, 200, 350, 260, 58, 110, 165);
      auto bNewGame = std::make_unique<CButton>(1, 150, 210, 330, 50, 58, 110, 165);
      auto bOption = std::make_unique<CButton>(2, 150, 270, 330, 50, 58, 110, 165);
      auto bRules = std::make_unique<CButton>(3, 150, 330, 330, 50, 58, 110, 165);
      auto bQuit = std::make_unique<CButton>(4, 150, 390, 330, 50, 58, 110, 165);

      General()->DrawTextInBox(msg("play"), 150, 210, 330, 50, 255, 255, 0, 26);
      General()->DrawTextInBox(msg("settings"), 150, 270, 330, 50, 255, 255, 0, 26);
      General()->DrawTextInBox(msg("btn_rules"), 150, 330, 330, 50, 255, 255, 0, 26);
      General()->DrawTextInBox(msg("quit"), 150, 390, 330, 50, 255, 255, 0, 26);

      int choice = -1;
      while (1) {
         int k = General()->ReadKey();
         if (k > 1000) {
            choice = k - 1000;
            break;
         }
      }

      bNewGame.reset();
      bOption.reset();
      bRules.reset();
      bQuit.reset();
      mainbox.reset();

      switch (choice) {
         // single player game
         case 1:
            RunGame();
            break;

         // settings
         case 2:
            Settings();
            break;

         // rules & how to play
         case 3:
            RulesMenu();
            break;

         // quit game
         case 4:
            UserQuit();
            return;
      }
   }
}

void CGame::RulesMenu()
{
   int scroll_y = 0;
   int canvas_w = 540;
   int canvas_h = 4200;

   SurfacePtr canvas(SDL_CreateSurface(canvas_w, canvas_h, SDL_PIXELFORMAT_RGBA8888), SDL_DestroySurface);
   if (!canvas) return;

   SDL_FillSurfaceRect(canvas.get(), NULL, SDL_MapSurfaceRGBA(canvas.get(), 20, 32, 52, 255));

   auto DrawCanvasText = [&](const char *txt, int x, int y, int w, int r, int g, int b, int size) -> int {
      if (txt == nullptr) return y + 20;
      SDL_Surface *s = General()->RenderTextWrapped(txt, r, g, b, size, w);
      int rendered_h = 20;
      if (s != nullptr) {
         SDL_Rect dst = { x, y, s->w, s->h };
         SDL_BlitSurface(s, NULL, canvas.get(), &dst);
         rendered_h = s->h;
         SDL_DestroySurface(s);
      }
      return y + rendered_h + 8;
   };

   auto DrawCanvasCard = [&](int card_id, int x, int y, int w = 42, int h = 66) {
      SDL_Surface *card_surf = General()->RenderCard(CCard(card_id), w, h);
      if (card_surf != nullptr) {
         SDL_Rect dst = { x, y, w, h };
         SDL_BlitSurface(card_surf, NULL, canvas.get(), &dst);
         SDL_DestroySurface(card_surf);
      }
   };

   int cy = 15;

   // SECTION 1: DECK OVERVIEW & 12 MONTH SUITS
   cy = DrawCanvasText("1. Hanafuda Deck & 12 Month Suits", 10, cy, 520, 255, 255, 0, 20);
   cy = DrawCanvasText("A standard Hanafuda deck consists of 48 cards divided into 12 monthly suits representing flora and fauna, with 4 cards per month:", 10, cy, 520, 255, 255, 255, 14);

   const char *month_names[12] = {
      "January - Pine (松)", "February - Plum Blossom (梅)", "March - Cherry Blossom (桜)",
      "April - Wisteria (藤)", "May - Iris (菖蒲)", "June - Peony (牡丹)",
      "July - Bush Clover (萩)", "August - Pampas Grass (芒)", "September - Chrysanthemum (菊)",
      "October - Maple (楓)", "November - Willow (柳)", "December - Paulownia (桐)"
   };

   const char *month_descs[12] = {
      "Crane & Sun (Bright 20pt), Red Poetry Ribbon (5pt), 2 Plain Cards (1pt).",
      "Bush Warbler (Animal 5pt), Red Poetry Ribbon (5pt), 2 Plain Cards (1pt).",
      "Curtain (Bright 20pt), Red Poetry Ribbon (5pt), 2 Plain Cards (1pt).",
      "Cuckoo (Animal 5pt), Red Grass Ribbon (5pt), 2 Plain Cards (1pt).",
      "Eight-Plank Bridge (Animal 5pt), Red Grass Ribbon (5pt), 2 Plain Cards (1pt).",
      "Butterflies (Animal 5pt), Blue Ribbon (5pt), 2 Plain Cards (1pt).",
      "Boar (Animal 5pt), Red Grass Ribbon (5pt), 2 Plain Cards (1pt).",
      "Full Moon (Bright 20pt), Geese (Animal 5pt), 2 Plain Cards (1pt).",
      "Sake Cup (Animal/Plain), Blue Ribbon (5pt), 2 Plain Cards (1pt).",
      "Deer (Animal 5pt), Blue Ribbon (5pt), 2 Plain Cards (1pt).",
      "Rain Man (Bright 20pt), Swallow (5pt), Red Ribbon (5pt), Ssangpi Double Plain.",
      "Phoenix (Bright 20pt), 2 Plain Cards, Ssangpi Double Plain."
   };

   for (int m = 0; m < 12; m++) {
      cy = DrawCanvasText(month_names[m], 10, cy, 520, 0, 255, 255, 16);
      int card_start = m * 4;
      int card_y = cy;
      for (int c = 0; c < 4; c++) {
         DrawCanvasCard(card_start + c, 10 + c * 48, card_y, 42, 66);
      }
      int desc_end_y = DrawCanvasText(month_descs[m], 215, card_y, 310, 240, 255, 255, 13);
      cy = (card_y + 66 > desc_end_y ? card_y + 66 : desc_end_y) + 18;
   }

   // SECTION 2: DEALER SELECTION & TURN FLOW
   cy += 10;
   cy = DrawCanvasText("2. First-Dealer Selection & Turn Flow", 10, cy, 520, 255, 255, 0, 20);
   cy = DrawCanvasText("Before Round 1, both players draw a card from the deck (Mekuri). The player drawing the card from the earliest Month becomes the first dealer (Oya / Sun). The winner of each round becomes dealer for the next round!", 10, cy, 520, 255, 255, 255, 14);

   // SECTION 3: JAPANESE KOI-KOI RULES & YAKU
   cy += 10;
   cy = DrawCanvasText("3. Japanese Koi-Koi Rules & Yaku Sets", 10, cy, 520, 255, 255, 0, 20);
   cy = DrawCanvasText("Form combination sets (Yaku) to earn points. Calling Koi-Koi continues the round for higher points, but if opponent scores first, they win!", 10, cy, 520, 255, 255, 255, 14);

   cy = DrawCanvasText("Lights (Gokou):", 10, cy, 520, 0, 255, 255, 15);
   int card_y = cy;
   DrawCanvasCard(0, 10, card_y);  // Crane
   DrawCanvasCard(8, 55, card_y);  // Curtain
   DrawCanvasCard(28, 100, card_y);// Moon
   DrawCanvasCard(40, 145, card_y);// Rain Man
   DrawCanvasCard(44, 190, card_y);// Phoenix
   int desc_y = DrawCanvasText("Five Lights (10 pts), Four Lights (8 pts), Rain Four Lights (7 pts), Three Lights (5 pts).", 240, card_y, 285, 255, 255, 255, 13);
   cy = (card_y + 66 > desc_y ? card_y + 66 : desc_y) + 18;

   cy = DrawCanvasText("Ribbons (Akatan & Aotan):", 10, cy, 520, 0, 255, 255, 15);
   card_y = cy;
   DrawCanvasCard(1, 10, card_y);
   DrawCanvasCard(5, 55, card_y);
   DrawCanvasCard(9, 100, card_y);
   DrawCanvasCard(21, 145, card_y);
   DrawCanvasCard(33, 190, card_y);
   DrawCanvasCard(37, 235, card_y);
   desc_y = DrawCanvasText("Red Poetry Ribbons Akatan (5 pts), Blue Ribbons Aotan (5 pts), 5 Plain Ribbons (1 pt + 1 pt per extra).", 285, card_y, 240, 255, 255, 255, 13);
   cy = (card_y + 66 > desc_y ? card_y + 66 : desc_y) + 18;

   cy = DrawCanvasText("Ino-Shika-Chou (Boar, Deer, Butterflies):", 10, cy, 520, 0, 255, 255, 15);
   card_y = cy;
   DrawCanvasCard(24, 10, card_y);
   DrawCanvasCard(36, 55, card_y);
   DrawCanvasCard(20, 100, card_y);
   desc_y = DrawCanvasText("Ino-Shika-Chou (5 pts + 1 pt per extra animal), 5 Animals (1 pt + 1 pt per extra animal).", 150, card_y, 375, 255, 255, 255, 13);
   cy = (card_y + 66 > desc_y ? card_y + 66 : desc_y) + 18;

   // SECTION 4: KOREAN GO-STOP RULES & PENALTIES
   cy += 10;
   cy = DrawCanvasText("4. Korean Go-Stop Rules & Penalties", 10, cy, 520, 255, 255, 0, 20);
   cy = DrawCanvasText("In Go-Stop, players must reach at least 3 points before calling Go or Stop. Multipliers increase with Go calls, Gwang-bak (Light Penalty), or Pi-bak (Junk Penalty)!", 10, cy, 520, 255, 255, 255, 14);

   cy = DrawCanvasText("Godori (5 Birds - 5 pts):", 10, cy, 520, 0, 255, 255, 15);
   card_y = cy;
   DrawCanvasCard(4, 10, card_y);
   DrawCanvasCard(12, 55, card_y);
   DrawCanvasCard(29, 100, card_y);
   desc_y = DrawCanvasText("February Bush Warbler, April Cuckoo, and August Geese (5 pts).", 150, card_y, 375, 255, 255, 255, 13);
   cy = (card_y + 66 > desc_y ? card_y + 66 : desc_y) + 18;

   cy = DrawCanvasText("Ribbon Combinations (Hongdan, Cheongdan, Chodan):", 10, cy, 520, 0, 255, 255, 15);
   card_y = cy;
   DrawCanvasCard(13, 10, card_y);
   DrawCanvasCard(17, 55, card_y);
   DrawCanvasCard(25, 100, card_y);
   desc_y = DrawCanvasText("Hongdan Red Poetry (3 pts), Cheongdan Blue (3 pts), Chodan Grass Ribbons (3 pts).", 150, card_y, 375, 255, 255, 255, 13);
   cy = (card_y + 66 > desc_y ? card_y + 66 : desc_y) + 18;

   cy = DrawCanvasText("Ssangpi (Double Junk) & Penalties:", 10, cy, 520, 0, 255, 255, 15);
   card_y = cy;
   DrawCanvasCard(43, 10, card_y);
   DrawCanvasCard(47, 55, card_y);
   DrawCanvasCard(32, 100, card_y);
   desc_y = DrawCanvasText("November/December double junk and Sake Cup count as 2 pi. 3+ Go doubles score; Gwang-bak and Pi-bak double opponent losses!", 150, card_y, 375, 255, 255, 255, 13);
   cy = (card_y + 66 > desc_y ? card_y + 66 : desc_y) + 18;

   // SECTION 5: BET MODE & DOUBLE UP
   cy += 10;
   cy = DrawCanvasText("5. Bet Mode & Double Up", 10, cy, 520, 255, 255, 0, 20);
   cy = DrawCanvasText("In Bet Mode, win hands and double up your score by guessing whether the next drawn card is Big (Months 7-12) or Small (Months 1-6)!", 10, cy, 520, 255, 255, 255, 14);
   cy += 30;

   int max_scroll = cy > 330 ? (cy - 330) : 0;

   General()->ClearScreen(true, false, true);

   auto mainbox = std::make_unique<CBox>(25, 15, 590, 450, 40, 55, 85, 255, true);
   auto titlebox = std::make_unique<CBox>(25, 15, 590, 45, 0, 128, 128, 255, true);
   auto viewportbox = std::make_unique<CBox>(35, 68, 545, 334, 20, 32, 52, 255, true);
   auto btnBack = std::make_unique<CButton>(3, 415, 412, 190, 42, 165, 58, 58);

   bool is_dragging_scroll = false;
   bool redraw = true;
   while (true) {
      if (redraw) {
         General()->DrawTextInBox(msg("rules_header"), 25, 15, 590, 45, 20, 32, 52, 22);

         SDL_Rect clip_rect = { 35, 68, 545, 334 };
         SDL_SetSurfaceClipRect(Screen(), &clip_rect);

         SDL_Rect src_rect = { 0, scroll_y, 540, 330 };
         SDL_Rect dst_rect = { 38, 70, 540, 330 };
         SDL_BlitSurface(canvas.get(), &src_rect, Screen(), &dst_rect);

         SDL_SetSurfaceClipRect(Screen(), NULL);

         UTIL_FillRect(Screen(), 588, 70, 12, 330, 15, 25, 40);
         int thumb_h = 50;
         int thumb_y = 70 + (max_scroll > 0 ? (scroll_y * (330 - thumb_h)) / max_scroll : 0);
         UTIL_FillRect(Screen(), 588, thumb_y, 12, thumb_h, 0, 200, 255);

         General()->DrawTextInBox("Back to Menu", 415, 412, 190, 42, 255, 255, 255, 18);

         General()->UpdateScreen();
         redraw = false;
      }

      SDL_Event event;
      if (SDL_WaitEvent(&event)) {
         if (Renderer() != nullptr) {
            SDL_ConvertEventToRenderCoordinates(Renderer(), &event);
         }
         if (event.type == SDL_EVENT_QUIT) {
            
            return;
         } else if (event.type == SDL_EVENT_KEY_DOWN) {
            if (event.key.key == SDLK_UP) {
               scroll_y = (scroll_y - 80 < 0) ? 0 : (scroll_y - 80);
               redraw = true;
            } else if (event.key.key == SDLK_DOWN) {
               scroll_y = (scroll_y + 80 > max_scroll) ? max_scroll : (scroll_y + 80);
               redraw = true;
            } else if (event.key.key == SDLK_ESCAPE || event.key.key == SDLK_RETURN) {
               
               return;
            }
         } else if (event.type == SDL_EVENT_MOUSE_WHEEL) {
            if (event.wheel.y > 0.0f || event.wheel.y < 0.0f) {
               int delta = (event.wheel.y > 0.0f) ? -80 : 80;
               scroll_y = (scroll_y + delta < 0) ? 0 : ((scroll_y + delta > max_scroll) ? max_scroll : (scroll_y + delta));
               redraw = true;
            }
         } else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
            float mx = event.button.x;
            float my = event.button.y;

            if (mx >= 585 && mx <= 602 && my >= 70 && my <= 400) {
               is_dragging_scroll = true;
               float ratio = (my - 70.0f - 25.0f) / (330.0f - 50.0f);
               scroll_y = (ratio < 0.0f) ? 0 : ((ratio > 1.0f) ? max_scroll : (int)(ratio * max_scroll));
               redraw = true;
            } else if (mx >= 35 && mx <= 584 && my >= 68 && my <= 402) {
               if (my < 235) {
                  scroll_y = (scroll_y - 80 < 0) ? 0 : (scroll_y - 80);
               } else {
                  scroll_y = (scroll_y + 80 > max_scroll) ? max_scroll : (scroll_y + 80);
               }
               redraw = true;
            } else if (mx >= 415 && mx <= 605 && my >= 412 && my <= 454) {
               
               return;
            }
         } else if (event.type == SDL_EVENT_MOUSE_MOTION) {
            if (is_dragging_scroll) {
               float my = event.motion.y;
               float ratio = (my - 70.0f - 25.0f) / (330.0f - 50.0f);
               scroll_y = (ratio < 0.0f) ? 0 : ((ratio > 1.0f) ? max_scroll : (int)(ratio * max_scroll));
               redraw = true;
            }
         } else if (event.type == SDL_EVENT_MOUSE_BUTTON_UP) {
            is_dragging_scroll = false;
         } else if (event.type == SDL_EVENT_WINDOW_RESIZED || event.type == SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED) {
            redraw = true;
         }
      }
   }
}

void CGame::Settings()
{
   constexpr int kBtnX = 280;
   constexpr int kBtnW = 150;
   constexpr int kBtnH = 24;
   constexpr int kSliderX = 280;
   constexpr int kSliderY = 80;
   constexpr int kSliderW = 150;
   constexpr int kSliderH = 24;

   CButton fs(1, kBtnX, 20, kBtnW, kBtnH, 0, 0, 0);
   CButton snd(2, kBtnX, 50, kBtnW, kBtnH, 0, 0, 0);
   CButton gm(3, kBtnX, 110, kBtnW, kBtnH, 0, 0, 0);
   CButton l(4, kBtnX, 140, kBtnW, kBtnH, 0, 0, 0);
   CButton as(5, kBtnX, 170, kBtnW, kBtnH, 0, 0, 0);
   CButton ok(6, 20, 210, 150, 34, 58, 110, 165);

   int curgm = atoi(Config().Get("GAME", "GameMode", "0"));
   int curas = 3;
   int volume = atoi(Config().Get("OPTIONS", "Volume", "100"));
   if (volume < 0) volume = 0;
   if (volume > 100) volume = 100;

   std::vector<std::string> langs = DiscoverLanguages();
   size_t langIdx = 0;
   {
      std::string curLang = Config().Get("OPTIONS", "Language", "eng");
      for (size_t i = 0; i < langs.size(); i++) {
         if (langs[i] == curLang) { langIdx = i; break; }
      }
   }
   int valueas[5] = {800, 500, 300, 180, 50};

   bool isDragging = false;

   auto DrawSlider = [&](int vol, bool enabled) {
      int trackH = 6;
      int trackY = kSliderY + (kSliderH - trackH) / 2;
      int bgR = enabled ? 60 : 80;
      int bgG = enabled ? 60 : 80;
      int bgB = enabled ? 60 : 80;
      int fillR = enabled ? 0 : 120;
      int fillG = enabled ? 120 : 120;
      int fillB = enabled ? 255 : 120;
      int thumbR = enabled ? 255 : 180;
      int thumbG = enabled ? 255 : 180;
      int thumbB = enabled ? 255 : 180;
      if (Screen()) {
         UTIL_FillRect(Screen(), kSliderX, trackY, kSliderW, trackH, bgR, bgG, bgB);
         int fillW = kSliderW * vol / 100;
         if (fillW > 0) {
            UTIL_FillRect(Screen(), kSliderX, trackY, fillW, trackH, fillR, fillG, fillB);
         }
         int thumbX = kSliderX + fillW - 4;
         if (thumbX < kSliderX) thumbX = kSliderX;
         if (thumbX > kSliderX + kSliderW - 8) thumbX = kSliderX + kSliderW - 8;
         UTIL_FillRect(Screen(), thumbX, kSliderY + 4, 8, kSliderH - 8, thumbR, thumbG, thumbB);
         UTIL_Rect(Screen(), thumbX, kSliderY + 4, 8, kSliderH - 8, 0, 0, 0);
         UTIL_Rect(Screen(), kSliderX, trackY, kSliderW, trackH, 0, 0, 0);
      }
   };

   while (true) {
      bool soundEnabled = !atoi(Config().Get("OPTIONS", "NoSound", "0"));

      General()->ClearScreen();

      // Labels — volume label grayed when disabled
      General()->DrawText(msg("fullscreen"), 20, 20, 255, 255, 0, 24);
      General()->DrawText(msg("enablesound"), 20, 50, 255, 255, 0, 24);
      if (soundEnabled) {
         General()->DrawText(msg("volume"), 20, kSliderY + 2, 255, 255, 0, 24);
      } else {
         General()->DrawText(msg("volume"), 20, kSliderY + 2, 130, 130, 130, 24);
      }
      General()->DrawText(msg("gamemode"), 20, 110, 255, 255, 0, 24);
      General()->DrawText(msg("language"), 20, 140, 255, 255, 0, 24);
      General()->DrawText(msg("animspeed"), 20, 170, 255, 255, 0, 24);
      General()->DrawTextInBox("OK", 20, 210, 150, 34, 255, 255, 255, 20);

      const char *strgm[3] = {msg("gamemode0"), msg("gamemode1"), msg("gamemode2")};
      const char *stras[5] = {msg("veryslow"), msg("slow"), msg("middle"), msg("fast"), msg("veryfast")};

      if (atoi(Config().Get("OPTIONS", "FullScreen", "0"))) {
         General()->DrawText(msg("Enabled"), kBtnX, 20, 255, 255, 255, 24);
      } else {
         General()->DrawText(msg("Disabled"), kBtnX, 20, 255, 255, 255, 24);
      }

      if (soundEnabled) {
         General()->DrawText(msg("Enabled"), kBtnX, 50, 255, 255, 255, 24);
      } else {
         General()->DrawText(msg("Disabled"), kBtnX, 50, 255, 255, 255, 24);
      }

      DrawSlider(volume, soundEnabled);
      {
         std::string volStr = std::format("{}%", volume);
         int tr = soundEnabled ? 255 : 130;
         int tg = soundEnabled ? 255 : 130;
         int tb = soundEnabled ? 255 : 130;
         General()->DrawText(volStr.c_str(), kSliderX + kSliderW + 10, kSliderY + 2, tr, tg, tb, 16);
      }

      General()->DrawText(strgm[curgm], kBtnX, 110, 255, 255, 255, 24);
      General()->DrawText(msg(Config().Get("OPTIONS", "Language", "eng")), kBtnX, 140, 255, 255, 255, 24);
      General()->DrawText(stras[curas], kBtnX, 170, 255, 255, 255, 24);

      General()->UpdateScreen(0, 0, layout::kScreenW, layout::kScreenH);

      SDL_Event event;
      if (!SDL_WaitEvent(&event)) continue;
      if (Renderer()) SDL_ConvertEventToRenderCoordinates(Renderer(), &event);

      if (event.type == SDL_EVENT_QUIT) {
         return;
      } else if (event.type == SDL_EVENT_KEY_DOWN) {
         if (event.key.key == SDLK_ESCAPE) {
            // Treat ESC as OK
            InitTextMessage();
            General()->LoadFonts();
            if (atoi(Config().Get("OPTIONS", "NoSound", "0"))) {
               NoSound() = true;
            } else {
               NoSound() = false;
               if (!g_fAudioOpened.load()) {
                  if (SOUND_OpenAudio(22050, SDL_AUDIO_S16, 1, 1024)) {
                     std::println(stderr, "WARNING: Couldn't open audio: {}", SDL_GetError());
                     NoSound() = true;
                  } else {
                     g_fAudioOpened.store(true);
                     General()->LoadSound();
                  }
               } else {
                  SOUND_SetVolume(volume);
                  General()->LoadSound();
               }
            }
            bool fs_setting = (atoi(Config().Get("OPTIONS", "FullScreen", "0")) > 0);
            bool fs_active = false;
            if (Window() != nullptr) {
               fs_active = (SDL_GetWindowFlags(Window()) & SDL_WINDOW_FULLSCREEN) != 0;
            }
            if (fs_setting != fs_active) {
               UTIL_ToggleFullScreen();
            }
            return;
         } else if (event.key.key == SDLK_LEFT && soundEnabled) {
            volume -= 5;
            if (volume < 0) volume = 0;
            Config().Set("OPTIONS", "Volume", std::to_string(volume).c_str());
            SOUND_SetVolume(volume);
            continue;
         } else if (event.key.key == SDLK_RIGHT && soundEnabled) {
            volume += 5;
            if (volume > 100) volume = 100;
            Config().Set("OPTIONS", "Volume", std::to_string(volume).c_str());
            SOUND_SetVolume(volume);
            continue;
         }
      } else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
         if (event.button.button == SDL_BUTTON_LEFT) {
            float mx = event.button.x;
            float my = event.button.y;
            // Slider drag start
            if (soundEnabled && mx >= kSliderX && mx < kSliderX + kSliderW && my >= kSliderY && my < kSliderY + kSliderH) {
               isDragging = true;
               int newVol = (int)((mx - kSliderX) * 100 / kSliderW);
               if (newVol < 0) newVol = 0;
               if (newVol > 100) newVol = 100;
               if (newVol != volume) {
                  volume = newVol;
                  Config().Set("OPTIONS", "Volume", std::to_string(volume).c_str());
                  SOUND_SetVolume(volume);
               }
               continue;
            }
            int id = CButton::GetButtonId((int)mx, (int)my);
            if (id > 0) {
               switch (id) {
                  case 1:
                     if (atoi(Config().Get("OPTIONS", "FullScreen", "0"))) {
                        Config().Set("OPTIONS", "FullScreen", "0");
                     } else {
                        Config().Set("OPTIONS", "FullScreen", "1");
                     }
                     break;
                  case 2:
                     if (atoi(Config().Get("OPTIONS", "NoSound", "0"))) {
                        Config().Set("OPTIONS", "NoSound", "0");
                     } else {
                        Config().Set("OPTIONS", "NoSound", "1");
                     }
                     break;
                  case 3:
                     if (++curgm >= 3) curgm = 0;
                     Config().Set("GAME", "GameMode", std::to_string(curgm).c_str());
                     m_iGameMode = curgm;
                     if (m_iGameMode == GAMEMODE_BET) SetMaxHandCards(6);
                     else SetMaxHandCards(8);
                     break;
                  case 4:
                     langIdx = (langIdx + 1) % langs.size();
                     Config().Set("OPTIONS", "Language", langs[langIdx].c_str());
                     InitTextMessage();
                     General()->LoadFonts();
                     break;
                  case 5:
                     if (++curas >= 5) curas = 0;
                     Config().Set("OPTIONS", "AnimSpeed", std::to_string(valueas[curas]).c_str());
                     m_flAnimDuration = (float)valueas[curas];
                     break;
                  case 6: {
                     InitTextMessage();
                     General()->LoadFonts();
                     if (atoi(Config().Get("OPTIONS", "NoSound", "0"))) {
                        NoSound() = true;
                     } else {
                        NoSound() = false;
                        if (!g_fAudioOpened.load()) {
                           if (SOUND_OpenAudio(22050, SDL_AUDIO_S16, 1, 1024)) {
                              std::println(stderr, "WARNING: Couldn't open audio: {}", SDL_GetError());
                              NoSound() = true;
                           } else {
                              g_fAudioOpened.store(true);
                              General()->LoadSound();
                           }
                        } else {
                           SOUND_SetVolume(volume);
                           General()->LoadSound();
                        }
                     }
                     bool fs_setting = (atoi(Config().Get("OPTIONS", "FullScreen", "0")) > 0);
                     bool fs_active = false;
                     if (Window() != nullptr) {
                        fs_active = (SDL_GetWindowFlags(Window()) & SDL_WINDOW_FULLSCREEN) != 0;
                     }
                     if (fs_setting != fs_active) {
                        UTIL_ToggleFullScreen();
                     }
                     return;
                  }
               }
            }
         }
      } else if (event.type == SDL_EVENT_MOUSE_MOTION) {
         if (isDragging && soundEnabled) {
            float mx = event.motion.x;
            int newVol = (int)((mx - kSliderX) * 100 / kSliderW);
            if (newVol < 0) newVol = 0;
            if (newVol > 100) newVol = 100;
            if (newVol != volume) {
               volume = newVol;
               Config().Set("OPTIONS", "Volume", std::to_string(volume).c_str());
               SOUND_SetVolume(volume);
            }
            continue;
         }
      } else if (event.type == SDL_EVENT_MOUSE_BUTTON_UP) {
         isDragging = false;
      } else if (event.type == SDL_EVENT_WINDOW_RESIZED || event.type == SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED) {
         continue;
      }
   }
}

void CGame::RunGame()
{
   m_pPlayers[0] = std::make_unique<CPlayer>();
   m_pPlayers[1] = std::make_unique<CBot>();

   m_pPlayers[0]->SetOpponent(m_pPlayers[1].get());
   m_pPlayers[1]->SetOpponent(m_pPlayers[0].get());

   InitGame();

   while (m_iCurrentRound <= 12) {
      NewRound();
      PlayRound();
      // save the score
      Config().Set("GAME", "Score", std::to_string(m_iScore).c_str());
      m_iCurrentRound++;
   }

   ShowMatchResults();
}

void CGame::DetermineFirstDealer()
{
   if (GetGameMode() == GAMEMODE_BET) {
      m_pPlayers[1]->SetAsDealer();
      return;
   }

   CCard c1 = CCard::GetRandomCard();
   CCard c2 = CCard::GetRandomCard();
   while (c1 == c2) {
      c2 = CCard::GetRandomCard();
   }

   int m1 = c1.GetMonth();
   int m2 = c2.GetMonth();

   bool player_first = false;
   if (m1 < m2) {
      player_first = true;
   } else if (m1 > m2) {
      player_first = false;
   } else {
      player_first = (c1.GetValue() >= c2.GetValue());
   }

   if (player_first) {
      m_pPlayers[0]->SetAsDealer();
   } else {
      m_pPlayers[1]->SetAsDealer();
   }

   General()->ClearScreen(false, false, false);
   CCard pile(255);
   for (int i = 0; i < 5; i++) {
      General()->DrawCard(pile, 50 + i * 2, 95 + i * 2, 48, 78, true);
   }
   DrawScore();

   General()->DrawCard(c1, 240, 160, 48, 78, true);
   General()->DrawCard(c2, 350, 160, 48, 78, true);

   CBox box(20, 260, 595, 50, 40, 55, 85);
   std::string notice = player_first ?
      std::vformat(msg("dealer_cut_you"), std::make_format_args(m1, m2)) :
      std::vformat(msg("dealer_cut_com"), std::make_format_args(m1, m2));

   General()->DrawTextInBox(notice.c_str(), 20, 260, 595, 50, 255, 255, 255, 18);
   UTIL_Delay(3500);
}

void CGame::InitGame()
{
   m_iScore = 0;
   m_iCurrentRound = 1;
   Config().Set("GAME", "Score", "0");

   DetermineFirstDealer();
}

void CGame::NewRound()
{
   CCard::NewRound();
   m_iNumDeskCard = 0;

   m_pPlayers[0]->NewRound();
   m_pPlayers[1]->NewRound();
}

void CGame::PlayRound()
{
   int i;

   // Deal 8 cards to the table
   for (i = 0; i < 8; i++) {
      m_DeskCards[i] = CCard::GetRandomCard();
   }
   m_iNumDeskCard = 8;

   ValidateInitialDesk();

   if (m_iGameMode == GAMEMODE_BET) {
      m_iScore -= 1;
   }

   // don't allow more than 99999 points or less than -99999 points
   if (abs(m_iScore) > 99999) {
      m_iScore = 99999 * (m_iScore / abs(m_iScore));
   }

   InitScreen();
   AnimDeal();

   // playing the game
   CBasePlayer *current = CBasePlayer::GetDealer(), *winner = NULL;

   while (1) {
      if (current->GetNumHandCard() <= 0) {
         // No card is left
         if (GetGameMode() != GAMEMODE_KOREAN) {
            current->CalcResult();
            if (current->m_Result.score > 0 &&
               current->GetOpponent()->m_Result.score <= 0)
            {
               winner = current;
            }
         } else {
            winner = NULL;
         }
         break;
      }

      int s = current->SelectCard();

      int x = 10 + s * 48;
      int y = current->IsBot() ? 10 : 400;
      if (current->IsBot()) {
         UTIL_Delay(100);
         General()->DrawCard(current->GetHandCard(s), x, y, 48, 78);
         UTIL_Delay(200);
      }

      CardDiscarded(current->GetHandCard(s), current, 10 + 48 * s,
         (current->IsBot() ? 10 : 400));

      current->DeleteCard(s);
      current->DrawHand();
      current->DrawCaptured();
      DrawDeskCard();
      current->CalcResult();
      // Korean game requires at least 3 points
      if (current->m_Result.score - current->GetPrevScore() > 0 &&
         (GetGameMode() != GAMEMODE_KOREAN || current->m_Result.score >= 3)) {
         winner = current;
         current->DrawCurResult();
         if (current->GetNumHandCard() > 0 && current->WantToContinue())
         {
            current->ShiftResult();
            General()->PlaySound(SOUND_GO);
            current->m_iNumContinue++;
            UTIL_Delay(1000);
         } else {
            break;
         }
      }

      if (GetGameMode() == GAMEMODE_KOREAN && current->m_iNumLeaveThree >= 3) {
         General()->ClearPromptArea();
         CBox box(20, 260, 595, 50, 40, 55, 85);
         if (current->IsBot()) {
            General()->DrawTextInBox(msg("comget5pts"), 20, 260, 595, 50, 255, 255, 255, 18);
            m_iScore -= 3;
            General()->PlaySound(SOUND_LOSE);
         } else {
            General()->DrawTextInBox(msg("youget5pts"), 20, 260, 595, 50, 255, 255, 255, 18);
            m_iScore += 3;
            General()->PlaySound(SOUND_WIN);
         }
         DrawScore();
         UTIL_Delay(3500);
         break;
      }

      current = current->GetOpponent();
   }

   if (winner != NULL) {
      if (GetGameMode() == GAMEMODE_KOREAN) {
         winner->CalcAddResult();
      }
      winner->DrawAllResult();
      if (winner->IsBot()) {
         if (m_iGameMode != GAMEMODE_BET) {
            m_iScore -= winner->m_Result.score;
         }
         General()->PlaySound(SOUND_LOSE);
         UTIL_Delay(2500);
      } else {
         General()->PlaySound(SOUND_WIN);
         UTIL_Delay(2500);

         if (m_iGameMode == GAMEMODE_BET) {
            while (DoubleUp(winner)) {
               // Do nothing here
            }
         }

         m_iScore += winner->m_Result.score;
      }

      if (GetGameMode() != GAMEMODE_BET) {
         winner->SetAsDealer();
      }
   }

   return;
}

void CGame::CardDiscarded(const CCard &c, CBasePlayer *current, int sx, int sy)
{
   CCard drawn = CCard::GetRandomCard();
   PhaseState st;
   HandleDiscardPhase(c, drawn, current, sx, sy, st);
   st.save2 = SDL_CreateSurface(48, 78, SDL_PIXELFORMAT_RGBA8888);
   SDL_Rect dstrect{60, 105, 48, 78};
   UTIL_Delay(200);
   General()->DrawCard(drawn, 60, 105, 48, 78, true);
   UTIL_Delay(200);
   HandleDrawnPhase(drawn, current, st);
   AnimatePendingCaptures(st, current);
}

void CGame::GetOneCardFromOpponent(CBasePlayer *current)
{
   int index = -1, index2 = -1, i;
   for (i = 0; i < current->GetOpponent()->GetNumCapturedCard(); i++) {
      const CCard &c = current->GetOpponent()->GetCapturedCard(i);
      if (c.GetType() != CARD_NONE) {
         continue;
      }
      if (c.GetValue() == 43 || c.GetValue() == 45) {
         index2 = i;
      } else {
         index = i;
      }
   }

   CCard g;
   if (index == -1 && index2 == -1) {
      return;
   } else if (index == -1) {
      g = current->GetOpponent()->GetCapturedCard(index2);
      current->AddCapturedCard(g);
      current->GetOpponent()->DeleteCapturedCard(index2);
   } else {
      g = current->GetOpponent()->GetCapturedCard(index);
      current->AddCapturedCard(g);
      current->GetOpponent()->DeleteCapturedCard(index);
   }

   UTIL_Delay(500);
   General()->PlaySound(SOUND_HINT2);
   current->GetOpponent()->DrawCaptured();
   int sy = (current->IsBot() ? 400 : 10), dy = (current->IsBot() ? 10 : 400);

   SDL_Surface *save = SDL_CreateSurface(48, 78, SDL_PIXELFORMAT_RGBA8888);

   SDL_Rect dstrect;
   dstrect.x = 575;
   dstrect.y = sy;
   dstrect.w = 48;
   dstrect.h = 78;

   General()->DrawCard(g, 575, sy, 48, 78, true);
   AnimCardMove(575, sy, 575, dy, 48, 78, save);
   SDL_DestroySurface(save);
   current->DrawCaptured();
   UTIL_Delay(500);
}

int CGame::SelectCardOnDesk(int month)
{
   std::unique_ptr<CButton> b[2];
   int count = 0, index[2], i;

   for (i = 0; i < 2; i++) {
      b[i] = nullptr;
      index[i] = -1;
   }

   SDL_Surface *save = SDL_CreateSurface(480, 166, SDL_PIXELFORMAT_RGBA8888);

   SDL_Rect dstrect;
   dstrect.x = 140;
   dstrect.y = 100;
   dstrect.w = 480;
   dstrect.h = 166;

   for (i = 0; i < m_iNumDeskCard; i++) {
      if (m_DeskCards[i].GetMonth() == month) {
         if (count >= 2) {
            TerminateOnError("CGame::SelectCardOnDesk(): count >= 2");
         }
         index[count] = i;
         b[count] = std::make_unique<CButton>(count, 140 + (i / 2) * 48,
            100 + (i & 1) * 78, 48, 78, 0, 0, 0);
         count++;
         m_DeskCards[i].m_iRenderEffect |= EF_BOX;
         General()->DrawCard(m_DeskCards[i], 140 + (i / 2) * 48,
            100 + (i & 1) * 78, 48, 78, true);
      }
   }

   count = -1;

   while (1) {
      int k = General()->ReadKey();
      if (k >= 1000) {
         count = k - 1000;
         break;
      }
   }

   b[0].reset();
   b[1].reset();

   General()->UpdateScreen(dstrect.x, dstrect.y, dstrect.w, dstrect.h);
   SDL_DestroySurface(save);

   for (i = 0; i < m_iNumDeskCard; i++) {
      m_DeskCards[i].m_iRenderEffect &= ~(EF_DARK | EF_BOX);
   }

   return index[count];
}

void CGame::InitScreen()
{
   int i;
   CCard c;

   // clear the screen
   General()->ClearScreen(false, false, false);

   // draw the card pile
   c = 255;
   for (i = 0; i < 5; i++) {
      General()->DrawCard(c, 50 + i * 2, 95 + i * 2, 48, 78, true);
   }

   DrawScore();
}

void CGame::RedrawTable()
{
   InitScreen();
   if (m_pPlayers[0] != nullptr) {
      m_pPlayers[0]->DrawHand();
      m_pPlayers[0]->DrawCaptured();
   }
   if (m_pPlayers[1] != nullptr) {
      m_pPlayers[1]->DrawHand();
      m_pPlayers[1]->DrawCaptured();
   }
   DrawDeskCard();
   DrawScore();
}

void CGame::AnimDeal()
{
   if (Screen() == nullptr) return;

   SDL_Surface *save = SDL_CreateSurface(48, 78, Screen()->format);
   SDL_Surface *card = nullptr;

   int i, j, k, l;

   for (i = 0; i < 2; i++) {
      for (j = 0; j < 3; j++) {
         k = ((CBasePlayer::GetDealer() == m_pPlayers[1].get()) ? j : 1 - j);
         for (l = 0; l < ((m_iGameMode == GAMEMODE_BET && j < 2) ? 3 : 4); l++) {
            CCard c;
            if (j < 2) {
               if (k != 0 && k != 1) {
                  TerminateOnError("CGame::AnimDeal(): k != 0 && k != 1");
               }
               if (!m_pPlayers[k]->IsBot()) {
                  c = m_pPlayers[k]->GetHandCard(i * ((m_iGameMode == GAMEMODE_BET) ? 3 : 4) + l);
               } else {
                  c = CCard(255);
               }
            } else {
               c = m_DeskCards[l + i * 4];
            }

            card = General()->RenderCard(c, 48, 78);

            Uint64 first = SDL_GetTicks(), now = first;
            SDL_Rect dstrect, dstrect2;

            dstrect.x = 60;
            dstrect.y = 105;
            dstrect.w = dstrect2.w = card->w;
            dstrect.h = dstrect2.h = card->h;

            if (j < 2) {
               dstrect2.x = 10 + (l + i * ((m_iGameMode == GAMEMODE_BET) ? 3 : 4)) * 48;
               dstrect2.y = k ? 10 : 400;
            } else {
               dstrect2.x = 140 + (l / 2 + i * 2) * 48;
               dstrect2.y = 100 + (l & 1) * 78;
            }

            SDL_Rect prev_dstrect3 = dstrect;
            if (save != nullptr) {
               SDL_BlitSurface(Screen(), &prev_dstrect3, save, NULL);
            }
            do {
               SDL_Rect dstrect3;
               dstrect3.w = card->w;
               dstrect3.h = card->h;

               float ratio = (now - first) / m_flAnimDuration;
               dstrect3.x = (int)(dstrect.x + (dstrect2.x - dstrect.x) * ratio);
               dstrect3.y = (int)(dstrect.y + (dstrect2.y - dstrect.y) * ratio);

               if (save != nullptr) {
                  SDL_BlitSurface(save, NULL, Screen(), &prev_dstrect3);
                  SDL_BlitSurface(Screen(), &dstrect3, save, NULL);
               }

               SDL_BlitSurface(card, NULL, Screen(), &dstrect3);
               General()->UpdateScreen(prev_dstrect3.x, prev_dstrect3.y,
                  prev_dstrect3.w, prev_dstrect3.h);
               General()->UpdateScreen(dstrect3.x, dstrect3.y,
                  dstrect3.w, dstrect3.h);

               SDL_Delay(5);
               now = SDL_GetTicks();
               prev_dstrect3 = dstrect3;
            } while (now < first + (Uint64)m_flAnimDuration);

            if (save != nullptr) {
               SDL_BlitSurface(save, NULL, Screen(), &prev_dstrect3);
            }
            SDL_BlitSurface(card, NULL, Screen(), &dstrect2);
            General()->UpdateScreen(prev_dstrect3.x, prev_dstrect3.y,
               prev_dstrect3.w, prev_dstrect3.h);
            General()->UpdateScreen(dstrect2.x, dstrect2.y,
               dstrect2.w, dstrect2.h);
            if (card != nullptr) {
               SDL_DestroySurface(card);
            }
            General()->PlaySound(SOUND_DRAWCARD);
         }
         UTIL_Delay(200);
      }
   }

   if (save != nullptr) {
      SDL_DestroySurface(save);
   }

   General()->ClearPromptArea();
   CBox box(20, 260, 595, 50, 40, 55, 85);
   General()->PlaySound(SOUND_HINT);
   General()->DrawTextInBox((CBasePlayer::GetDealer() == m_pPlayers[1].get()) ?
      msg("comdealer") : msg("youdealer"), 20, 260, 595, 50, 255, 255, 255, 18);
   UTIL_Delay(3500);
}

SDL_Surface *CGame::AnimCardMove(int sx, int sy, int dx, int dy,
   int w, int h, SDL_Surface *save, bool retsave, bool retcard)
{
   if (Screen() == nullptr) return nullptr;

   Uint64 first = SDL_GetTicks(), now = first;
   SDL_Rect dstrect, dstrect2;

   SDL_Surface *card = SDL_CreateSurface(w, h, Screen()->format);

   if (save == NULL) {
      save = SDL_CreateSurface(w, h, Screen()->format);
      SDL_Rect r = { sx, sy, w, h };
      SDL_BlitSurface(Screen(), &r, save, NULL);
   }

   dstrect.x = sx;
   dstrect.y = sy;
   dstrect.w = dstrect2.w = w;
   dstrect.h = dstrect2.h = h;

   SDL_BlitSurface(Screen(), &dstrect, card, NULL);

   if (sx == 60 && sy == 105) {
      General()->DrawCard(CCard(255), 60, 105, 48, 78, false);
   }

   dstrect2.x = dx;
   dstrect2.y = dy;

   SDL_Rect prev_dstrect3 = dstrect;
   do {
      SDL_Rect dstrect3;
      dstrect3.w = w;
      dstrect3.h = h;

      float ratio = (now - first) / m_flAnimDuration;
      dstrect3.x = (int)(dstrect.x + (dstrect2.x - dstrect.x) * ratio);
      dstrect3.y = (int)(dstrect.y + (dstrect2.y - dstrect.y) * ratio);

      if (save != nullptr) {
         SDL_BlitSurface(save, NULL, Screen(), &prev_dstrect3);
         SDL_BlitSurface(Screen(), &dstrect3, save, NULL);
      }

      SDL_BlitSurface(card, NULL, Screen(), &dstrect3);
      General()->UpdateScreen();

      SDL_Delay(5);
      now = SDL_GetTicks();
      prev_dstrect3 = dstrect3;
   } while (now < first + (Uint64)m_flAnimDuration);

   if (save != nullptr) {
      SDL_BlitSurface(save, NULL, Screen(), &prev_dstrect3);
   }

   if (retsave && save != nullptr) {
      SDL_BlitSurface(Screen(), &dstrect2, save, NULL);
   } else if (save != nullptr) {
      SDL_DestroySurface(save);
      save = NULL;
   }

   SDL_BlitSurface(card, NULL, Screen(), &dstrect2);
   General()->UpdateScreen(prev_dstrect3.x, prev_dstrect3.y,
      prev_dstrect3.w, prev_dstrect3.h);
   General()->UpdateScreen(dstrect2.x, dstrect2.y,
      dstrect2.w, dstrect2.h);

   if (retcard) {
      return card;
   }

   SDL_DestroySurface(card);
   return save;
}

void CGame::DrawScore()
{
   if (Screen() != nullptr) {
      UTIL_FillRect(Screen(), 10, 190, 120, 55, 30, 130, 100);
   }
   CBox s(10, 190, 120, 55, 0, 175, 0, 160, true);
   {
      std::string scoreTxt = std::format("SCORE  {}", m_iScore);
      General()->DrawTextBrush(scoreTxt.c_str(), 15, 195, 255, 255, 0, 16);
   }
   {
      std::string rdTxt = std::format("RD {}/12", m_iCurrentRound > 12 ? 12 : m_iCurrentRound);
      General()->DrawTextBrush(rdTxt.c_str(), 15, 220, 0, 255, 255, 16);
   }
   General()->UpdateScreen(10, 190, 120, 55);
}

void CGame::ShowMatchResults()
{
   General()->ClearScreen();

   CBox mainbox(40, 120, 560, 240, 40, 55, 85);
   CBox titlebox(40, 120, 560, 50, 0, 128, 128);

   std::string header;
   int r, g, b;

   if (m_iScore > 0) {
      header = msg("match_victory");
      r = 0; g = 255; b = 0;
      General()->PlaySound(SOUND_WIN);
   } else if (m_iScore < 0) {
      header = msg("match_defeat");
      r = 255; g = 50; b = 50;
      General()->PlaySound(SOUND_LOSE);
   } else {
      header = msg("match_draw");
      r = 255; g = 255; b = 0;
   }

   General()->DrawText(header.c_str(), 180, 128, r, g, b, 36);
   {
      std::string scoreText = std::vformat(msg("final_score_fmt"), std::make_format_args(m_iScore));
      General()->DrawText(scoreText.c_str(), 140, 190, 255, 255, 255, 26);
   }
   
   if (m_iScore > 0) {
      General()->DrawText(msg("congrats_win"), 60, 250, 255, 255, 0, 22);
   } else if (m_iScore < 0) {
      General()->DrawText(msg("better_luck_lose"), 60, 250, 255, 150, 150, 22);
   } else {
      General()->DrawText(msg("tie_game"), 180, 250, 255, 255, 0, 22);
   }

   CButton okbtn(1, 240, 300, 160, 40, 58, 110, 165);
   General()->DrawTextInBox("OK", 240, 300, 160, 40, 255, 255, 255, 24);
   General()->UpdateScreen();
   // Flush any pending events before waiting for OK to avoid stray auto-advance
   SDL_FlushEvents(SDL_EVENT_MOUSE_BUTTON_DOWN, SDL_EVENT_MOUSE_BUTTON_UP);
   SDL_FlushEvent(SDL_EVENT_KEY_DOWN);
   while (1) {
      int k = General()->ReadKey();
      if (k == 1001 || k == SDLK_RETURN || k == SDLK_ESCAPE || k == SDLK_SPACE) {
         break;
      }
   }
   // Flush again before returning to main menu so next ReadKey starts clean
   SDL_FlushEvents(SDL_EVENT_MOUSE_BUTTON_DOWN, SDL_EVENT_MOUSE_BUTTON_UP);
   SDL_FlushEvent(SDL_EVENT_KEY_DOWN);
}

void CGame::DrawDeskCard()
{
   SDL_Rect dstrect;

   dstrect.w = 48;
   dstrect.h = 78;

   int i;

   for (i = 0; i < 20; i++) {
      dstrect.x = 140 + (i / 2) * 48;
      dstrect.y = 100 + (i & 1) * 78;
      if (i < m_iNumDeskCard && m_DeskCards[i].IsValid()) {
         General()->DrawCard(m_DeskCards[i], dstrect.x, dstrect.y,
            48, 78, false);
      } else {
         if (Screen() != nullptr) {
            UTIL_FillRect(Screen(), dstrect.x, dstrect.y, dstrect.w, dstrect.h, 30, 130, 100);
         }
      }
   }

   if (Screen() != nullptr) {
      UTIL_FillRect(Screen(), 140, 100 + 78 * 2, 48 * 10, 10, 30, 130, 100);
   }

   General()->UpdateScreen(140, 100, 48 * 10, 78 * 2 + 10);
}

bool CGame::DoubleUp(CBasePlayer *player)
{
   auto mainbox = std::make_unique<CBox>(20, 250, 595, 100, 40, 55, 85);
   auto yesbtn = std::make_unique<CButton>(1, 40, 300, 140, 38, 58, 110, 165);
   auto nobtn = std::make_unique<CButton>(2, 200, 300, 140, 38, 165, 58, 58);
   bool ret = true;

   General()->DrawTextInBox(msg("doubleupyesorno"), 20, 255, 595, 35, 255, 255, 0, 20);
   General()->DrawTextInBox(msg("yes"), 40, 300, 140, 38, 255, 255, 255, 20);
   General()->DrawTextInBox(msg("no"), 200, 300, 140, 38, 255, 255, 255, 20);

   while (1) {
      int k = General()->ReadKey();
      if (k > 1000) {
         if (k == 1000 + 2) {
            // the "no" button is clicked
            ret = false;
         }
         break;
      }
   }

   yesbtn.reset();
   nobtn.reset();
   mainbox.reset();

   if (!ret) {
      return false;
   }

   General()->ClearScreen();

   // draw the card pile
   CCard c(255);
   int i;
   for (i = 0; i < 5; i++) {
      General()->DrawCard(c, 50 + i * 2, 95 + i * 2, 48, 78, true);
   }

   c = RandomLong(0, 47);

   mainbox = std::make_unique<CBox>(20, 250, 595, 100, 40, 55, 85);
   yesbtn = std::make_unique<CButton>(1, 40, 300, 140, 38, 58, 110, 165);
   nobtn = std::make_unique<CButton>(2, 200, 300, 140, 38, 165, 58, 58);

   General()->DrawTextInBox(msg("bigorsmall"), 20, 255, 595, 35, 255, 255, 0, 20);
   General()->DrawTextInBox(msg("big"), 40, 300, 140, 38, 255, 255, 255, 20);
   General()->DrawTextInBox(msg("small"), 200, 300, 140, 38, 255, 255, 255, 20);

   CBox s(25, 190, 110, 70, 0, 175, 0, 160);
   General()->DrawTextBrush("WIN", 30, 190, 255, 255, 0, 32);
   {
      std::string scoreStr = std::format("{:6d}", player->m_Result.score);
      General()->DrawTextBrush(scoreStr.c_str(), 30, 220);
   }
   General()->UpdateScreen(25, 190, 110, 70);

   bool isbig = true;
   while (1) {
      int k = General()->ReadKey();
      if (k > 1000) {
         if (k == 1000 + 2) {
            // the "small" button is clicked
            isbig = false;
         }
         break;
      }
   }

   yesbtn.reset();
   nobtn.reset();
   mainbox.reset();

   SDL_Surface *save = SDL_CreateSurface(48, 78, SDL_PIXELFORMAT_RGBA8888);

   SDL_Rect dstrect;
   dstrect.x = 60;
   dstrect.y = 105;
   dstrect.w = 48;
   dstrect.h = 78;

   General()->DrawCard(c, 60, 105, 48, 78);
   General()->PlaySound(SOUND_DRAWCARD);
   AnimCardMove(60, 105, 150, 115, 48, 78, save);
   SDL_DestroySurface(save);
   UTIL_Delay(800);

   if ((isbig && c.GetMonth() >= 7) || (!isbig && c.GetMonth() < 7)) {
      ret = true;
      player->m_Result.score *= 2;
      General()->DrawTextBrush("WIN", 210, 125, 255, 255, 128, 64);
      General()->PlaySound(SOUND_WIN);
   } else {
      ret = false;
      player->m_Result.score = 0;
      General()->DrawTextBrush("LOSE", 210, 125, 128, 255, 255, 64);
      General()->PlaySound(SOUND_LOSE);
   }

   UTIL_Delay(2000);
   return ret;
}

int CGame::FindFreeDeskCardSlot(int exclude)
{
   int i;
   for (i = 0; i < m_iNumDeskCard; i++) {
      if (exclude != -1 && i == exclude)
         continue;
      if (!m_DeskCards[i].IsValid() && i != exclude) {
         return i;
      }
   }
   return i;
}

void CGame::RemoveDeskCard(int index)
{
   m_DeskCards[index].Destroy();
}

int CGame::FindMatchingCards(const CCard &card, int indices[3]) const
{
   int count = 0;
   for (int i = 0; i < m_iNumDeskCard; i++) {
      if (card == m_DeskCards[i]) {
         if (count < 3) indices[count] = i;
         count++;
      }
   }
   for (int i = count; i < 3; i++) indices[i] = -1;
   return count;
}

int CGame::ChooseSlotForPair(int idx0, int idx1, const CCard &played, CBasePlayer *player) const
{
   if (m_DeskCards[idx0].GetType() == m_DeskCards[idx1].GetType()) {
      if (m_DeskCards[idx1].GetValue() == 43 || m_DeskCards[idx1].GetValue() == 45) {
         return idx1;
      }
      return idx0;
   }
   return player->SelectCardOnDesk(m_DeskCards[idx0].GetMonth(), played);
}

void CGame::CapturePair(CBasePlayer *player, const CCard &played, int deskIdx)
{
   player->AddCapturedCard(played);
   player->AddCapturedCard(m_DeskCards[deskIdx]);
   m_DeskCards[deskIdx].Destroy();
}

void CGame::CaptureTriple(CBasePlayer *player, const CCard &played, int idx0, int idx1, int idx2)
{
   player->AddCapturedCard(played);
   player->AddCapturedCard(m_DeskCards[idx0]);
   player->AddCapturedCard(m_DeskCards[idx1]);
   player->AddCapturedCard(m_DeskCards[idx2]);
   m_DeskCards[idx0].Destroy();
   m_DeskCards[idx1].Destroy();
   m_DeskCards[idx2].Destroy();
}

void CGame::ValidateInitialDesk()
{
   bool allpairs = true;
   for (int i = 0; i < m_iNumDeskCard; i++) {
      int count = 0;
      for (int j = 0; j < m_iNumDeskCard; j++) {
         if (m_DeskCards[i] == m_DeskCards[j]) count++;
      }
      if (count != 2) allpairs = false;
      if (count >= 3) {
         int count2 = 999;
         while (count2 >= 3) {
            CCard::PutBackToPile(m_DeskCards[i]);
            m_DeskCards[i] = CCard::GetRandomCard();
            count2 = 0;
            for (int k = 0; k < m_iNumDeskCard; k++) {
               if (m_DeskCards[i] == m_DeskCards[k]) count2++;
            }
         }
      }
   }
   if (allpairs) {
      int index = RandomLong(0, m_iNumDeskCard - 1);
      int count = 999;
      while (count >= 2) {
         CCard::PutBackToPile(m_DeskCards[index]);
         m_DeskCards[index] = CCard::GetRandomCard();
         count = 0;
         for (int k = 0; k < m_iNumDeskCard; k++) {
            if (m_DeskCards[index] == m_DeskCards[k]) count++;
         }
      }
   }
}

void CGame::HandleDiscardPhase(const CCard& c, const CCard& drawn, CBasePlayer* cur, int sx, int sy, PhaseState& st)
{
   int index[3] = {-1,-1,-1};
   int count = FindMatchingCards(c, index);
   if (count >= 4) TerminateOnError("CGame::CardDiscarded(): count >= 3");
   if (count <= 0) {
      st.slot = FindFreeDeskCardSlot();
      int dx = 140 + 48 * (st.slot / 2);
      int dy = 100 + 78 * (st.slot & 1);
      AnimCardMove(sx, sy, dx, dy);
      General()->PlaySound(SOUND_MOVECARD);
      m_DeskCards[st.slot] = c;
      if (st.slot >= m_iNumDeskCard) m_iNumDeskCard = st.slot + 1;
      UTIL_Delay(200);
      st.slot = 999;
      if (GetGameMode() == GAMEMODE_KOREAN) st.getfourMonth = c.GetMonth();
   } else if (count == 1) {
      st.slot = index[0];
      st.x1 = 140 + 48 * (st.slot / 2) + 10;
      st.y1 = 100 + 78 * (st.slot & 1) + 10;
      st.save1 = AnimCardMove(sx, sy, st.x1, st.y1, 48, 78, NULL, true);
      General()->PlaySound(SOUND_PICKCARD);
      UTIL_Delay(200);
      if (GetGameMode() == GAMEMODE_KOREAN && c == drawn && cur->GetNumHandCard() > 1) {
         SDL_DestroySurface(st.save1);
         st.save1 = NULL;
         int slot1 = FindFreeDeskCardSlot();
         m_DeskCards[slot1] = c;
         if (slot1 >= m_iNumDeskCard) m_iNumDeskCard = slot1 + 1;
         st.leavethree = true;
         st.slot = 999;
      } else {
         CapturePair(cur, c, st.slot);
      }
   } else if (GetGameMode() == GAMEMODE_KOREAN && count >= 3) {
      st.x1 = 140 + 48 * (index[0] / 2) + 10;
      st.y1 = 100 + 78 * (index[0] & 1) + 10;
      st.save1 = AnimCardMove(sx, sy, st.x1, st.y1, 48, 78, NULL, true);
      General()->PlaySound(SOUND_PICKCARD);
      UTIL_Delay(200);
      AnimCardMove(st.x1, st.y1, 575, cur->IsBot() ? 10 : 400, 48, 78, st.save1);
      UTIL_Delay(50);
      for (int k = 0; k < 3; k++) {
         int tx = 140 + 48 * (index[k] / 2);
         int ty = 100 + 78 * (index[k] & 1);
         General()->PlaySound(SOUND_MOVECARD);
         AnimCardMove(tx, ty, 575, cur->IsBot() ? 10 : 400);
         UTIL_Delay(50);
      }
      CaptureTriple(cur, c, index[0], index[1], index[2]);
      st.save1 = NULL;
      st.slot = 999;
      if (cur->GetNumHandCard() > 1 && cur->GetOpponent()->GetNumHandCard() > 0) GetOneCardFromOpponent(cur);
   } else if (count == 2) {
      st.slot = ChooseSlotForPair(index[0], index[1], c, cur);
      st.x1 = 140 + 48 * (st.slot / 2) + 10;
      st.y1 = 100 + 78 * (st.slot & 1) + 10;
      st.save1 = AnimCardMove(sx, sy, st.x1, st.y1, 48, 78, NULL, true);
      General()->PlaySound(SOUND_PICKCARD);
      UTIL_Delay(200);
      CapturePair(cur, c, st.slot);
      if (GetGameMode() == GAMEMODE_KOREAN) st.getfourMonth = c.GetMonth();
   }
}

void CGame::FixupOverlappedSlot(SDL_Surface* save1, SDL_Surface* card2, int oldSlot, int newSlot, int dx, int dy)
{
   if (save1 == NULL || card2 == NULL) return;
   if (newSlot == oldSlot + 1 && !(oldSlot & 1)) {
      SDL_Rect src{10,0,38,10}, dst{0,68,38,10};
      SDL_BlitSurface(card2, &src, save1, &dst);
      SDL_Rect upd{dx+10, dy, 38, 10};
      General()->UpdateScreen(upd.x, upd.y, upd.w, upd.h);
   } else if (newSlot == oldSlot + 2) {
      SDL_Rect src{0,10,10,68}, dst{38,0,10,68};
      SDL_BlitSurface(card2, &src, save1, &dst);
      SDL_Rect upd{dx, dy+10, 10, 68};
      General()->UpdateScreen(upd.x, upd.y, upd.w, upd.h);
   } else if (newSlot == oldSlot + 3 && !(oldSlot & 1)) {
      SDL_Rect src{0,0,10,10}, dst{38,68,10,10};
      SDL_BlitSurface(card2, &src, save1, &dst);
      SDL_Rect upd{dx, dy, 10, 10};
      General()->UpdateScreen(upd.x, upd.y, upd.w, upd.h);
   }
}

void CGame::HandleDrawnPhase(const CCard& drawn, CBasePlayer* cur, PhaseState& st)
{
   int index[3] = {-1,-1,-1};
   int count = FindMatchingCards(drawn, index);
   if (count >= 4) TerminateOnError("CGame::CardDiscarded(): count >= 3");
   if (count <= 0 || st.leavethree) {
      int oldSlot = st.slot;
      st.slot = FindFreeDeskCardSlot(oldSlot);
      int dx = 140 + 48 * (st.slot / 2);
      int dy = 100 + 78 * (st.slot & 1);
      SDL_Surface* card2 = AnimCardMove(60, 105, dx, dy, 48, 78, st.save2, true, true);
      General()->PlaySound(SOUND_MOVECARD);
      m_DeskCards[st.slot] = drawn;
      if (st.slot >= m_iNumDeskCard) m_iNumDeskCard = st.slot + 1;
      FixupOverlappedSlot(st.save1, card2, oldSlot, st.slot, dx, dy);
      if (st.save2) { SDL_DestroySurface(st.save2); st.save2 = NULL; }
      if (card2) { SDL_DestroySurface(card2); card2 = NULL; }
      UTIL_Delay(200);
      if (GetGameMode() == GAMEMODE_KOREAN && st.leavethree) {
         cur->m_iNumLeaveThree++;
         if (cur->GetNumHandCard() >= 8) {
            General()->ClearPromptArea();
            CBox box(20, 260, 595, 50, 40, 55, 85);
            General()->PlaySound(SOUND_HINT);
            if (cur->IsBot()) { General()->DrawTextInBox(msg("comget3pts"), 20, 260, 595, 50, 255, 255, 255, 18); m_iScore -= 3; }
            else { General()->DrawTextInBox(msg("youget3pts"), 20, 260, 595, 50, 255, 255, 255, 18); m_iScore += 3; }
            UTIL_Delay(3500);
            DrawScore();
         }
      }
   } else if (count == 1) {
      st.x2 = 140 + 48 * (index[0] / 2) + 10;
      st.y2 = 100 + 78 * (index[0] & 1) + 10;
      st.save2 = AnimCardMove(60, 105, st.x2, st.y2, 48, 78, st.save2, true);
      General()->PlaySound(SOUND_PICKCARD);
      UTIL_Delay(200);
      if (drawn.GetMonth() == st.getfourMonth) {
         if (cur->GetNumHandCard() > 1 && cur->GetOpponent()->GetNumHandCard() > 0) GetOneCardFromOpponent(cur);
      }
      CapturePair(cur, drawn, index[0]);
   } else if (GetGameMode() == GAMEMODE_KOREAN && count >= 3) {
      st.x2 = 140 + 48 * (index[0] / 2) + 10;
      st.y2 = 100 + 78 * (index[0] & 1) + 10;
      st.save2 = AnimCardMove(60, 105, st.x2, st.y2, 48, 78, st.save2, true);
      General()->PlaySound(SOUND_PICKCARD);
      UTIL_Delay(200);
      if (st.save1 && st.save2 && (st.x1 < st.x2 || st.y1 < st.y2)) {
         std::swap(st.save1, st.save2);
         std::swap(st.x1, st.x2);
         std::swap(st.y1, st.y2);
      }
      General()->PlaySound(SOUND_MOVECARD);
      AnimCardMove(st.x2, st.y2, 575, cur->IsBot() ? 10 : 400, 48, 78, st.save2);
      UTIL_Delay(50);
      General()->PlaySound(SOUND_MOVECARD);
      AnimCardMove(st.x2 - 10, st.y2 - 10, 575, cur->IsBot() ? 10 : 400);
      UTIL_Delay(50);
      st.save2 = NULL;
      if (st.save1) {
         General()->PlaySound(SOUND_MOVECARD);
         AnimCardMove(st.x1, st.y1, 575, cur->IsBot() ? 10 : 400, 48, 78, st.save1);
         UTIL_Delay(50);
         General()->PlaySound(SOUND_MOVECARD);
         AnimCardMove(st.x1 - 10, st.y1 - 10, 575, cur->IsBot() ? 10 : 400);
         UTIL_Delay(50);
         st.save1 = NULL;
      }
      for (int k = 1; k < 3; k++) {
         int tx = 140 + 48 * (index[k] / 2);
         int ty = 100 + 78 * (index[k] & 1);
         General()->PlaySound(SOUND_MOVECARD);
         AnimCardMove(tx, ty, 575, cur->IsBot() ? 10 : 400);
         UTIL_Delay(50);
      }
      CaptureTriple(cur, drawn, index[0], index[1], index[2]);
      if (cur->GetNumHandCard() > 1 && cur->GetOpponent()->GetNumHandCard() > 0) GetOneCardFromOpponent(cur);
   } else if (count == 2) {
      st.slot = ChooseSlotForPair(index[0], index[1], drawn, cur);
      st.x2 = 140 + 48 * (st.slot / 2) + 10;
      st.y2 = 100 + 78 * (st.slot & 1) + 10;
      st.save2 = AnimCardMove(60, 105, st.x2, st.y2, 48, 78, st.save2, true);
      General()->PlaySound(SOUND_PICKCARD);
      UTIL_Delay(200);
      CapturePair(cur, drawn, st.slot);
   }
}

void CGame::AnimatePendingCaptures(PhaseState& st, CBasePlayer* cur)
{
   UTIL_Delay(200);
   if (st.save1 && st.save2 && (st.x1 < st.x2 || st.y1 < st.y2)) {
      std::swap(st.save1, st.save2);
      std::swap(st.x1, st.x2);
      std::swap(st.y1, st.y2);
   }
   if (st.save2) {
      General()->PlaySound(SOUND_MOVECARD);
      AnimCardMove(st.x2, st.y2, 575, cur->IsBot() ? 10 : 400, 48, 78, st.save2);
      UTIL_Delay(50);
      General()->PlaySound(SOUND_MOVECARD);
      AnimCardMove(st.x2 - 10, st.y2 - 10, 575, cur->IsBot() ? 10 : 400);
      UTIL_Delay(50);
   }
   if (st.save1) {
      General()->PlaySound(SOUND_MOVECARD);
      AnimCardMove(st.x1, st.y1, 575, cur->IsBot() ? 10 : 400, 48, 78, st.save1);
      UTIL_Delay(50);
      General()->PlaySound(SOUND_MOVECARD);
      AnimCardMove(st.x1 - 10, st.y1 - 10, 575, cur->IsBot() ? 10 : 400);
      UTIL_Delay(50);
   }
   if (GetGameMode() == GAMEMODE_KOREAN) {
      int i;
      for (i = 0; i < GetNumDeskCard(); i++) if (GetDeskCard(i).IsValid()) break;
      if (i >= GetNumDeskCard()) {
         if (cur->GetNumHandCard() > 1 && cur->GetOpponent()->GetNumHandCard() > 0) GetOneCardFromOpponent(cur);
      }
   }
}


