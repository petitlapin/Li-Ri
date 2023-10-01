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

#include <stdlib.h>
#include <iostream.h>
#include <string.h>

#include "audio.h"
#include "utils.h"
#include "preference.h"

/*** Variable globales ***/
/*************************/
extern sPreference Pref;
extern int Horloge;

/*** Constructeur et Destructeur ***/
/***********************************/
Audio::Audio(void) : N(0), NMus(0), MemoHorloge(0), Son(NULL), Music(NULL)
{ }

Audio::~Audio(void)
{
  if(N) {
    Mix_HaltChannel(-1);
    for(int i=0;i<N;i++) 
      if(Son[i]) Mix_FreeChunk(Son[i]);
    delete [] Son;
  }
  Mix_CloseAudio();
}

/*** Initialise l'Audio ***/
/**************************/
bool Audio::Init(void)
{
  char PathFile[512];

  if(Mix_OpenAudio(22050,AUDIO_S16,1,1024)) {
    cerr <<"Enable to init Sound card ! "<<SDL_GetError()<<endl;
    return false;
  }
  
  /*** Allocation de la mémoire ***/
  N=sFin;
  Son=new Mix_Chunk* [sFin];
  
  /*** Chargement des sons ***/
  strcpy(PathFile,"Sounds/clic.wav");
  GetPath(PathFile);
  Son[sClic]=Mix_LoadWAV(PathFile);

  strcpy(PathFile,"Sounds/speed.wav");
  GetPath(PathFile);
  Son[sSpeed]=Mix_LoadWAV(PathFile);

  strcpy(PathFile,"Sounds/crash.wav");
  GetPath(PathFile);
  Son[sCrash]=Mix_LoadWAV(PathFile);

  strcpy(PathFile,"Sounds/end.wav");
  GetPath(PathFile);
  Son[sEnd]=Mix_LoadWAV(PathFile);

  strcpy(PathFile,"Sounds/lose.wav");
  GetPath(PathFile);
  Son[sLose]=Mix_LoadWAV(PathFile);

  strcpy(PathFile,"Sounds/etire.wav");
  GetPath(PathFile);
  Son[sEtire]=Mix_LoadWAV(PathFile);

  strcpy(PathFile,"Sounds/wagon.wav");
  GetPath(PathFile);
  Son[sWagon]=Mix_LoadWAV(PathFile);

  strcpy(PathFile,"Sounds/reduit.wav");
  GetPath(PathFile);
  Son[sReduit]=Mix_LoadWAV(PathFile);

  strcpy(PathFile,"Sounds/live.wav");
  GetPath(PathFile);
  Son[sLive]=Mix_LoadWAV(PathFile);

  strcpy(PathFile,"Sounds/menu.mod");
  GetPath(PathFile);
  Music=Mix_LoadMUS(PathFile);
    
  return true;
}

/*** Charge une music 0=menu 1,2,3,4 = game ***/
/**********************************************/
void Audio::LoadMusic(int Num)
{
  char Provi[512]="Sounds/jeu1.xm";

  if(!N) return;

  NMus=Num;

  if(Music) {
    PauseMusic(true);
    Mix_HaltMusic(); // Arrete la music
    Mix_FreeMusic(Music);
    Music=NULL;
  }

  if(Num==0) { // Si music du menu
    strcpy(Provi,"Sounds/menu.mod");
    GetPath(Provi);
    Music=Mix_LoadMUS(Provi);
  }
  else {
    Provi[10]=(char)(Num)+'0';
    GetPath(Provi);
    Music=Mix_LoadMUS(Provi);
  }
  PlayMusic();
}

/*** Passe à la music de jeu suivante ***/
/****************************************/
void Audio::NextMusic(void)
{
  NMus++;
  if(NMus>2) NMus=1;
  LoadMusic(NMus);
}

/*** Fait la lecture d'un son ***/
/********************************/
void Audio::Play(eSon So) {
  if(!N) return;

  if(So==sClic) {
    if(Horloge-MemoHorloge<=120) return;
    MemoHorloge=Horloge;
  }
  
  Mix_PlayChannel(-1,Son[So],0);
}

/*** Joue la music ***/
/*********************/
void Audio::PlayMusic(void) {
  if(Music && N) {
    Mix_PlayMusic(Music,-1);
    DoVolume();
  }
}

/*** Met ou non en pause ***/
/***************************/
void Audio::Pause(bool Et)
{
  if(!N) return;

  if(Et) SDL_PauseAudio(1);
  else SDL_PauseAudio(0);
}

void Audio::PauseMusic(bool Et)
{
  if(!N) return;

  if(Et) Mix_PauseMusic();
  else Mix_ResumeMusic();
}

/*** Valide les Volumes ***/
/**************************/
void Audio::DoVolume(void)
{
  if(!N) return;

  Mix_Volume(-1,(int)Pref.Volume);
  if(NMus) Mix_VolumeMusic((int)Pref.VolumeM);
  else Mix_VolumeMusic((int)Pref.VolumeM/2);
}
