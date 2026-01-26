//      (_||_/
//      (    )
//     ( o  0 )
//-OOO°--(_)---°OOO---------------------------------------
//                   Copyright (C) 2006 By Dominique Roux-Serret
// .OOOo      oOOO.  roux-serret@ifrance.com
//-(   )------(   )---------------------------------------
//  ( (        ) /   Copyright (C) 2023 By Johnny Jazeix
//   (_)      (_/    jazeix@gmail.com

//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 or version 3 of the License.

//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.

//    You should have received a copy of the GNU General Public License along
//    with this program; if not, write to the Free Software Foundation, Inc.,
//    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

#include <SDL3/SDL_audio.h> // for AUDIO_S16
#include <SDL3/SDL_error.h> // for SDL_GetError
#include <SDL3/SDL_log.h> // for SDL_LogInfo, SDL_LOG_CATEGORY_APPLICATION
#include <cstring>
#include <cstdio>

#include <map>
#include <string>

#include "audio.h"
#include "utils.h"
#include "preference.h"

extern sNewPreference Pref;
extern int currentTime;

Audio::~Audio()
{
    if (N) {
        for (int i = 0; i < N; i++) {
            if (Sound[i]) {
                MIX_DestroyAudio(Sound[i]);
            }
        }
        delete[] Sound;
    }
    // MIX_DestroyMixer(Mixer); // Need to find why this call crashes
}

bool Audio::Init()
{
    char PathFile[512];
    Mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, NULL);
    if (!Mixer) {
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Unable to init Sound card: %s", SDL_GetError());
        return false;
    }
    Track = MIX_CreateTrack(Mixer);
    MusicTrack = MIX_CreateTrack(Mixer);
    N = sSize;
    Sound = new MIX_Audio *[sSize];

    std::map<eSon, std::string> sounds {
        { sClic, "Sounds/click.wav" },
        { sSpeed, "Sounds/speed.wav" },
        { sCrash, "Sounds/crash.wav" },
        { sEnd, "Sounds/end.wav" },
        { sLose, "Sounds/lose.wav" },
        { sEtire, "Sounds/etire.wav" },
        { sWagon, "Sounds/wagon.wav" },
        { sReduit, "Sounds/reduit.wav" },
        { sLive, "Sounds/live.wav" }
    };
    for (const auto &sound: sounds) {
        strcpy(PathFile, sound.second.c_str());
        Utils::GetPath(PathFile);
        Sound[sound.first] = MIX_LoadAudio(Mixer, PathFile, true);
    }

    return true;
}

/*** Loads a music track, 0 = menu music 1,2,3,4=game music tracks ***/
/*********************************************************************/
void Audio::LoadMusic(int Num)
{
    char Provi[512];

    if (!N) {
        return;
    }

    NMus = Num;

    if (Music) {
        PauseMusic(true);
        MIX_StopTrack(Track, 0);
        MIX_DestroyAudio(Music);
        Music = nullptr;
    }

    if (Num == 0) { // menu music
        switch (Pref.AudioTheme) {
        case mMaf:
            strcpy(Provi, "Sounds/menu_maf.mod");
            break;
        case mZabiden:
            strcpy(Provi, "Sounds/menu_zabiden.ogg");
            break;
        }
        Utils::GetPath(Provi);
        Music = MIX_LoadAudio(Mixer, Provi, true);
    }
    else { // in game music
        switch (Pref.AudioTheme) {
        case mMaf:
            sprintf(Provi, "Sounds/ingame%d_maf.xm", Num);
            break;
        case mZabiden:
            sprintf(Provi, "Sounds/ingame%d_zabiden.ogg", Num);
            break;
        }
        Utils::GetPath(Provi);
        Music = MIX_LoadAudio(Mixer, Provi, true);
    }
    PlayMusic();
}

/*** Switch to next game track ***/
/********************************/
void Audio::NextMusic()
{
    NMus++;
    if (NMus > 2) {
        NMus = 1;
    }
    LoadMusic(NMus);
}

/*** Plays a sound effect ***/
/****************************/
void Audio::Play(eSound index)
{
    if (!N) {
        return;
    }

    if (index == sClick) {
        if (currentTime - MemorizedTime <= 120) {
            return;
        }
        MemorizedTime = currentTime;
    }

    SDL_PropertiesID options;
    SDL_SetNumberProperty(options, MIX_PROP_PLAY_LOOPS_NUMBER, -1);
    MIX_SetTrackAudio(Track, Son[So]);
    MIX_PlayTrack(Track, options);
}

#include <stdio.h>
void Audio::PlayMusic() const
{
    if (Music && N) {
        SDL_PropertiesID options;
        SDL_SetNumberProperty(options, MIX_PROP_PLAY_LOOPS_NUMBER, -1);
        MIX_SetTrackAudio(MusicTrack, Music);
        MIX_PlayTrack(MusicTrack, options);
        DoVolume(MusicTrack);
    }
}

void Audio::PauseMusic(bool IsMusicPlaying) const
{
    if (!N) {
        return;
    }

    if (Et) {
        MIX_PauseTrack(MusicTrack);
    }
    else {
        MIX_ResumeTrack(MusicTrack);
    }
}

/*** Handles sound volumes ***/
/*****************************/
void Audio::DoVolume(MIX_Track *track) const
{
    if (!N) {
        return;
    }
    if(track == Track) {
        MIX_SetTrackGain(track, Pref.Volume);
    }

    if(track == MusicTrack) {
    if (NMus) {
        MIX_SetTrackGain(track, Pref.VolumeM);
    }
    else {
        MIX_SetTrackGain(track, Pref.VolumeM / 2);
    }
    }
}
