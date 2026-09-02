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
#include <SDL3_mixer/SDL_mixer.h>
#include <atomic>

static MIX_Mixer *g_pMixer = nullptr;
std::atomic<bool> g_fAudioOpened{false};

void SOUND_FillAudio(void *udata, unsigned char *stream, int len)
{
   (void)udata;
   (void)stream;
   (void)len;
}

int SOUND_OpenAudio(int freq, int format, int channels, int samples)
{
   (void)freq;
   (void)format;
   (void)channels;
   (void)samples;

   if (g_fAudioOpened) {
      return 0;
   }

   if (!MIX_Init()) {
      std::println(stderr, "WARNING: Couldn't init SDL3_mixer: {}", SDL_GetError());
      return -1;
   }

   SDL_AudioSpec spec;
   SDL_zero(spec);
   spec.freq = (freq > 0) ? freq : 44100;
   spec.format = (format != 0) ? (SDL_AudioFormat)format : SDL_AUDIO_S16;
   spec.channels = (channels > 0) ? channels : 2;

   g_pMixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec);
   if (!g_pMixer) {
      g_pMixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, NULL);
   }

   if (!g_pMixer) {
      std::println(stderr, "WARNING: Couldn't create SDL3_mixer device: {}", SDL_GetError());
      MIX_Quit();
      return -1;
   }

   g_fAudioOpened = true;
   return 0;
}

SoundSample *SOUND_LoadWAV(const char *filename)
{
   if (!g_fAudioOpened || !g_pMixer) {
      return NULL;
   }

   MIX_Audio *audio = MIX_LoadAudio(g_pMixer, filename, true);
   if (audio == NULL) {
      std::println(stderr, "WARNING: Cannot load WAV file {}: {}", filename, SDL_GetError());
      return NULL;
   }

   SoundSample *sample = new SoundSample();
   sample->audio = audio;
   return sample;
}

void SOUND_FreeWAV(SoundSample *audio)
{
   if (audio == NULL) {
      return;
   }
   if (audio->audio != NULL) {
      MIX_DestroyAudio(audio->audio);
   }
   delete audio;
}

void SOUND_PlayWAV(SoundSample *audio)
{
   if (audio == nullptr || audio->audio == nullptr || !g_fAudioOpened || !g_pMixer) {
      return;
   }
   if (!MIX_PlayAudio(g_pMixer, audio->audio)) {
      std::println(stderr, "WARNING: MIX_PlayAudio failed: {}", SDL_GetError());
   }
}
