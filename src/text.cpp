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
   if (langs.empty()) langs.push_back("eng");
   std::sort(langs.begin(), langs.end());
   langs.erase(std::unique(langs.begin(), langs.end()), langs.end());
   return langs;
}

static std::string printfToFormat(std::string s)
{
   std::string out;
   out.reserve(s.size());
   for (size_t i = 0; i < s.size(); ) {
      if (s[i] == '%' && i + 1 < s.size()) {
         if (s[i+1] == '%') { out += '%'; i += 2; continue; }
         size_t j = i + 1;
         while (j < s.size() && std::isdigit((unsigned char)s[j])) j++;
         if (j < s.size() && (s[j] == 'd' || s[j] == 's')) {
            out += "{}";
            i = j + 1;
            continue;
         }
      }
      out += s[i++];
   }
   return out;
}

void FreeTextMessage()
{
   gMessageTable.clear();
}

void InitTextMessage()
{
   FreeTextMessage();
   std::string lang = Config().Get("OPTIONS", "Language", "eng");

   std::string jsonFile = std::format("{}i18n/{}.json", DATA_DIR, lang);
   std::ifstream jf(jsonFile);
   if (jf.is_open()) {
      try {
         nlohmann::json j;
         jf >> j;
         for (auto &[k, v] : j.items()) {
            if (v.is_string()) gMessageTable[k] = printfToFormat(v.get<std::string>());
         }
         g_availableLangs = DiscoverLanguages();
         return;
      } catch (const std::exception &e) {
         std::println(stderr, "WARNING: JSON parse failed for {}: {}", jsonFile, e.what());
      }
   }

   std::println(stderr, "WARNING: cannot load {}.json!", lang);
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
