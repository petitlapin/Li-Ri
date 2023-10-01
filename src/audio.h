//      (_||_/
//      (    )       Classe Audio
//     ( o  0 )
//-OOO°--(_)---°OOO---------------------------------------
//                   Copyright (C) 2006 By Dominique Roux-Serret
// .OOOo      oOOO.  roux-serret@ifrance.com
//-(   )------(   )---------------------------------------
//  ( (        ) /   Le 27/05/2006
//   (_)      (_/

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

#ifndef _AUDIO_DOM_
#define _AUDIO_DOM_

#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>

/*** Enumération des sons ***/
/****************************/
enum eSon {
  sClic=0,
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
class Audio {
 public:
  Audio(void);
  ~Audio(void);

  /*** Fonctions ***/
  bool Init(void); // Initialise et charge les fichiers audio
  void LoadMusic(int Num); // Charge une music, 0 = music du menu 1,2,3,4=Jeu
  void NextMusic(void);    // Passe à la music suivante

  void Play(eSon); // Joue un son
  void PlayMusic(void); // Joue la music

  void Pause(bool Etat); // Met ou non en pause
  void PauseMusic(bool Etat); // Met ou no la music en pause

  void DoVolume(void); // Valide les volumes audio
  
  /*** Variables ***/
  int N; // Nombre d'échantillon audio
  int NMus; // Numéro de la music en cours
  int MemoHorloge; // Mémorise l'horloge pour les clics
  Mix_Chunk **Son; // Pointe sur les sons
  Mix_Music *Music; // Pointe sur les musics
};

#endif
