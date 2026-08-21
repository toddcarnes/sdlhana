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
#include <cstring>

static unsigned int audio_len = 0;
static unsigned char *audio_pos = NULL;
static SDL_AudioSpec audio_spec;
static SDL_AudioStream *g_audioStream = nullptr;
bool g_fAudioOpened = false;

void SOUND_FillAudio(void *udata, unsigned char *stream, int len)
{
   (void)udata;
   (void)stream;
   (void)len;
}

int SOUND_OpenAudio(int freq, int format, int channels, int samples)
{
   (void)samples;
   if (g_fAudioOpened) {
      return 0;
   }

   audio_spec.freq = freq;
   audio_spec.format = (SDL_AudioFormat)format;
   audio_spec.channels = channels;

   g_audioStream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &audio_spec, NULL, NULL);
   if (!g_audioStream) {
      std::println(stderr, "WARNING: Couldn't open audio stream: {}", SDL_GetError());
      return -1;
   }

   SDL_ResumeAudioStreamDevice(g_audioStream);
   g_fAudioOpened = true;
   return 0;
}

SoundSample *SOUND_LoadWAV(const char *filename)
{
   if (!g_fAudioOpened) {
      return NULL;
   }

   SDL_AudioSpec spec;
   Uint8 *buf = NULL;
   Uint32 len = 0;

   if (!SDL_LoadWAV(filename, &spec, &buf, &len)) {
      return NULL;
   }

   SoundSample *sample = new SoundSample();
   sample->buf = buf;
   sample->len = len;
   sample->spec = spec;
   return sample;
}

void SOUND_FreeWAV(SoundSample *audio)
{
   if (audio == NULL) {
      return;
   }
   if (audio->buf != NULL) {
      SDL_free(audio->buf);
   }
   delete audio;
}

void SOUND_PlayWAV(SoundSample *audio)
{
   if (audio == nullptr || g_audioStream == nullptr) {
      return;
   }
   SDL_SetAudioStreamFormat(g_audioStream, &audio->spec, &audio_spec);
   SDL_PutAudioStreamData(g_audioStream, audio->buf, audio->len);
}
