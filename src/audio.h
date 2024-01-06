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

/*** Enumération des sons ***/
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

/*** Définition de la classe Audio ***/
/*************************************/
class Audio
{
public:
    Audio() = default;
    ~Audio();

    /*** Fonctions ***/
    bool Init(); // Initialise et charge les fichiers audio
    void LoadMusic(int Num); // Charge une music, 0 = music du menu 1,2,3,4=Jeu
    void NextMusic(); // Passe à la music suivante

    void Play(eSon); // Joue un son
    void PlayMusic() const; // Joue la music

    void PauseMusic(bool Etat) const; // Met ou no la music en pause

    void DoVolume() const; // Valide les volumes audio
    Mix_Music *Music { nullptr }; // Pointe sur les musics

private:
    /*** Variables ***/
    int N { 0 }; // Nombre d'échantillon audio
    int NMus { 0 }; // Numéro de la music en cours
    int MemoHorloge { 0 }; // Mémorise l'horloge pour les clics
    Mix_Chunk **Son { nullptr }; // Pointe sur les sons
};

#endif
