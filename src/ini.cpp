//
// Copyright (c) 2005, 2006 Wei Mingzhi <whistler@openoffice.org>
// Copyright (c) 2026 Todd Carnes <toddcarnes@gmail.com>
// All Rights Reserved.
//

#include "ini.h"
#include <string>
#include <fstream>
#include <algorithm>
#include <cctype>
#include <print>

static inline std::string trim_copy(std::string s)
{
   auto notSpace = [](unsigned char c) { return !std::isspace(c); };
   s.erase(s.begin(), std::find_if(s.begin(), s.end(), notSpace));
   s.erase(std::find_if(s.rbegin(), s.rend(), notSpace).base(), s.end());
   return s;
}

void CIniFile::trim(char *str)
{
   int pos = 0;
   char *dest = str;

   // skip leading blanks
   while (str[pos] <= ' ' && str[pos] > 0)
      pos++;

   while (str[pos]) {
      *(dest++) = str[pos];
      pos++;
   }

   *(dest--) = '\0'; // store the null

   // remove trailing blanks
   while (dest >= str && *dest <= ' ' && *dest > 0)
      *(dest--) = '\0';
}

CIniFile::CIniFile():
ini(NULL), key_count(0), current_size(0)
{
}

CIniFile::CIniFile(const char *filename):
ini(NULL), key_count(0), current_size(0)
{
   Load(filename);
}

CIniFile::~CIniFile()
{
   FreeAllTheStuff();
}

void CIniFile::FreeAllTheStuff()
{
   int i, j;

   if (ini) {
      // free all the memory allocated for this ini file
      for (i = 0; i < key_count; i++) {
         // delete all the values in this key...
         for (j = 0; j < ini[i].value_count; j++) {
            free(ini[i].values[j].value);
            free(ini[i].values[j].value_name);
         }

         free(ini[i].values);
         free(ini[i].key_name);
      }
      free(ini);
      ini = NULL;
   }
}

int CIniFile::Load(const char *filename)
{
   std::ifstream file(filename);
   if (!file) {
      std::println(stderr, "cannot load ini file {}", filename);
      return 1;
   }

   if (Valid()) {
      FreeAllTheStuff();
   }

   std::string line, section;
   while (std::getline(file, line)) {
      line = trim_copy(line);
      if (line.empty() || line[0] == ';' || line[0] == '/' || line[0] == '#')
         continue;
      if (line.front() == '[' && line.back() == ']') {
         section = trim_copy(line.substr(1, line.size() - 2));
      } else {
         auto pos = line.find('=');
         if (pos != std::string::npos) {
            std::string key = trim_copy(line.substr(0, pos));
            std::string val = trim_copy(line.substr(pos + 1));
            Set(section.c_str(), key.c_str(), val.c_str());
         }
      }
   }
   return 0;
}

int CIniFile::Save(const char *filename)
{
   std::ofstream file(filename);
   if (!file) {
      std::println(stderr, "cannot save to INI file: {}", filename);
      return 1;
   }
   for (int i = 0; i < key_count; i++) {
      file << '[' << ini[i].key_name << "]\n";
      for (int j = 0; j < ini[i].value_count; j++) {
         file << ini[i].values[j].value_name << '=' << ini[i].values[j].value << '\n';
      }
      file << '\n';
   }
   return 0;
}

// set the value in ini file
void CIniFile::Set(const char *key, const char *value, const char *set)
{
   ini_key_t *pKey = NULL;
   ini_value_t *pValue = NULL;
   int i;

   for (i = 0; i < key_count; i++) {
      if (strcasecmp(ini[i].key_name, key) == 0) {
         pKey = &ini[i];
         break;
      }
   }

   // if this is a new key, try to allocate memory for it
   if (pKey == NULL) {
      key_count++;

      // if we don't have enough room for this new key, try to allocate more memory
      if (key_count > current_size) {
         current_size += INI_SIZE_INCREMENT;
         if (ini) {
            ini = (ini_key_t *)realloc(ini, sizeof(ini_key_t) * current_size);
         } else {
            ini = (ini_key_t *)malloc(sizeof(ini_key_t) * current_size);
         }

         if (!ini) {
            std::println(stderr, "Memory allocation error!");
            exit(1);
         }
      }

      pKey = &ini[key_count - 1];
      pKey->key_name = strdup(key);
      pKey->values = NULL;
      pKey->value_count = 0;
      pKey->current_size = 0;
   }

   for (i = 0; i < pKey->value_count; i++) {
      if (strcasecmp(value, pKey->values[i].value_name) == 0) {
         pValue = &pKey->values[i];
         break;
      }
   }

   if (pValue != NULL) {
      // this value already exists in the key...
      free(pValue->value);
      pValue->value = strdup(set);

      if (pValue->value == NULL) {
         std::println(stderr, "Memory allocation error!");
         exit(1);
      }
   } else {
      // this is a new value...
      pKey->value_count++;

      // if we don't have enough room for this new value, try to allocate more memory
      if (pKey->value_count > pKey->current_size) {
         pKey->current_size += INI_SIZE_INCREMENT;
         if (pKey->values) {
            pKey->values = (ini_value_t *)realloc(pKey->values, sizeof(ini_value_t) * pKey->current_size);
         } else {
            pKey->values = (ini_value_t *)malloc(sizeof(ini_value_t) * pKey->current_size);
         }

         if (pKey->values == NULL) {
            std::println(stderr, "Memory allocation error!");
            exit(1);
         }
      }

      pKey->values[pKey->value_count - 1].value_name = strdup(value);
      pKey->values[pKey->value_count - 1].value = strdup(set);

      if (pKey->values[pKey->value_count - 1].value == NULL ||
         pKey->values[pKey->value_count - 1].value_name == NULL) {
         std::println(stderr, "Memory allocation error!");
         exit(1);
      }

      trim(pKey->values[pKey->value_count - 1].value);
      trim(pKey->values[pKey->value_count - 1].value_name);
   }
}

// get a value from the ini file
const char *CIniFile::Get(const char *key, const char *value, const char *def)
{
   ini_key_t *pKey = NULL;
   int i;

   for (i = 0; i < key_count; i++) {
      if (strcasecmp(ini[i].key_name, key) == 0) {
         pKey = &ini[i];
         break;
      }
   }

   if (pKey != NULL) {
      for (i = 0; i < pKey->value_count; i++) {
         if (strcasecmp(pKey->values[i].value_name, value) == 0) {
            return pKey->values[i].value;
         }
      }
   }

   return def; // value is not found; use default value
}
