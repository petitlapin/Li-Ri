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

#include <map>
#include <string>

#include "audio.h"
#include "utils.h"
#include "preference.h"

/*** Variable globales ***/
/*************************/
extern sNewPreference Pref;
extern int currentTime;

/*** Constructeur et Destructeur ***/
/***********************************/
void Audio::Stop()
{
    MIX_StopTrack(Track, 0);
    if (N) {
        for (int i = 0; i < N; i++) {
            if (Son[i]) {
                MIX_DestroyAudio(Son[i]);
            }
        }
        delete[] Son;
    }
    SDL_DestroyProperties(musicOptions);
    SDL_DestroyProperties(soundOptions);
    MIX_DestroyAudio(Music);
    MIX_DestroyTrack(Track);
    MIX_DestroyTrack(MusicTrack);
    MIX_DestroyMixer(Mixer);
}

/*** Initialise l'Audio ***/
/**************************/
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

    /*** Allocation de la mémoire ***/
    N = sFin;
    Son = new MIX_Audio *[sFin];

    /*** Chargement des sons ***/
    std::map<eSon, std::string> sounds {
        { sClic, "Sounds/clic.wav" },
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
        Son[sound.first] = MIX_LoadAudio(Mixer, PathFile, true);
    }

    strcpy(PathFile, "Sounds/menu.mod");
    Utils::GetPath(PathFile);
    Music = MIX_LoadAudio(Mixer, PathFile, true);

    musicOptions = SDL_CreateProperties();
    SDL_SetNumberProperty(musicOptions, MIX_PROP_PLAY_LOOPS_NUMBER, -1);
    soundOptions = SDL_CreateProperties();
    SDL_SetNumberProperty(soundOptions, MIX_PROP_PLAY_LOOPS_NUMBER, -1);

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
        MIX_StopTrack(Track, 0);
        MIX_DestroyAudio(Music);
        Music = nullptr;
    }

    if (Num == 0) { // Si music du menu
        strcpy(Provi, "Sounds/menu.mod");
        Utils::GetPath(Provi);
        Music = MIX_LoadAudio(Mixer, Provi, true);
    }
    else {
        Provi[10] = (char)(Num) + '0';
        Utils::GetPath(Provi);
        Music = MIX_LoadAudio(Mixer, Provi, true);
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
        if (currentTime - MemorizedTime <= 120) {
            return;
        }
        MemorizedTime = currentTime;
    }

    MIX_SetTrackAudio(Track, Son[So]);
    MIX_PlayTrack(Track, soundOptions);
}

/*** Joue la music ***/
/*********************/
void Audio::PlayMusic() const
{
    if (Music && N) {
        MIX_SetTrackAudio(MusicTrack, Music);
        MIX_PlayTrack(MusicTrack, musicOptions);
        DoVolume(MusicTrack);
    }
}

void Audio::PauseMusic(bool Et) const
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

/*** Valide les Volumes ***/
/**************************/
void Audio::DoVolume(MIX_Track *track) const
{
    if (!N) {
        return;
    }
    if (track == Track) {
        MIX_SetTrackGain(track, Pref.Volume);
    }

    if (track == MusicTrack) {
        if (NMus) {
            MIX_SetTrackGain(track, Pref.VolumeM);
        }
        else {
            MIX_SetTrackGain(track, Pref.VolumeM / 2);
        }
    }
}
