//      (_||_/
//      (    )       Audio class
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

#ifndef AUDIO_DOM_
#define AUDIO_DOM_

#include <SDL2/SDL_mixer.h>

/*** Enumeration of sounds***/
/****************************/
enum eSon {
    sClic = 0,
    sSpeed,
    sCrash,
    sEnd,
    sLose,
    sEtire,
    sWagon,
    sReduit,
    sLive,
    sFin
};

/***       Audio class define      ***/
/*************************************/
class Audio
{
public:
    Audio() = default;
    ~Audio();

    /*** Functions ***/
    bool Init(); // Initialization of audio
    void LoadMusic(int Num); // Load music, 0 = music for menu 1,2,3,4=Jeu
    void NextMusic(); // Skip to the next game music

    void Play(eSon); // Plays a sound
    void PlayMusic() const; // Plays a music

    void PauseMusic(bool Etat) const; // Pause music

    void DoVolume() const; // Validates Volumes
    Mix_Music *Music { nullptr }; // Music pointer

private:
    /*** Variables ***/
    int N { 0 }; // Number of audio samples
    int NMus { 0 }; // Current music number
    int MemoHorloge { 0 }; // Remembers the time for clicks
    Mix_Chunk **Son { nullptr }; // Sound pointer
};

#endif
