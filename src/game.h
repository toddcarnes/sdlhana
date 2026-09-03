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

#ifndef GAME_H
#define GAME_H

enum
{
   GAMEMODE_KOIKOI = 0,
   GAMEMODE_BET,
   GAMEMODE_KOREAN,
};

class CGame
{
public:
   CGame();
   ~CGame();

   void            MainMenu();
   void            Settings();

   void            RunGame();

   void            InitGame();
   void            RulesMenu();
   void            NewRound();

   void            PlayRound();

   int             SelectCardOnDesk(int month);

   inline int      GetGameMode() const  { return m_iGameMode; }

   inline int      GetNumDeskCard()   const  { return m_iNumDeskCard; }
   inline CCard    GetDeskCard(int i) const  { assert(i >= 0 && i < m_iNumDeskCard); return m_DeskCards[i]; }

   void            RedrawTable();

private:
   int             m_iGameMode;
   int             m_iScore;
   int             m_iCurrentRound;

   float           m_flAnimDuration;

   std::unique_ptr<CBasePlayer> m_pPlayers[2];

   CCard           m_DeskCards[24];
   int             m_iNumDeskCard;

   void            InitScreen();
   void            DetermineFirstDealer();
   void            AnimDeal();
   SDL_Surface    *AnimCardMove(int sx, int sy, int dx, int dy, int w = 48, int h = 78, SDL_Surface *save = NULL, bool retsave = false, bool retcard = false);
   void            DrawScore();
   void            DrawRoundInfo();
   void            ShowMatchResults();
   void            DrawDeskCard();
   void            CardDiscarded(const CCard &s, CBasePlayer *current, int sx, int sy);
   void            GetOneCardFromOpponent(CBasePlayer *current);
   bool            DoubleUp(CBasePlayer *player);

   int             FindFreeDeskCardSlot(int exclude = -1);
   void            RemoveDeskCard(int index);

   // Refactored helpers for CardDiscarded and desk validation (review #2)
   int             FindMatchingCards(const CCard &card, int indices[3]) const;
   int             ChooseSlotForPair(int idx0, int idx1, const CCard &played, CBasePlayer *player) const;
   void            CapturePair(CBasePlayer *player, const CCard &played, int deskIdx);
   void            CaptureTriple(CBasePlayer *player, const CCard &played, int idx0, int idx1, int idx2);
   void            ValidateInitialDesk();

   // Phase helpers to reduce CardDiscarded below 150 lines (review #1 follow-up)
   struct PhaseState {
      SDL_Surface* save1 = nullptr;
      SDL_Surface* save2 = nullptr;
      int x1 = 0, y1 = 0, x2 = 0, y2 = 0;
      int slot = -1;
      int getfourMonth = -1;
      bool leavethree = false;
   };
   void HandleDiscardPhase(const CCard& c, const CCard& drawn, CBasePlayer* cur, int sx, int sy, PhaseState& st);
   void HandleDrawnPhase(const CCard& drawn, CBasePlayer* cur, PhaseState& st);
   void FixupOverlappedSlot(SDL_Surface* save1, SDL_Surface* card2, int oldSlot, int newSlot, int dx, int dy);
   void AnimatePendingCaptures(PhaseState& st, CBasePlayer* cur);
};

#include <memory>

#endif

