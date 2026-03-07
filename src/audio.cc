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

#include <SDL2/SDL_audio.h> // for AUDIO_S16
#include <SDL2/SDL_error.h> // for SDL_GetError
#include <SDL2/SDL_log.h> // for SDL_LogInfo, SDL_LOG_CATEGORY_APPLICATION
#include <cstring>
#include <cstdio>

#include "audio.h"
#include "utils.h"
#include "preference.h"

extern sNewPreference Pref;
extern int currentTime;

Audio::~Audio()
{
    if (N) {
        Mix_HaltChannel(-1);
        for (int i = 0; i < N; i++) {
            if (Sound[i]) {
                Mix_FreeChunk(Sound[i]);
            }
        }
        delete[] Sound;
    }
    Mix_CloseAudio();
}

bool Audio::Init()
{
    char PathFile[512];

    if (Mix_OpenAudio(44100, AUDIO_S16, 1, 1024)) {
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Enable to init Sound card: %s", SDL_GetError());
        return false;
    }

    /*** Memory allocation ***/
    N = sSize;
    Sound = new Mix_Chunk *[sSize];

    /*** Loading sound effects ***/
    strcpy(PathFile, "Sounds/click.wav");
    Utils::GetPath(PathFile);
    Sound[sClick] = Mix_LoadWAV(PathFile);

    strcpy(PathFile, "Sounds/speed.wav");
    Utils::GetPath(PathFile);
    Sound[sSpeed] = Mix_LoadWAV(PathFile);

    strcpy(PathFile, "Sounds/crash.wav");
    Utils::GetPath(PathFile);
    Sound[sCrash] = Mix_LoadWAV(PathFile);

    strcpy(PathFile, "Sounds/end.wav");
    Utils::GetPath(PathFile);
    Sound[sEnd] = Mix_LoadWAV(PathFile);

    strcpy(PathFile, "Sounds/lose.wav");
    Utils::GetPath(PathFile);
    Sound[sLose] = Mix_LoadWAV(PathFile);

    strcpy(PathFile, "Sounds/expand.wav");
    Utils::GetPath(PathFile);
    Sound[sExpand] = Mix_LoadWAV(PathFile);

    strcpy(PathFile, "Sounds/wagon.wav");
    Utils::GetPath(PathFile);
    Sound[sWagon] = Mix_LoadWAV(PathFile);

    strcpy(PathFile, "Sounds/shrink.wav");
    Utils::GetPath(PathFile);
    Sound[sShrink] = Mix_LoadWAV(PathFile);

    strcpy(PathFile, "Sounds/live.wav");
    Utils::GetPath(PathFile);
    Sound[sLive] = Mix_LoadWAV(PathFile);

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
        Mix_HaltMusic();
        Mix_FreeMusic(Music);
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
        Music = Mix_LoadMUS(Provi);
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
        Music = Mix_LoadMUS(Provi);
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

    Mix_PlayChannel(-1, Sound[index], 0);
}

/*** Plays the music ***/
/***********************/
void Audio::PlayMusic() const
{
    if (Music && N) {
        Mix_PlayMusic(Music, -1);
        DoVolume();
    }
}

void Audio::PauseMusic(bool IsMusicPlaying) const
{
    if (!N) {
        return;
    }

    if (IsMusicPlaying) {
        Mix_PauseMusic();
    }
    else {
        Mix_ResumeMusic();
    }
}

/*** Handles sound volumes ***/
/*****************************/
void Audio::DoVolume() const
{
    if (!N) {
        return;
    }

    Mix_Volume(-1, (int)Pref.Volume);
    if (NMus) {
        Mix_VolumeMusic((int)Pref.VolumeM);
    }
    else {
        Mix_VolumeMusic((int)Pref.VolumeM / 2);
    }
}
