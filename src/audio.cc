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

#include "audio.h"
#include "utils.h"
#include "preference.h"

/*** Variable globales ***/
/*************************/
extern sPreference Pref;
extern int Horloge;

/*** Constructeur et Destructeur ***/
/***********************************/
Audio::~Audio()
{
    if (N) {
        Mix_HaltChannel(-1);
        for (int i = 0; i < N; i++) {
            if (Son[i]) {
                Mix_FreeChunk(Son[i]);
            }
        }
        delete[] Son;
    }
    Mix_CloseAudio();
}

/*** Initialise l'Audio ***/
/**************************/
bool Audio::Init()
{
    char PathFile[512];

    if (Mix_OpenAudio(22050, AUDIO_S16, 1, 1024)) {
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Enable to init Sound card: %s", SDL_GetError());
        return false;
    }

    /*** Allocation de la mémoire ***/
    N = sFin;
    Son = new Mix_Chunk *[sFin];

    /*** Chargement des sons ***/
    strcpy(PathFile, "Sounds/clic.wav");
    Utils::GetPath(PathFile);
    Son[sClic] = Mix_LoadWAV(PathFile);

    strcpy(PathFile, "Sounds/speed.wav");
    Utils::GetPath(PathFile);
    Son[sSpeed] = Mix_LoadWAV(PathFile);

    strcpy(PathFile, "Sounds/crash.wav");
    Utils::GetPath(PathFile);
    Son[sCrash] = Mix_LoadWAV(PathFile);

    strcpy(PathFile, "Sounds/end.wav");
    Utils::GetPath(PathFile);
    Son[sEnd] = Mix_LoadWAV(PathFile);

    strcpy(PathFile, "Sounds/lose.wav");
    Utils::GetPath(PathFile);
    Son[sLose] = Mix_LoadWAV(PathFile);

    strcpy(PathFile, "Sounds/etire.wav");
    Utils::GetPath(PathFile);
    Son[sEtire] = Mix_LoadWAV(PathFile);

    strcpy(PathFile, "Sounds/wagon.wav");
    Utils::GetPath(PathFile);
    Son[sWagon] = Mix_LoadWAV(PathFile);

    strcpy(PathFile, "Sounds/reduit.wav");
    Utils::GetPath(PathFile);
    Son[sReduit] = Mix_LoadWAV(PathFile);

    strcpy(PathFile, "Sounds/live.wav");
    Utils::GetPath(PathFile);
    Son[sLive] = Mix_LoadWAV(PathFile);

    strcpy(PathFile, "Sounds/menu.mod");
    Utils::GetPath(PathFile);
    Music = Mix_LoadMUS(PathFile);

    return true;
}

/*** Charge une music 0=menu 1,2,3,4 = game ***/
/**********************************************/
void Audio::LoadMusic(int Num)
{
    char Provi[512] = "Sounds/jeu1.xm";

    if (!N) {
        return;
    }

    NMus = Num;

    if (Music) {
        PauseMusic(true);
        Mix_HaltMusic(); // Arrete la music
        Mix_FreeMusic(Music);
        Music = nullptr;
    }

    if (Num == 0) { // Si music du menu
        strcpy(Provi, "Sounds/menu.mod");
        Utils::GetPath(Provi);
        Music = Mix_LoadMUS(Provi);
    }
    else {
        Provi[10] = (char)(Num) + '0';
        Utils::GetPath(Provi);
        Music = Mix_LoadMUS(Provi);
    }
    PlayMusic();
}

/*** Passe à la music de jeu suivante ***/
/****************************************/
void Audio::NextMusic()
{
    NMus++;
    if (NMus > 2) {
        NMus = 1;
    }
    LoadMusic(NMus);
}

/*** Fait la lecture d'un son ***/
/********************************/
void Audio::Play(eSon So)
{
    if (!N) {
        return;
    }

    if (So == sClic) {
        if (Horloge - MemoHorloge <= 120) {
            return;
        }
        MemoHorloge = Horloge;
    }

    Mix_PlayChannel(-1, Son[So], 0);
}

/*** Joue la music ***/
/*********************/
void Audio::PlayMusic() const
{
    if (Music && N) {
        Mix_PlayMusic(Music, -1);
        DoVolume();
    }
}

void Audio::PauseMusic(bool Et) const
{
    if (!N) {
        return;
    }

    if (Et) {
        Mix_PauseMusic();
    }
    else {
        Mix_ResumeMusic();
    }
}

/*** Valide les Volumes ***/
/**************************/
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
