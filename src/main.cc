//      (_||_/
//      (    )       Programme Principale
//     ( o  0 )
//-OOO°--(_)---°OOO---------------------------------------
//                   Copyright (C) 2006 By Dominique Roux-Serret
// .OOOo      oOOO.  roux-serret@ifrance.com
//-(   )------(   )---------------------------------------
//  ( (        ) /   Le 03/01/2006
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

#include <stdio.h>
#include <stdlib.h>
#include <iostream.h>
#include <string.h>
#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>

#include "preference.h"
#include "jeux.h"
#include "audio.h"
#include "sprite.h"
#include "ecran.h"
#include "mouse.h"
#include "menu.h"
#include "tableau.h"
#include "editeur.h"
#include "utils.h"

/*** Variables globales ***/
/************************/
SDL_Surface *sdlVideo; // Pointe sur l'écran video
SDL_VideoInfo *sdlVideoInfo; // Infos sur la video
Uint32 FontColor;      // Couleur du fond d'écran

char Titre[]="Ri-li V2.0.1";

Sprite *Sprites=NULL; // Pointe sur les sprites
int NSprites=0; // Nombre de sprites en mémoire
Ecran Ec[2];          // Pointe sur les 2 buffets vidéo
sPreference Pref;     // Tableau des préférences.
Jeux Jeu;             // Gère le jeu
Mouse Sourie;         // Gère les mouvements de sourie
Menu MenuPrincipale;  // Gère les menus
Tableau Niveau;       // Gère les niveaux
Editeur Edit;         // Gère le menu de l'éditeur
Audio Sons;           // Gère les sons

int Horloge=0; // Horloges du jeu
int HorlogeAvant=0;

#ifdef LINUX
char DefPath[256]; // Chemin par defaut dans arg
#endif

/*** Initialise les preferences ***/
/**********************************/
void InitPref(void)
{
#ifdef LINUX
  DefPath[0]=0;
#endif

  Pref.NiveauMax=0;
  Pref.FullScreen=false;
  Pref.Langue=-1;
  Pref.Volume=(float)SDL_MIX_MAXVOLUME;
  Pref.VolumeM=(float)SDL_MIX_MAXVOLUME; //*6.0/10.0;

  for(int i=0;i<8;i++) { // Vide les scores
    Pref.Sco[i].Score=0;
    Pref.Sco[i].Name[0]=0;
  }
/*   Pref.Sco[0].Score=11425; */
/*   sprintf(Pref.Sco[0].Name,"%s","Dominique"); */
/*   Pref.Sco[1].Score=678; */
/*   sprintf(Pref.Sco[1].Name,"%s","Veronique"); */

  LoadPref();

  Pref.Difficulte=Normal;
  Pref.Vitesse=VITESSE_MOY;
  Pref.VitesseMoy=VITESSE_MOY;
  Pref.NVie=N_VIES_DEP;
  Pref.EcartWagon=ECARTWAGON_MOY;
}

/*** Preogramme principale ***/
/*****************************/
int main(int narg,char *argv[])
{
  int i;
  char **pTitre=NULL;
  char **pIcon=NULL;
  Sprite Spr;
  eMenu RetM,RetMenu=mMenu;

  // Initialuse les préferences
  InitPref();
#ifdef LINUX
  if(narg>1) strcpy(DefPath,argv[1]);
#endif
 
  // Initilise SDL
  if( SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER|SDL_INIT_AUDIO|SDL_INIT_NOPARACHUTE) < 0 ) {
    cerr <<"Impossible d'initialiser SDL:"<<SDL_GetError()<<endl;
    exit(-1);
  }
  // Ferme le programme correctement quant quit
  atexit(SDL_Quit);
    
  // Teste la resolution video
  sdlVideoInfo=(SDL_VideoInfo*)SDL_GetVideoInfo();

  if(sdlVideoInfo->vfmt->BitsPerPixel==8) {
    cerr <<"Impossible d'utiliser 8bits pour la vidéo !"<<endl;
    exit(-1);
  }
  
  // Demande la resolution Video
#ifndef LINUX
  int vOption;
  if(Pref.FullScreen)   vOption=SDL_SWSURFACE; // Bug accé aux bits pour les cordes
  else  vOption=SDL_SWSURFACE|SDL_DOUBLEBUF;
#else
#ifndef __AMIGAOS4__
  int vOption=SDL_SWSURFACE|SDL_DOUBLEBUF;
#else
  int vOption=SDL_SWSURFACE;
#endif
#endif
  if(Pref.FullScreen) vOption|=SDL_FULLSCREEN;
  sdlVideo=SDL_SetVideoMode(800,600,sdlVideoInfo->vfmt->BitsPerPixel,vOption);

  if(sdlVideo==NULL) {
    cerr <<"Impossible de passer dans le mode vidéo 800x600 !"<<endl;
    exit(-1);
  }
  // Change le nom de la fenetre
  SDL_WM_GetCaption(pTitre,pIcon);
  SDL_WM_SetCaption(Titre,NULL);
  SDL_ShowCursor(0); // Masque le curseur
  SDL_EnableUNICODE(1);

  // Couleur du font d'écran
  FontColor=SDL_MapRGB(sdlVideo->format,128,128,128);

  // Chargement des sprites
  Sons.Init();
  if(LoadSprites()==false) exit(-1);
  if(Niveau.Load()==false) exit(-1);

  Sons.PlayMusic();
  Sourie.InitStart();

  // Initialise l'horloge et le hazard
  HorlogeAvant=Horloge=SDL_GetTicks();
  srand(SDL_GetTicks());

  // Si pas de langues demande la langue
  if(Pref.Langue==-1) MenuPrincipale.SDLMain_Langue();
  
  // Gère les menus
  do {
    switch(RetMenu) {
    case mMenu:
      RetM=MenuPrincipale.SDLMain();
      break;
    case mLangue:
      RetM=MenuPrincipale.SDLMain_Langue();
      break;
    case mOption:
      RetM=MenuPrincipale.SDLMain_Options();
      break;
    case mScoreEdit:
      RetM=MenuPrincipale.SDLMain_Score(true);
      break;
    case mScore:
      RetM=MenuPrincipale.SDLMain_Score();
      break;
    case mMenuSpeed:
      RetM=MenuPrincipale.SDLMain_Speed();
      break;
    case mMenuNiveau:
      RetM=MenuPrincipale.SDLMain_Niveau();
      break;
    case mJeux:
      Sons.LoadMusic(1);
      RetM=Jeu.SDLMain();
      Sons.LoadMusic(0);
      break;
    case mEdit:
      RetM=Edit.SDLMain(0);
      break;
    default:
      RetM=mQuit;
    }
    RetMenu=RetM;
  } while(RetMenu!=mQuit);
  
  // Ferme proprement le programme
  Mix_HaltMusic(); // Arrete la music
  Mix_FreeMusic(Sons.Music); // Efface la music
  
  for(i=0;i<NSprites;i++) { // Efface les sprites
    Sprites[i].Delete();
  }
  delete [] Sprites;

  SauvePref(); // Sauve les preferences

  exit(0);
}
