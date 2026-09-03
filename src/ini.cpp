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
#ifdef WITH_HASH
   m_Hash = (ini_key_t **)calloc(INI_HASH_KEY_SIZE, sizeof(ini_key_t *));
   if (m_Hash == NULL) {
      std::println(stderr, "Memory allocation error!");
      exit(1);
   }
#endif
}

CIniFile::CIniFile(const char *filename):
ini(NULL), key_count(0), current_size(0)
{
#ifdef WITH_HASH
   m_Hash = (ini_key_t **)calloc(INI_HASH_KEY_SIZE, sizeof(ini_key_t *));
   if (m_Hash == NULL) {
      std::println(stderr, "Memory allocation error!");
      exit(1);
   }
#endif
   Load(filename);
}

CIniFile::~CIniFile()
{
   FreeAllTheStuff();
#ifdef WITH_HASH
   free(m_Hash);
#endif
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

#ifdef WITH_HASH
   // null out the hash table
   memset(m_Hash, 0, sizeof(ini_key_t *) * INI_HASH_KEY_SIZE);
#endif
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

#ifdef WITH_HASH
   // search if this key already exists...
   // first search in the hash table...
   unsigned short vhash = GetHashValue(key) % INI_HASH_KEY_SIZE;
   if (m_Hash[vhash] != NULL && strcasecmp(key, m_Hash[vhash]->key_name) == 0) {
      pKey = m_Hash[vhash]; // found this value in the hash table
   }
#endif

   if (pKey == NULL) {
      // not found in the hash table, do a normal search...
      for (i = 0; i < key_count; i++) {
         if (strcasecmp(ini[i].key_name, key) == 0) {
            pKey = &ini[i];
            break;
         }
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
#ifdef WITH_HASH
      memset(pKey->hash, 0, sizeof(pKey->hash)); // zero out the hash table
      // store this new key in the hash table...
      unsigned short vhash = GetHashValue(pKey->key_name) % INI_HASH_KEY_SIZE;
      m_Hash[vhash] = pKey;
#endif
   }

#ifdef WITH_HASH
   // search if the value is already in the key...
   vhash = GetHashValue(value) % INI_HASH_VALUE_SIZE;
   if (pKey->hash[vhash] != NULL && strcasecmp(value, pKey->hash[vhash]->value_name) == 0) {
      pValue = pKey->hash[vhash]; // we have found the value in the hash table
   }
#endif

   if (pValue == NULL) {
      // value is not found in the hash table, do a normal search...
      for (i = 0; i < pKey->value_count; i++) {
         if (strcasecmp(value, pKey->values[i].value_name) == 0) {
            // we have found the value
            pValue = &pKey->values[i];
            break;
         }
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

#ifdef WITH_HASH
      // store this new value in the hash table
      unsigned short vhash = GetHashValue(pKey->values[pKey->value_count - 1].value_name) % INI_HASH_VALUE_SIZE;
      pKey->hash[vhash] = &pKey->values[pKey->value_count - 1];
#endif
   }
}

// get a value from the ini file
const char *CIniFile::Get(const char *key, const char *value, const char *def)
{
   ini_key_t *pKey = NULL;
   int i;

#ifdef WITH_HASH
   // search for the key name...
   // first search in the hash table...
   unsigned short vhash = GetHashValue(key) % INI_HASH_KEY_SIZE;
   if (m_Hash[vhash] != NULL && strcasecmp(key, m_Hash[vhash]->key_name) == 0) {
      pKey = m_Hash[vhash]; // found this value in the hash table
   }
#endif

   if (pKey == NULL) {
      // Not found in the hash table, do a normal search...
      for (i = 0; i < key_count; i++) {
         if (strcasecmp(ini[i].key_name, key) == 0) {
            pKey = &ini[i];
            break;
         }
      }
   }

   if (pKey != NULL) {
#ifdef WITH_HASH
      // key found, search for the value in this key...
      vhash = GetHashValue(value) % INI_HASH_VALUE_SIZE;
      if (pKey->hash[vhash] != NULL && strcasecmp(value, pKey->hash[vhash]->value_name) == 0) {
         return pKey->hash[vhash]->value;
      }
#endif
      // not found in the hash table, do a normal linear search...
      for (i = 0; i < pKey->value_count; i++) {
         if (strcasecmp(pKey->values[i].value_name, value) == 0) {
            return pKey->values[i].value;
         }
      }
   }

   return def; // value is not found; use default value
}

#ifdef WITH_HASH
/**
 * This hash function has been taken from an Article in Dr Dobbs Journal.
 * This is normally a collision-free function, distributing keys evenly.
 * Collision can be avoided by comparing the key itself in last resort.
 */
unsigned short CIniFile::GetHashValue(const char *sz)
{
   unsigned short hash = 0;

   while (*sz) {
      // convert all the characters to be upper case, so
      // that it will be case insensitive
      char a = *sz;
      if (a >= 'a' && a <= 'z') {
         a -= 'a' - 'A';
      }

      hash += (unsigned short)a;
      hash += (hash << 10);
      hash ^= (hash >> 6);

      sz++;
   }

   hash += (hash << 3);
   hash ^= (hash >> 11);
   hash += (hash << 15);

   return hash;
}
#endif

