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
// portions of the code.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
// 02110-1301, USA
//

#include "main.h"
#include <unordered_map>
#include <fstream>
#include <string>
#include <string_view>

static std::unordered_map<std::string, std::string> gMessageTable;

void FreeTextMessage()
{
   gMessageTable.clear();
}

void InitTextMessage()
{
   FreeTextMessage();

   std::string filename = std::format("{}titles{}.txt", DATA_DIR, cfg.Get("OPTIONS", "Language", "eng"));
   std::ifstream file(filename);

   if (!file.is_open()) {
      std::println(stderr, "WARNING: cannot load titles{}.txt!", cfg.Get("OPTIONS", "Language", ""));
      return;
   }

   std::string line;
   std::string current_name;
   std::string current_msg;
   enum { NAME, TEXT } state = NAME;
   int linenumber = 0;

   while (std::getline(file, line)) {
      linenumber++;
      std::string raw_line = line + "\n";
      
      // Trim line for state parsing
      std::string trimmed = line;
      trim(trimmed.data());
      // adjust trimmed length
      size_t first = trimmed.find_first_not_of(" \t\r\n");
      if (first == std::string::npos) {
         trimmed.clear();
      } else {
         size_t last = trimmed.find_last_not_of(" \t\r\n");
         trimmed = trimmed.substr(first, (last - first + 1));
      }

      if (trimmed.empty() || trimmed[0] == '#' || trimmed[0] == '/') {
         continue; // skip empty or comment lines
      }

      switch (state) {
         case NAME:
            if (trimmed == "}") {
               TerminateOnError("Unexpected \"}\" found in titles.txt, line %d", linenumber);
            } else if (trimmed == "{") {
               state = TEXT;
               current_msg.clear();
            } else {
               current_name = trimmed;
            }
            break;

         case TEXT:
            if (trimmed == "{") {
               TerminateOnError("Unexpected \"{\" found in titles.txt, line %d", linenumber);
            } else if (trimmed == "}") {
               // Remove trailing newlines/carriage returns
               while (!current_msg.empty() && (current_msg.back() == '\n' || current_msg.back() == '\r')) {
                  current_msg.pop_back();
               }
               gMessageTable[current_name] = current_msg;
               state = NAME;
            } else {
               current_msg += raw_line;
            }
            break;
      }
   }
}

const char *msg(const char *name)
{
   if (name == nullptr) {
      return "";
   }
   auto it = gMessageTable.find(name);
   if (it != gMessageTable.end()) {
      return it->second.c_str();
   }
   return name;
}
