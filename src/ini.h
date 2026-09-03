//
// Copyright (c) 2005, 2006 Wei Mingzhi <whistler@openoffice.org>
// Copyright (c) 2026 Todd Carnes <toddcarnes@gmail.com>
// All Rights Reserved.
//

#ifndef INI_H
#define INI_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if defined(__has_include) && __has_include(<malloc.h>) && !defined(__APPLE__)
#include <malloc.h>
#endif
#include <ctype.h>

#if defined(_MSC_VER)
#pragma warning (disable : 4244)
#pragma warning (disable : 4514)
#pragma warning (disable : 4706)
#endif

#if defined(_WIN32) && !defined(__BORLANDC__)
#ifndef strcasecmp
#define strcasecmp strcmpi
#endif
#endif

static const int INI_SIZE_INCREMENT = 10; // increment of memory allocation

typedef struct {
   char           *value_name;
   char           *value;
} ini_value_t;

typedef struct {
   char               *key_name;
   int                 value_count;
   int                 current_size;
   ini_value_t        *values;
} ini_key_t;

class CIniFile
{
public:
   CIniFile();
   CIniFile(const char *filename);
   virtual ~CIniFile();

   int Load(const char *filename);
   int Save(const char *filename);

   const char *Get(const char *key, const char *value, const char *def = "");
   void Set(const char *key, const char *value, const char *set);

   inline bool Valid(void) { return (ini != NULL); }

private:
   ini_key_t      *ini;
   int             key_count;
   int             current_size;

   void            FreeAllTheStuff(void);

   // utility functions
   void            trim(char *str);
};

#endif
