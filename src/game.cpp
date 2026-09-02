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

CGame::CGame()
{
   m_iGameMode = atoi(cfg.Get("GAME", "GameMode", "0"));
   if (m_iGameMode == GAMEMODE_BET) {
      CBasePlayer::m_iMaxHandCards = 6; // only 6 cards in bet mode
   }

   m_iScore = atoi(cfg.Get("GAME", "Score", "0"));
   m_flAnimDuration = atof(cfg.Get("OPTIONS", "AnimSpeed", "180"));
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
      gpGeneral->ClearScreen(true, false, true);

      auto mainbox = std::make_unique<CBox>(140, 200, 350, 260, 58, 110, 165);
      auto bNewGame = std::make_unique<CButton>(1, 150, 210, 330, 50, 58, 110, 165);
      auto bOption = std::make_unique<CButton>(2, 150, 270, 330, 50, 58, 110, 165);
      auto bRules = std::make_unique<CButton>(3, 150, 330, 330, 50, 58, 110, 165);
      auto bQuit = std::make_unique<CButton>(4, 150, 390, 330, 50, 58, 110, 165);

      gpGeneral->DrawTextInBox(msg("play"), 150, 210, 330, 50, 255, 255, 0, 26);
      gpGeneral->DrawTextInBox(msg("settings"), 150, 270, 330, 50, 255, 255, 0, 26);
      gpGeneral->DrawTextInBox(msg("btn_rules"), 150, 330, 330, 50, 255, 255, 0, 26);
      gpGeneral->DrawTextInBox(msg("quit"), 150, 390, 330, 50, 255, 255, 0, 26);

      int choice = -1;
      while (1) {
         int k = gpGeneral->ReadKey();
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

   SDL_Surface *canvas = SDL_CreateSurface(canvas_w, canvas_h, SDL_PIXELFORMAT_RGBA8888);
   if (canvas == nullptr) return;

   SDL_FillSurfaceRect(canvas, NULL, SDL_MapSurfaceRGBA(canvas, 20, 32, 52, 255));

   auto DrawCanvasText = [&](const char *txt, int x, int y, int w, int r, int g, int b, int size) -> int {
      if (txt == nullptr) return y + 20;
      SDL_Surface *s = gpGeneral->RenderTextWrapped(txt, r, g, b, size, w);
      int rendered_h = 20;
      if (s != nullptr) {
         SDL_Rect dst = { x, y, s->w, s->h };
         SDL_BlitSurface(s, NULL, canvas, &dst);
         rendered_h = s->h;
         SDL_DestroySurface(s);
      }
      return y + rendered_h + 8;
   };

   auto DrawCanvasCard = [&](int card_id, int x, int y, int w = 42, int h = 66) {
      SDL_Surface *card_surf = gpGeneral->RenderCard(CCard(card_id), w, h);
      if (card_surf != nullptr) {
         SDL_Rect dst = { x, y, w, h };
         SDL_BlitSurface(card_surf, NULL, canvas, &dst);
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

   gpGeneral->ClearScreen(true, false, true);

   auto mainbox = std::make_unique<CBox>(25, 15, 590, 450, 40, 55, 85, 255, true);
   auto titlebox = std::make_unique<CBox>(25, 15, 590, 45, 0, 128, 128, 255, true);
   auto viewportbox = std::make_unique<CBox>(35, 68, 545, 334, 20, 32, 52, 255, true);
   auto btnBack = std::make_unique<CButton>(3, 415, 412, 190, 42, 165, 58, 58);

   bool is_dragging_scroll = false;
   bool redraw = true;
   while (true) {
      if (redraw) {
         gpGeneral->DrawTextInBox(msg("rules_header"), 25, 15, 590, 45, 20, 32, 52, 22);

         SDL_Rect clip_rect = { 35, 68, 545, 334 };
         SDL_SetSurfaceClipRect(gpScreen, &clip_rect);

         SDL_Rect src_rect = { 0, scroll_y, 540, 330 };
         SDL_Rect dst_rect = { 38, 70, 540, 330 };
         SDL_BlitSurface(canvas, &src_rect, gpScreen, &dst_rect);

         SDL_SetSurfaceClipRect(gpScreen, NULL);

         UTIL_FillRect(gpScreen, 588, 70, 12, 330, 15, 25, 40);
         int thumb_h = 50;
         int thumb_y = 70 + (max_scroll > 0 ? (scroll_y * (330 - thumb_h)) / max_scroll : 0);
         UTIL_FillRect(gpScreen, 588, thumb_y, 12, thumb_h, 0, 200, 255);

         gpGeneral->DrawTextInBox("Back to Menu", 415, 412, 190, 42, 255, 255, 255, 18);

         gpGeneral->UpdateScreen();
         redraw = false;
      }

      SDL_Event event;
      if (SDL_WaitEvent(&event)) {
         if (gpRenderer != nullptr) {
            SDL_ConvertEventToRenderCoordinates(gpRenderer, &event);
         }
         if (event.type == SDL_EVENT_QUIT) {
            SDL_DestroySurface(canvas);
            return;
         } else if (event.type == SDL_EVENT_KEY_DOWN) {
            if (event.key.key == SDLK_UP) {
               scroll_y = (scroll_y - 80 < 0) ? 0 : (scroll_y - 80);
               redraw = true;
            } else if (event.key.key == SDLK_DOWN) {
               scroll_y = (scroll_y + 80 > max_scroll) ? max_scroll : (scroll_y + 80);
               redraw = true;
            } else if (event.key.key == SDLK_ESCAPE || event.key.key == SDLK_RETURN) {
               SDL_DestroySurface(canvas);
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
               SDL_DestroySurface(canvas);
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
   CButton fs(1, 280, 20, 150, 24, 0, 0, 0);
   CButton snd(2, 280, 50, 150, 24, 0, 0, 0);
   CButton gm(3, 280, 80, 150, 24, 0, 0, 0);
   CButton l(4, 280, 110, 150, 24, 0, 0, 0);
   CButton as(5, 280, 140, 150, 24, 0, 0, 0);
   CButton ok(6, 20, 180, 150, 34, 58, 110, 165);

   int curgm = atoi(cfg.Get("GAME", "GameMode", "0"));
   int curas = 3;

   char lngs[256], *curlng;
   strcpy(lngs, cfg.Get("OPTIONS", "AllLanguage", "eng"));
   curlng = strtok(lngs, ",");
   int valueas[5] = {800, 500, 300, 180, 50};

   while (1) {
      gpGeneral->ClearScreen();

      gpGeneral->DrawText(msg("fullscreen"), 20, 20, 255, 255, 0, 24);
      gpGeneral->DrawText(msg("enablesound"), 20, 50, 255, 255, 0, 24);
      gpGeneral->DrawText(msg("gamemode"), 20, 80, 255, 255, 0, 24);
      gpGeneral->DrawText(msg("language"), 20, 110, 255, 255, 0, 24);
      gpGeneral->DrawText(msg("animspeed"), 20, 140, 255, 255, 0, 24);
      gpGeneral->DrawTextInBox("OK", 20, 180, 150, 34, 255, 255, 255, 20);

      const char *strgm[3] = {msg("gamemode0"), msg("gamemode1"), msg("gamemode2")};
      const char *stras[5] = {msg("veryslow"), msg("slow"), msg("middle"), msg("fast"), msg("veryfast")};

      if (atoi(cfg.Get("OPTIONS", "FullScreen", "0"))) {
         gpGeneral->DrawText(msg("Enabled"), 280, 20, 255, 255, 255, 24);
      } else {
         gpGeneral->DrawText(msg("Disabled"), 280, 20, 255, 255, 255, 24);
      }

      if (!atoi(cfg.Get("OPTIONS", "NoSound", "0"))) {
         gpGeneral->DrawText(msg("Enabled"), 280, 50, 255, 255, 255, 24);
      } else {
         gpGeneral->DrawText(msg("Disabled"), 280, 50, 255, 255, 255, 24);
      }

      gpGeneral->DrawText(strgm[curgm], 280, 80, 255, 255, 255, 24);
      gpGeneral->DrawText(msg(cfg.Get("OPTIONS", "Language", "eng")), 280, 110, 255, 255, 255, 24);
      gpGeneral->DrawText(stras[curas], 280, 140, 255, 255, 255, 24);

      gpGeneral->UpdateScreen(0, 0, 640, 480);

      int k = gpGeneral->ReadKey();
      if (k > 1000) {
         switch (k - 1000) {
            case 1:
               if (atoi(cfg.Get("OPTIONS", "FullScreen", "0"))) {
                  cfg.Set("OPTIONS", "FullScreen", "0");
               } else {
                  cfg.Set("OPTIONS", "FullScreen", "1");
               }
               break;

            case 2:
               if (atoi(cfg.Get("OPTIONS", "NoSound", "0"))) {
                  cfg.Set("OPTIONS", "NoSound", "0");
               } else {
                  cfg.Set("OPTIONS", "NoSound", "1");
               }
               break;

            case 3:
               if (++curgm >= 3) {
                  curgm = 0;
               }
               cfg.Set("GAME", "GameMode", va("%d", curgm));
               m_iGameMode = curgm;
               if (m_iGameMode == GAMEMODE_BET) {
                  CBasePlayer::m_iMaxHandCards = 6;
               } else {
                  CBasePlayer::m_iMaxHandCards = 8;
               }
               break;

            case 4:
               cfg.Set("OPTIONS", "Language", curlng);
               curlng = strtok(NULL, ",");
               if (curlng == NULL) {
                  strcpy(lngs, cfg.Get("OPTIONS", "AllLanguage", "eng"));
                  curlng = strtok(lngs, ",");
               }
               InitTextMessage();
               gpGeneral->LoadFonts();
               break;

            case 5:
               if (++curas >= 5) {
                  curas = 0;
               }
               cfg.Set("OPTIONS", "AnimSpeed", va("%d", valueas[curas]));
               m_flAnimDuration = (float)valueas[curas];
               break;

            case 6:
               InitTextMessage();
               gpGeneral->LoadFonts();

               if (atoi(cfg.Get("OPTIONS", "NoSound", "0"))) {
                  g_fNoSound = true;
               } else {
                  g_fNoSound = false;
                  extern bool g_fAudioOpened;
                  if (!g_fAudioOpened) {
                     if (SOUND_OpenAudio(22050, SDL_AUDIO_S16, 1, 1024)) {
                        std::println(stderr, "WARNING: Couldn't open audio: {}", SDL_GetError());
                        g_fNoSound = true;
                     } else {
                        g_fAudioOpened = true;
                        gpGeneral->LoadSound();
                     }
                  } else {
                     gpGeneral->LoadSound();
                  }
               }

               bool fs_setting = (atoi(cfg.Get("OPTIONS", "FullScreen", "0")) > 0);
               bool fs_active = false;
               if (gpWindow != nullptr) {
                  fs_active = (SDL_GetWindowFlags(gpWindow) & SDL_WINDOW_FULLSCREEN) != 0;
               }
               if (fs_setting != fs_active) {
                  UTIL_ToggleFullScreen();
               }
               return; // exit settings
         }
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
      cfg.Set("GAME", "Score", va("%d", m_iScore));
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

   int m1 = c1.GetMonth() + 1;
   int m2 = c2.GetMonth() + 1;

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

   gpGeneral->ClearScreen(false, false, false);
   CCard pile(255);
   for (int i = 0; i < 5; i++) {
      gpGeneral->DrawCard(pile, 50 + i * 2, 95 + i * 2, 48, 78, true);
   }
   DrawScore();

   gpGeneral->DrawCard(c1, 240, 160, 48, 78, true);
   gpGeneral->DrawCard(c2, 350, 160, 48, 78, true);

   CBox box(20, 260, 595, 50, 40, 55, 85);
   const char *notice = player_first ?
      va(msg("dealer_cut_you"), m1, m2) :
      va(msg("dealer_cut_com"), m1, m2);

   gpGeneral->DrawTextInBox(notice, 20, 260, 595, 50, 255, 255, 255, 18);
   UTIL_Delay(3500);
}

void CGame::InitGame()
{
   m_iScore = 0;
   m_iCurrentRound = 1;
   cfg.Set("GAME", "Score", "0");

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
         gpGeneral->DrawCard(current->GetHandCard(s), x, y, 48, 78);
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
            gpGeneral->PlaySound(SOUND_GO);
            current->m_iNumContinue++;
            UTIL_Delay(1000);
         } else {
            break;
         }
      }

      if (GetGameMode() == GAMEMODE_KOREAN && current->m_iNumLeaveThree >= 3) {
         gpGeneral->ClearPromptArea();
         CBox box(20, 260, 595, 50, 40, 55, 85);
         if (current->IsBot()) {
            gpGeneral->DrawTextInBox(msg("comget5pts"), 20, 260, 595, 50, 255, 255, 255, 18);
            m_iScore -= 3;
            gpGeneral->PlaySound(SOUND_LOSE);
         } else {
            gpGeneral->DrawTextInBox(msg("youget5pts"), 20, 260, 595, 50, 255, 255, 255, 18);
            m_iScore += 3;
            gpGeneral->PlaySound(SOUND_WIN);
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
         gpGeneral->PlaySound(SOUND_LOSE);
         UTIL_Delay(2500);
      } else {
         gpGeneral->PlaySound(SOUND_WIN);
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
   // draw one card from the desk
   CCard drawn = CCard::GetRandomCard();

   SDL_Surface *save1 = NULL, *save2 = NULL;
   int x1 = 0, y1 = 0, x2 = 0, y2 = 0, getfour_month = -1;
   bool leavethree = false;

   int index[3] = {-1, -1, -1}, slot = -1;
   int i;
   int count = FindMatchingCards(c, index);
   if (count >= 4) TerminateOnError("CGame::CardDiscarded(): count >= 3");

   if (count <= 0) {
      // No card matches the discarded one. Just throw the
      // discarded one to the desk.
      slot = FindFreeDeskCardSlot();
      int dx = 140 + 48 * (slot / 2);
      int dy = 100 + 78 * (slot & 1);

      AnimCardMove(sx, sy, dx, dy);
      gpGeneral->PlaySound(SOUND_MOVECARD);

      m_DeskCards[slot] = c;
      if (slot >= m_iNumDeskCard) {
         m_iNumDeskCard = slot + 1;
      }

      UTIL_Delay(200);
      slot = 999;

      if (GetGameMode() == GAMEMODE_KOREAN) {
         getfour_month = c.GetMonth();
      }
   } else if (count == 1) {
      // Only one card matches the discarded one
      slot = index[0];

      x1 = 140 + 48 * (slot / 2) + 10;
      y1 = 100 + 78 * (slot & 1) + 10;

      save1 = AnimCardMove(sx, sy, x1, y1, 48, 78, NULL, true);
      gpGeneral->PlaySound(SOUND_PICKCARD);
      UTIL_Delay(200);

      if (GetGameMode() == GAMEMODE_KOREAN && c == drawn &&
         current->GetNumHandCard() > 1)
      {
         SDL_DestroySurface(save1);
         save1 = NULL;
         int slot1 = FindFreeDeskCardSlot();
         m_DeskCards[slot1] = c;
         if (slot1 >= m_iNumDeskCard) {
            m_iNumDeskCard = slot1 + 1;
         }
         leavethree = true;
         slot = 999;
       } else {
         CapturePair(current, c, slot);
      }
   } else if (GetGameMode() == GAMEMODE_KOREAN && count >= 3) {
      // Three cards match the discarded one. Pick all these three cards
      x1 = 140 + 48 * (index[0] / 2) + 10;
      y1 = 100 + 78 * (index[0] & 1) + 10;

      save1 = AnimCardMove(sx, sy, x1, y1, 48, 78, NULL, true);
      gpGeneral->PlaySound(SOUND_PICKCARD);
      UTIL_Delay(200);

      AnimCardMove(x1, y1, 575, current->IsBot() ? 10 : 400, 48, 78, save1);
      UTIL_Delay(50);

      for (int k = 0; k < 3; k++) {
         x1 = 140 + 48 * (index[k] / 2);
         y1 = 100 + 78 * (index[k] & 1);

         gpGeneral->PlaySound(SOUND_MOVECARD);
         AnimCardMove(x1, y1, 575, current->IsBot() ? 10 : 400);
         UTIL_Delay(50);
      }

      CaptureTriple(current, c, index[0], index[1], index[2]);

      save1 = NULL;
      slot = 999;

      if (current->GetNumHandCard() > 1 && current->GetOpponent()->GetNumHandCard() > 0) {
         GetOneCardFromOpponent(current);
      }
   } else if (count == 2) {
      slot = ChooseSlotForPair(index[0], index[1], c, current);

      x1 = 140 + 48 * (slot / 2) + 10;
      y1 = 100 + 78 * (slot & 1) + 10;

      save1 = AnimCardMove(sx, sy, x1, y1, 48, 78, NULL, true);
      gpGeneral->PlaySound(SOUND_PICKCARD);
      UTIL_Delay(200);

      CapturePair(current, c, slot);

      if (GetGameMode() == GAMEMODE_KOREAN) {
         getfour_month = c.GetMonth();
      }
   }

   // Draw the drawn card
   save2 = SDL_CreateSurface(48, 78, SDL_PIXELFORMAT_RGBA8888);

   SDL_Rect dstrect;
   dstrect.x = 60;
   dstrect.y = 105;
   dstrect.w = 48;
   dstrect.h = 78;

   UTIL_Delay(200);
   gpGeneral->DrawCard(drawn, 60, 105, 48, 78, true);
   UTIL_Delay(200);

   count = FindMatchingCards(drawn, index);
   if (count >= 4) TerminateOnError("CGame::CardDiscarded(): count >= 3");

   if (count <= 0 || leavethree) {
      // No card matches the discarded one. Just throw the
      // discarded one to the desk.
      int oldslot = slot;
      slot = FindFreeDeskCardSlot(oldslot);
      int dx = 140 + 48 * (slot / 2);
      int dy = 100 + 78 * (slot & 1);

      SDL_Surface *card2 = AnimCardMove(60, 105, dx, dy, 48, 78, save2, true, true);
      gpGeneral->PlaySound(SOUND_MOVECARD);

      m_DeskCards[slot] = drawn;
      if (slot >= m_iNumDeskCard) {
         m_iNumDeskCard = slot + 1;
      }

      if (slot == oldslot + 1 && !(oldslot & 1)) {
         assert(save1 != NULL);
         SDL_Rect dstrect2;

         dstrect.x = 10;
         dstrect.y = 0;
         dstrect.w = 38;
         dstrect.h = 10;

         dstrect2.x = 0;
         dstrect2.y = 68;
         dstrect2.w = 38;
         dstrect2.h = 10;

         if (card2 != NULL && save1 != NULL) {
            SDL_BlitSurface(card2, &dstrect, save1, &dstrect2);
         }

         dstrect2.x = dx + 10;
         dstrect2.y = dy;

         gpGeneral->UpdateScreen(dstrect2.x, dstrect2.y, dstrect2.w, dstrect2.h);
      } else if (slot == oldslot + 2) {
         SDL_Rect dstrect2;

         dstrect.x = 0;
         dstrect.y = 10;
         dstrect.w = 10;
         dstrect.h = 68;

         dstrect2.x = 38;
         dstrect2.y = 0;
         dstrect2.w = 10;
         dstrect2.h = 68;

         if (card2 != NULL && save1 != NULL) {
            SDL_BlitSurface(card2, &dstrect, save1, &dstrect2);
         }

         dstrect2.x = dx;
         dstrect2.y = dy + 10;

         gpGeneral->UpdateScreen(dstrect2.x, dstrect2.y, dstrect2.w, dstrect2.h);
      } else if (slot == oldslot + 3 && !(oldslot & 1)) {
         SDL_Rect dstrect2;

         dstrect.x = 38;
         dstrect.y = 68;
         dstrect.w = 10;
         dstrect.h = 10;

         dstrect2.x = 0;
         dstrect2.y = 0;
         dstrect2.w = 10;
         dstrect2.h = 10;

         if (card2 != NULL && save1 != NULL) {
            SDL_BlitSurface(card2, &dstrect2, save1, &dstrect);
         }

         dstrect.x = dx;
         dstrect.y = dy;

         gpGeneral->UpdateScreen(dstrect.x, dstrect.y, dstrect.w, dstrect.h);
      }

      if (save2 != NULL) {
         SDL_DestroySurface(save2);
         save2 = NULL;
      }
      if (card2 != NULL) {
         SDL_DestroySurface(card2);
         card2 = NULL;
      }

      UTIL_Delay(200);

      if (GetGameMode() == GAMEMODE_KOREAN && leavethree) {
         current->m_iNumLeaveThree++;
         if (current->GetNumHandCard() >= 8) {
            // This happens in first round. Get 3 points from opponent
            gpGeneral->ClearPromptArea();
            CBox box(20, 260, 595, 50, 40, 55, 85);
            gpGeneral->PlaySound(SOUND_HINT);
            if (current->IsBot()) {
               gpGeneral->DrawTextInBox(msg("comget3pts"), 20, 260, 595, 50, 255, 255, 255, 18);
               m_iScore -= 3;
            } else {
               gpGeneral->DrawTextInBox(msg("youget3pts"), 20, 260, 595, 50, 255, 255, 255, 18);
               m_iScore += 3;
            }
            UTIL_Delay(3500);
            DrawScore();
         }
      }
   } else if (count == 1) {
      // Only one card matches the discarded one
      x2 = 140 + 48 * (index[0] / 2) + 10;
      y2 = 100 + 78 * (index[0] & 1) + 10;

      save2 = AnimCardMove(60, 105, x2, y2, 48, 78, save2, true);
      gpGeneral->PlaySound(SOUND_PICKCARD);
      UTIL_Delay(200);

      if (drawn.GetMonth() == getfour_month) {
         if (current->GetNumHandCard() > 1 && current->GetOpponent()->GetNumHandCard() > 0) {
            GetOneCardFromOpponent(current);
         }
      }

      CapturePair(current, drawn, index[0]);
   } else if (GetGameMode() == GAMEMODE_KOREAN && count >= 3) {
      // Three cards match the discarded one. Pick all these three cards
      x2 = 140 + 48 * (index[0] / 2) + 10;
      y2 = 100 + 78 * (index[0] & 1) + 10;

      save2 = AnimCardMove(60, 105, x2, y2, 48, 78, save2, true);
      gpGeneral->PlaySound(SOUND_PICKCARD);
      UTIL_Delay(200);

      if (save1 != NULL && save2 != NULL && (x1 < x2 || y1 < y2)) {
         SDL_Surface *savet = save1;
         save1 = save2;
         save2 = savet;
         i = x1;
         x1 = x2;
         x2 = i;
         i = y1;
         y1 = y2;
         y2 = i;
      }

      gpGeneral->PlaySound(SOUND_MOVECARD);
      AnimCardMove(x2, y2, 575, current->IsBot() ? 10 : 400, 48, 78, save2);
      UTIL_Delay(50);
      gpGeneral->PlaySound(SOUND_MOVECARD);
      AnimCardMove(x2 - 10, y2 - 10, 575, current->IsBot() ? 10 : 400);
      UTIL_Delay(50);
      save2 = NULL;

      if (save1 != NULL) {
         gpGeneral->PlaySound(SOUND_MOVECARD);
         AnimCardMove(x1, y1, 575, current->IsBot() ? 10 : 400, 48, 78, save1);
         UTIL_Delay(50);
         gpGeneral->PlaySound(SOUND_MOVECARD);
         AnimCardMove(x1 - 10, y1 - 10, 575, current->IsBot() ? 10 : 400);
         UTIL_Delay(50);
         save1 = NULL;
      }

      for (int k = 1; k < 3; k++) {
         x2 = 140 + 48 * (index[k] / 2);
         y2 = 100 + 78 * (index[k] & 1);

         gpGeneral->PlaySound(SOUND_MOVECARD);
         AnimCardMove(x2, y2, 575, current->IsBot() ? 10 : 400);
         UTIL_Delay(50);
      }

      CaptureTriple(current, drawn, index[0], index[1], index[2]);

      if (current->GetNumHandCard() > 1 && current->GetOpponent()->GetNumHandCard() > 0) {
         GetOneCardFromOpponent(current);
      }
   } else if (count == 2) {
      slot = ChooseSlotForPair(index[0], index[1], drawn, current);
      x2 = 140 + 48 * (slot / 2) + 10;
      y2 = 100 + 78 * (slot & 1) + 10;

      save2 = AnimCardMove(60, 105, x2, y2, 48, 78, save2, true);
      gpGeneral->PlaySound(SOUND_PICKCARD);
      UTIL_Delay(200);

      CapturePair(current, drawn, slot);
   }

   UTIL_Delay(200);

   if (save1 != NULL && save2 != NULL && (x1 < x2 || y1 < y2)) {
      SDL_Surface *savet = save1;
      save1 = save2;
      save2 = savet;
      i = x1;
      x1 = x2;
      x2 = i;
      i = y1;
      y1 = y2;
      y2 = i;
   }

   if (save2 != NULL) {
      gpGeneral->PlaySound(SOUND_MOVECARD);
      AnimCardMove(x2, y2, 575, current->IsBot() ? 10 : 400, 48, 78, save2);
      UTIL_Delay(50);
      gpGeneral->PlaySound(SOUND_MOVECARD);
      AnimCardMove(x2 - 10, y2 - 10, 575, current->IsBot() ? 10 : 400);
      UTIL_Delay(50);
   }

   if (save1 != NULL) {
      gpGeneral->PlaySound(SOUND_MOVECARD);
      AnimCardMove(x1, y1, 575, current->IsBot() ? 10 : 400, 48, 78, save1);
      UTIL_Delay(50);
      gpGeneral->PlaySound(SOUND_MOVECARD);
      AnimCardMove(x1 - 10, y1 - 10, 575, current->IsBot() ? 10 : 400);
      UTIL_Delay(50);
   }

   if (GetGameMode() == GAMEMODE_KOREAN) {
      for (i = 0; i < gpGame->GetNumDeskCard(); i++) {
         if (gpGame->GetDeskCard(i).IsValid()) {
            break;
         }
      }
      if (i >= gpGame->GetNumDeskCard()) {
         if (current->GetNumHandCard() > 1 && current->GetOpponent()->GetNumHandCard() > 0) {
            GetOneCardFromOpponent(current);
         }
      }
   }
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
   gpGeneral->PlaySound(SOUND_HINT2);
   current->GetOpponent()->DrawCaptured();
   int sy = (current->IsBot() ? 400 : 10), dy = (current->IsBot() ? 10 : 400);

   SDL_Surface *save = SDL_CreateSurface(48, 78, SDL_PIXELFORMAT_RGBA8888);

   SDL_Rect dstrect;
   dstrect.x = 575;
   dstrect.y = sy;
   dstrect.w = 48;
   dstrect.h = 78;

   gpGeneral->DrawCard(g, 575, sy, 48, 78, true);
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
         gpGeneral->DrawCard(m_DeskCards[i], 140 + (i / 2) * 48,
            100 + (i & 1) * 78, 48, 78, true);
      }
   }

   count = -1;

   while (1) {
      int k = gpGeneral->ReadKey();
      if (k >= 1000) {
         count = k - 1000;
         break;
      }
   }

   b[0].reset();
   b[1].reset();

   gpGeneral->UpdateScreen(dstrect.x, dstrect.y, dstrect.w, dstrect.h);
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
   gpGeneral->ClearScreen(false, false, false);

   // draw the card pile
   c = 255;
   for (i = 0; i < 5; i++) {
      gpGeneral->DrawCard(c, 50 + i * 2, 95 + i * 2, 48, 78, true);
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
   if (gpScreen == nullptr) return;

   SDL_Surface *save = SDL_CreateSurface(48, 78, gpScreen->format);
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

            card = gpGeneral->RenderCard(c, 48, 78);

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
               SDL_BlitSurface(gpScreen, &prev_dstrect3, save, NULL);
            }
            do {
               SDL_Rect dstrect3;
               dstrect3.w = card->w;
               dstrect3.h = card->h;

               float ratio = (now - first) / m_flAnimDuration;
               dstrect3.x = (int)(dstrect.x + (dstrect2.x - dstrect.x) * ratio);
               dstrect3.y = (int)(dstrect.y + (dstrect2.y - dstrect.y) * ratio);

               if (save != nullptr) {
                  SDL_BlitSurface(save, NULL, gpScreen, &prev_dstrect3);
                  SDL_BlitSurface(gpScreen, &dstrect3, save, NULL);
               }

               SDL_BlitSurface(card, NULL, gpScreen, &dstrect3);
               gpGeneral->UpdateScreen(prev_dstrect3.x, prev_dstrect3.y,
                  prev_dstrect3.w, prev_dstrect3.h);
               gpGeneral->UpdateScreen(dstrect3.x, dstrect3.y,
                  dstrect3.w, dstrect3.h);

               SDL_Delay(5);
               now = SDL_GetTicks();
               prev_dstrect3 = dstrect3;
            } while (now < first + (Uint64)m_flAnimDuration);

            if (save != nullptr) {
               SDL_BlitSurface(save, NULL, gpScreen, &prev_dstrect3);
            }
            SDL_BlitSurface(card, NULL, gpScreen, &dstrect2);
            gpGeneral->UpdateScreen(prev_dstrect3.x, prev_dstrect3.y,
               prev_dstrect3.w, prev_dstrect3.h);
            gpGeneral->UpdateScreen(dstrect2.x, dstrect2.y,
               dstrect2.w, dstrect2.h);
            if (card != nullptr) {
               SDL_DestroySurface(card);
            }
            gpGeneral->PlaySound(SOUND_DRAWCARD);
         }
         UTIL_Delay(200);
      }
   }

   if (save != nullptr) {
      SDL_DestroySurface(save);
   }

   gpGeneral->ClearPromptArea();
   CBox box(20, 260, 595, 50, 40, 55, 85);
   gpGeneral->PlaySound(SOUND_HINT);
   gpGeneral->DrawTextInBox((CBasePlayer::GetDealer() == m_pPlayers[1].get()) ?
      msg("comdealer") : msg("youdealer"), 20, 260, 595, 50, 255, 255, 255, 18);
   UTIL_Delay(3500);
}

SDL_Surface *CGame::AnimCardMove(int sx, int sy, int dx, int dy,
   int w, int h, SDL_Surface *save, bool retsave, bool retcard)
{
   if (gpScreen == nullptr) return nullptr;

   Uint64 first = SDL_GetTicks(), now = first;
   SDL_Rect dstrect, dstrect2;

   SDL_Surface *card = SDL_CreateSurface(w, h, gpScreen->format);

   if (save == NULL) {
      save = SDL_CreateSurface(w, h, gpScreen->format);
      SDL_Rect r = { sx, sy, w, h };
      SDL_BlitSurface(gpScreen, &r, save, NULL);
   }

   dstrect.x = sx;
   dstrect.y = sy;
   dstrect.w = dstrect2.w = w;
   dstrect.h = dstrect2.h = h;

   SDL_BlitSurface(gpScreen, &dstrect, card, NULL);

   if (sx == 60 && sy == 105) {
      gpGeneral->DrawCard(CCard(255), 60, 105, 48, 78, false);
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
         SDL_BlitSurface(save, NULL, gpScreen, &prev_dstrect3);
         SDL_BlitSurface(gpScreen, &dstrect3, save, NULL);
      }

      SDL_BlitSurface(card, NULL, gpScreen, &dstrect3);
      gpGeneral->UpdateScreen();

      SDL_Delay(5);
      now = SDL_GetTicks();
      prev_dstrect3 = dstrect3;
   } while (now < first + (Uint64)m_flAnimDuration);

   if (save != nullptr) {
      SDL_BlitSurface(save, NULL, gpScreen, &prev_dstrect3);
   }

   if (retsave && save != nullptr) {
      SDL_BlitSurface(gpScreen, &dstrect2, save, NULL);
   } else if (save != nullptr) {
      SDL_DestroySurface(save);
      save = NULL;
   }

   SDL_BlitSurface(card, NULL, gpScreen, &dstrect2);
   gpGeneral->UpdateScreen(prev_dstrect3.x, prev_dstrect3.y,
      prev_dstrect3.w, prev_dstrect3.h);
   gpGeneral->UpdateScreen(dstrect2.x, dstrect2.y,
      dstrect2.w, dstrect2.h);

   if (retcard) {
      return card;
   }

   SDL_DestroySurface(card);
   return save;
}

void CGame::DrawScore()
{
   if (gpScreen != nullptr) {
      UTIL_FillRect(gpScreen, 10, 190, 120, 55, 30, 130, 100);
   }
   CBox s(10, 190, 120, 55, 0, 175, 0, 160, true);
   gpGeneral->DrawTextBrush(va("SCORE  %d", m_iScore), 15, 195, 255, 255, 0, 16);
   gpGeneral->DrawTextBrush(va("RD %d/12", m_iCurrentRound > 12 ? 12 : m_iCurrentRound), 15, 220, 0, 255, 255, 16);
   gpGeneral->UpdateScreen(10, 190, 120, 55);
}

void CGame::ShowMatchResults()
{
   gpGeneral->ClearScreen();

   CBox mainbox(40, 120, 560, 240, 40, 55, 85);
   CBox titlebox(40, 120, 560, 50, 0, 128, 128);

   std::string header;
   int r, g, b;

   if (m_iScore > 0) {
      header = msg("match_victory");
      r = 0; g = 255; b = 0;
      gpGeneral->PlaySound(SOUND_WIN);
   } else if (m_iScore < 0) {
      header = msg("match_defeat");
      r = 255; g = 50; b = 50;
      gpGeneral->PlaySound(SOUND_LOSE);
   } else {
      header = msg("match_draw");
      r = 255; g = 255; b = 0;
   }

   gpGeneral->DrawText(header.c_str(), 180, 128, r, g, b, 36);
   gpGeneral->DrawText(va(msg("final_score_fmt"), m_iScore), 140, 190, 255, 255, 255, 26);
   
   if (m_iScore > 0) {
      gpGeneral->DrawText(msg("congrats_win"), 60, 250, 255, 255, 0, 22);
   } else if (m_iScore < 0) {
      gpGeneral->DrawText(msg("better_luck_lose"), 60, 250, 255, 150, 150, 22);
   } else {
      gpGeneral->DrawText(msg("tie_game"), 180, 250, 255, 255, 0, 22);
   }

   CButton okbtn(1, 240, 300, 160, 40, 58, 110, 165);
   gpGeneral->DrawText("OK", 305, 308, 255, 255, 255, 24);
   gpGeneral->UpdateScreen();

   while (1) {
      int k = gpGeneral->ReadKey();
      if (k > 0) {
         break;
      }
   }
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
         gpGeneral->DrawCard(m_DeskCards[i], dstrect.x, dstrect.y,
            48, 78, false);
      } else {
         if (gpScreen != nullptr) {
            UTIL_FillRect(gpScreen, dstrect.x, dstrect.y, dstrect.w, dstrect.h, 30, 130, 100);
         }
      }
   }

   if (gpScreen != nullptr) {
      UTIL_FillRect(gpScreen, 140, 100 + 78 * 2, 48 * 10, 10, 30, 130, 100);
   }

   gpGeneral->UpdateScreen(140, 100, 48 * 10, 78 * 2 + 10);
}

bool CGame::DoubleUp(CBasePlayer *player)
{
   auto mainbox = std::make_unique<CBox>(20, 250, 595, 100, 40, 55, 85);
   auto yesbtn = std::make_unique<CButton>(1, 40, 300, 140, 38, 58, 110, 165);
   auto nobtn = std::make_unique<CButton>(2, 200, 300, 140, 38, 165, 58, 58);
   bool ret = true;

   gpGeneral->DrawTextInBox(msg("doubleupyesorno"), 20, 255, 595, 35, 255, 255, 0, 20);
   gpGeneral->DrawTextInBox(msg("yes"), 40, 300, 140, 38, 255, 255, 255, 20);
   gpGeneral->DrawTextInBox(msg("no"), 200, 300, 140, 38, 255, 255, 255, 20);

   while (1) {
      int k = gpGeneral->ReadKey();
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

   gpGeneral->ClearScreen();

   // draw the card pile
   CCard c(255);
   int i;
   for (i = 0; i < 5; i++) {
      gpGeneral->DrawCard(c, 50 + i * 2, 95 + i * 2, 48, 78, true);
   }

   c = RandomLong(0, 47);

   mainbox = std::make_unique<CBox>(20, 250, 595, 100, 40, 55, 85);
   yesbtn = std::make_unique<CButton>(1, 40, 300, 140, 38, 58, 110, 165);
   nobtn = std::make_unique<CButton>(2, 200, 300, 140, 38, 165, 58, 58);

   gpGeneral->DrawTextInBox(msg("bigorsmall"), 20, 255, 595, 35, 255, 255, 0, 20);
   gpGeneral->DrawTextInBox(msg("big"), 40, 300, 140, 38, 255, 255, 255, 20);
   gpGeneral->DrawTextInBox(msg("small"), 200, 300, 140, 38, 255, 255, 255, 20);

   CBox s(25, 190, 110, 70, 0, 175, 0, 160);
   gpGeneral->DrawTextBrush("WIN", 30, 190, 255, 255, 0, 32);
   gpGeneral->DrawTextBrush(va("%6d", player->m_Result.score), 30, 220);
   gpGeneral->UpdateScreen(25, 190, 110, 70);

   bool isbig = true;
   while (1) {
      int k = gpGeneral->ReadKey();
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

   gpGeneral->DrawCard(c, 60, 105, 48, 78);
   gpGeneral->PlaySound(SOUND_DRAWCARD);
   AnimCardMove(60, 105, 150, 115, 48, 78, save);
   SDL_DestroySurface(save);
   UTIL_Delay(800);

   if ((isbig && c.GetMonth() >= 7) || (!isbig && c.GetMonth() < 7)) {
      ret = true;
      player->m_Result.score *= 2;
      gpGeneral->DrawTextBrush("WIN", 210, 125, 255, 255, 128, 64);
      gpGeneral->PlaySound(SOUND_WIN);
   } else {
      ret = false;
      player->m_Result.score = 0;
      gpGeneral->DrawTextBrush("LOSE", 210, 125, 128, 255, 255, 64);
      gpGeneral->PlaySound(SOUND_LOSE);
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

