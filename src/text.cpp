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
#include <filesystem>
#include <nlohmann/json.hpp>

static std::unordered_map<std::string, std::string> gMessageTable;
static std::vector<std::string> g_availableLangs;

std::vector<std::string> DiscoverLanguages()
{
   std::vector<std::string> langs;
   std::string i18nDir = std::string(DATA_DIR) + "i18n";
   if (std::filesystem::exists(i18nDir)) {
      for (auto &entry : std::filesystem::directory_iterator(i18nDir)) {
         if (entry.is_regular_file() && entry.path().extension() == ".json") {
            langs.push_back(entry.path().stem().string());
         }
      }
   }
   // Fallback: scan legacy titles*.txt in DATA_DIR
   if (std::filesystem::exists(DATA_DIR)) {
      for (auto &entry : std::filesystem::directory_iterator(DATA_DIR)) {
         std::string name = entry.path().filename().string();
         if (name.rfind("titles", 0) == 0 && name.size() > 10 && name.substr(name.size()-4) == ".txt") {
            std::string code = name.substr(6, name.size()-10);
            if (std::find(langs.begin(), langs.end(), code) == langs.end()) langs.push_back(code);
         }
      }
   }
   if (langs.empty()) langs.push_back("eng");
   std::sort(langs.begin(), langs.end());
   langs.erase(std::unique(langs.begin(), langs.end()), langs.end());
   return langs;
}

void FreeTextMessage()
{
   gMessageTable.clear();
}

void InitTextMessage()
{
   FreeTextMessage();
   std::string lang = cfg.Get("OPTIONS", "Language", "eng");

   // Try JSON first: data/i18n/<lang>.json
   std::string jsonFile = std::format("{}i18n/{}.json", DATA_DIR, lang);
   std::ifstream jf(jsonFile);
   if (jf.is_open()) {
      try {
         nlohmann::json j;
         jf >> j;
         for (auto &[k, v] : j.items()) {
            if (v.is_string()) gMessageTable[k] = v.get<std::string>();
         }
         // Populate available langs for autodiscovery (cached)
         g_availableLangs = DiscoverLanguages();
         return;
      } catch (const std::exception &e) {
         std::println(stderr, "WARNING: JSON parse failed for {}: {}", jsonFile, e.what());
      }
   }

   // Fallback to legacy titles*.txt
   std::string filename = std::format("{}titles{}.txt", DATA_DIR, lang);
   std::ifstream file(filename);
   if (!file.is_open()) {
      std::println(stderr, "WARNING: cannot load titles{}.txt nor {}.json!", lang, lang);
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
      std::string trimmed = line;
      trim(trimmed.data());
      size_t first = trimmed.find_first_not_of(" \t\r\n");
      if (first == std::string::npos) {
         trimmed.clear();
      } else {
         size_t last = trimmed.find_last_not_of(" \t\r\n");
         trimmed = trimmed.substr(first, (last - first + 1));
      }
      if (trimmed.empty() || trimmed[0] == '#' || trimmed[0] == '/') {
         continue;
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
   g_availableLangs = DiscoverLanguages();
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
