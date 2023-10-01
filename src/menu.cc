//      (_||_/
//      (    )       Classe Menu
//     ( o  0 )
//-OOO°--(_)---°OOO---------------------------------------
//                   Copyright (C) 2006 By Dominique Roux-Serret
// .OOOo      oOOO.  roux-serret@ifrance.com
//-(   )------(   )---------------------------------------
//  ( (        ) /   Le 23/03/2006
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

#include <iostream.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "preference.h"
#include "menu.h"
#include "sprite.h"
#include "ecran.h"
#include "mouse.h"
#include "audio.h"

#define PY 180

/*** Variables globales ***/
/**************************/
extern int Horloge;
extern int HorlogeAvant;
extern SDL_Surface *sdlVideo;
extern Sprite *Sprites;
extern Mouse Sourie;
extern Ecran Ec[2];
extern sPreference Pref;
extern Audio Sons;

static char Points[]=". . . . . . . . . . . . . .";
static struct mPy Menu_Py[20];

/*** Fait une attente pour 50fps ***/
/***********************************/
void Sleeping()
{
  int delay;

  if( (Horloge-HorlogeAvant) < (1000/60) ) {
    delay=1000/60-(Horloge-HorlogeAvant);
    SDL_Delay(delay);
    Horloge=SDL_GetTicks();
  }
}

/*** Constructeur et Destructeur ***/
/***********************************/
Menu::Menu() : PyE(0)
{ }

Menu::~Menu()
{ }

/*** Ajoute une entrée dans le tableau des boutons ***/
/*****************************************************/
void AddBouton(int Num,e_Sprite NumSp,int X,int Y)
{
  int NumS=(int)NumSp;

  Menu_Py[Num].DepX=X-Sprites[NumS].Dim[0].L/2;
  Menu_Py[Num].DepY=Y-Sprites[NumS].Dim[0].H/2;
  Menu_Py[Num].FinX=X+Sprites[NumS].Dim[0].L/2;
  Menu_Py[Num].FinY=Y+Sprites[NumS].Dim[0].H/2;
  Menu_Py[Num].Py=Num;
  Menu_Py[Num].Valide=true;
}

/*** Change le vidéo ***/
/***********************/
void ChangeVideo(void)
{
  SDL_VideoInfo *sdlVideoInfo;

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
  
  SDL_ShowCursor(0); // Cache le curseur
}

/*** SDL Main Menu principale ***/
/********************************/
eMenu Menu::SDLMain(void)
{
  int i;
  char MCode[5]={0,0,0,0,0};
  char key;

  // Initialisations Divers
  Sourie.Init(Menu_Py); // Initialise la sourie
  PyE=0;
  
  // Prend l'image du fond et fait l'affichage 
  Sprites[fond_menu].Affiche(400,300,0,Sprites[fmenu].Image[0]);
  Sprites[menu].Affiche(400,340,0,Sprites[fmenu].Image[0]);
  Sprites[titre].Affiche(400,65,0,Sprites[fmenu].Image[0]);
  Sprites[copyright].Affiche(400,587,0,Sprites[fmenu].Image[0]);

  AfficheText(400,229,T_play,Sprites[fmenu].Image[0]);
  AddBouton(0,T_play,400,229);
  AfficheText(400,306,T_scores,Sprites[fmenu].Image[0]);
  AddBouton(1,T_scores,400,306);
  AfficheText(400,384,T_moptions,Sprites[fmenu].Image[0]);
  AddBouton(2,T_moptions,400,384);
  AfficheText(400,461,T_quit,Sprites[fmenu].Image[0]);
  AddBouton(3,T_quit,400,461);
  Menu_Py[4].DepX=-1;

  // Efface le fond
  NumEc=0;
  Ec[1].Cls(fmenu);
  SDL_Flip(sdlVideo);
  Ec[0].Cls(fmenu);
  
  // Prend les evenements
  do {
    SDL_Event event;
    while(SDL_PollEvent(&event)) {
      Sourie.GetEvent(event,PyE); // Prend les evenements de la sourie
      switch(event.type) {
      case SDL_ACTIVEEVENT:
	if(event.active.gain==1) {
	  NumEc=0;
	  Ec[1].Cls(fmenu);
	  SDL_Flip(sdlVideo);
	  Ec[0].Cls(fmenu);
	}
	break;
      case SDL_KEYDOWN:
	if(event.key.state==SDL_PRESSED) {
	  Sons.Play(sClic);
	  switch(event.key.keysym.sym) {
	  case SDLK_ESCAPE:
	    return mQuit;
	  case SDLK_UP:
	    PyE--;
	    if(PyE<0) PyE=3;
	    break;
	  case SDLK_DOWN:
	    PyE++;
	    if(PyE>3) PyE=0;
	    break;
	  case SDLK_F12: // Sauve un screenshot
	    SDL_SaveBMP(sdlVideo,"screenshot.bmp");
	    break;    
	  case ' ':
	  case SDLK_RETURN:
	  case SDLK_KP_ENTER:
	    switch(PyE) {
	    case 0:
	      return mMenuNiveau;
	    case 1:
	      return mScore;
	    case 2:
	      return mOption;
	    case 3:
	      return mQuit;
	    }
	    break;
	  default:
	    key=event.key.keysym.unicode&0x7F; // Prend le caracataire correspondant à la touche
	    if(CharExiste(key)==true) { // Si la caractaire existe bien
	      for(i=2;i>=0;i--) MCode[i+1]=MCode[i]; // décale le code
	      MCode[0]=key;
	      if(strcmp(MCode,"tide")==0 || strcmp(MCode,"TIDE")==0) return mEdit; // Si editeur de niveau
	    } 
	  }
	}
	break;
      case SDL_QUIT:
	return mQuit;
	break;
      }
    }
    
    // Gère les variables
    HorlogeAvant=Horloge;
    Horloge=SDL_GetTicks();
    Sleeping();
    
    // Gère l'Affichage
    Ec[NumEc].Efface(fmenu);
    Affiche_Main();
    Sourie.Affiche(NumEc);
    
    // Echange les buffets video
    SDL_Flip(sdlVideo);
    NumEc=(NumEc+1)&1;
    
  } while(true);
  
  return mQuit;
}

/*** SDL Main Menu Choix de la langue ***/
/****************************************/
eMenu Menu::SDLMain_Langue(void)
{
  int NCol=1;
  int NL;
  int Ecart;
  int i;
  int x,y;
  int OldLangue=Pref.Langue;

  // Initialisations Divers
  Sourie.Init(Menu_Py); // Initialise la sourie
  PyE=Pref.Langue;
  if(PyE==-1) PyE=1;

  // Prend l'image du fond et fait l'affichage 
  Sprites[fond_menu].Affiche(400,300,0,Sprites[fmenu].Image[0]);
  
  // Affiche les langues possibles
  NCol=3;
  if(Pref.NLangues%NCol==0) NL=Pref.NLangues/NCol;
  else NL=Pref.NLangues/NCol+1;
  Ecart=600/(NL+1);

  for(i=0;i<Pref.NLangues;i++) {
    x=(i/NL)*(800/3)+(800/6);
    y=(i%NL)*Ecart+Ecart;
    
    Sprites[T_Langue+i].Affiche(x,y,0,Sprites[fmenu].Image[0]);
    AddBouton(i,(e_Sprite)(T_Langue+i),x,y);
  }
  
  Menu_Py[Pref.NLangues].DepX=-1;

  // Efface le fond
  NumEc=0;
  Ec[1].Cls(fmenu);
  SDL_Flip(sdlVideo);
  Ec[0].Cls(fmenu);
  
  // Prend les evenements
  do {
    SDL_Event event;
    while(SDL_PollEvent(&event)) {
      Sourie.GetEvent(event,PyE); // Prend les evenements de la sourie
      switch(event.type) {
      case SDL_ACTIVEEVENT:
	if(event.active.gain==1) {
	  NumEc=0;
	  Ec[1].Cls(fmenu);
	  SDL_Flip(sdlVideo);
	  Ec[0].Cls(fmenu);
	}
	break;
      case SDL_KEYDOWN:
	if(event.key.state==SDL_PRESSED) {
	  Sons.Play(sClic);
	  switch(event.key.keysym.sym) {
	  case SDLK_ESCAPE:
	    if(Pref.Langue==-1) Pref.Langue=PyE;
	    return mMenu;
	  case SDLK_UP:
	    PyE--;
	    if(PyE<0) PyE=Pref.NLangues-1;
	    break;
	  case SDLK_DOWN:
	    PyE++;
	    if(PyE>=Pref.NLangues) PyE=0;
	    break;
	  case SDLK_LEFT:
	    if(PyE-NL>=0) PyE-=NL;
	    break;
	  case SDLK_RIGHT:
	    if(PyE+NL<Pref.NLangues) PyE+=NL;
	    break;
	  case SDLK_F12: // Sauve un screenshot
	    SDL_SaveBMP(sdlVideo,"screenshot.bmp");
	    break;    
	  case ' ':
	  case SDLK_RETURN:
	  case SDLK_KP_ENTER:
	    Pref.Langue=PyE;
	    if(Pref.Langue!=OldLangue) LoadLangue();
	    return mMenu;
	  }
	}
	break;
      case SDL_QUIT:
	return mQuit;
	break;
      }
    }
    
    // Gère les variables
    HorlogeAvant=Horloge;
    Horloge=SDL_GetTicks();
    Sleeping();
    
    // Gère l'Affichage
    Ec[NumEc].Efface(fmenu);

    Affiche_Main_Centre();

    Sourie.Affiche(NumEc);

    // Echange les buffets video
    SDL_Flip(sdlVideo);
    NumEc=(NumEc+1)&1;
    
  } while(true);
  
  return mQuit;
}

/*** SDL Main Menu Choix des Options ***/
/***************************************/
void Menu::InitMain_Options(void)
{
  // Initialisations Divers
  Sourie.Init(Menu_Py); // Initialise la sourie
  PyE=4;

  // Prend l'image du fond et fait l'affichage 
  Sprites[fond_menu].Affiche(400,300,0,Sprites[fmenu].Image[0]);
  Sprites[gmenu].Affiche(400,300,0,Sprites[fmenu].Image[0]);
  Sprites[keys].Affiche(610,455,0,Sprites[fmenu].Image[0]);

  AddBouton(0,bruitage,140,110);
  AddBouton(1,music,160,200);

  AddBouton(2,fscreen,190,300);
  Menu_Py[2].DepX-=40;
  Menu_Py[2].FinX=625+40+Sprites[fscreen].Dim[0].L/2;
  Sprites[fscreen].Affiche(185,300,0,Sprites[fmenu].Image[0]);
  Sprites[window].Affiche(625,300,0,Sprites[fmenu].Image[0]);

  AddBouton(3,monde,180,400);

  // Centre à gauche le text de menu
  CentreM=120+Sprites[T_menu].Dim[0].L/2;
  AfficheText(CentreM,490,T_menu,Sprites[fmenu].Image[0]);
  AddBouton(4,T_menu,CentreM,490);

  // Boutons des bruitages
  Sprites[fleches].Affiche(250,110,1,Sprites[fmenu].Image[0]);
  Sprites[fleches].Affiche(700,110,4,Sprites[fmenu].Image[0]);
  Menu_Py[5].DepX=230;
  Menu_Py[5].DepY=70;
  Menu_Py[5].FinX=475;
  Menu_Py[5].FinY=145;
  Menu_Py[5].Py=5;
  Menu_Py[5].Valide=true;
  
  Menu_Py[6].DepX=476;
  Menu_Py[6].DepY=70;
  Menu_Py[6].FinX=720;
  Menu_Py[6].FinY=145;
  Menu_Py[6].Py=6;
  Menu_Py[6].Valide=true;

  // Boutons de musics
  Sprites[fleches].Affiche(250,200,1,Sprites[fmenu].Image[0]);
  Sprites[fleches].Affiche(700,200,4,Sprites[fmenu].Image[0]);
  Menu_Py[7].DepX=230;
  Menu_Py[7].DepY=155;
  Menu_Py[7].FinX=475;
  Menu_Py[7].FinY=245;
  Menu_Py[7].Py=7;
  Menu_Py[7].Valide=true;
  
  Menu_Py[8].DepX=476;
  Menu_Py[8].DepY=155;
  Menu_Py[8].FinX=720;
  Menu_Py[8].FinY=245;
  Menu_Py[8].Py=8;
  Menu_Py[8].Valide=true;

  Menu_Py[9].DepX=-1;

  // Efface le fond
  NumEc=0;
  Ec[1].Cls(fmenu);
  SDL_Flip(sdlVideo);
  Ec[0].Cls(fmenu);
}
  
/*** Gestion du menu Options ***/
/*******************************/
eMenu Menu::SDLMain_Options(void)
{
  int i,N;
  int NumSp;

  InitMain_Options(); // Prépare le menu

  // Prend les evenements
  do {
    SDL_Event event;
    while(SDL_PollEvent(&event)) {
      Sourie.GetEvent(event,PyE); // Prend les evenements de la sourie
      switch(event.type) {
      case SDL_ACTIVEEVENT:
	if(event.active.gain==1) {
	  NumEc=0;
	  Ec[1].Cls(fmenu);
	  SDL_Flip(sdlVideo);
	  Ec[0].Cls(fmenu);
	}
	break;
      case SDL_KEYDOWN:
	if(event.key.state==SDL_PRESSED) {
	  Sons.Play(sClic);
	  switch(event.key.keysym.sym) {
	  case SDLK_ESCAPE:
	    return mMenu;
	  case SDLK_LEFT:
	    switch(PyE) {
	    case 2:
	      if(Pref.FullScreen==false) {
		Pref.FullScreen=true;
		ChangeVideo();
		InitMain_Options();
		PyE=2;
	      }
	      break;
	    case 0:
	    case 5: // Diminue volume sons
	    case 6:
	      Pref.Volume-=SDL_MIX_MAXVOLUME/10.0;
	      if(Pref.Volume<0) Pref.Volume=0;
	      Sons.DoVolume();
	      Sons.Play(sLive);
	      break;
	    case 1:
	    case 7: // Diminue volume music
	    case 8:
	      Pref.VolumeM-=SDL_MIX_MAXVOLUME/10.0;
	      if(Pref.VolumeM<0) Pref.VolumeM=0;
	      Sons.DoVolume();
	      break;
	    }
	    break;
	  case SDLK_RIGHT:
	    switch(PyE) {
	    case 2:
	      if(Pref.FullScreen==true) {
		Pref.FullScreen=false;
		ChangeVideo();
		InitMain_Options();
		PyE=2;
	      }
	      break;
	    case 0:
	    case 5:
	    case 6:
	      Pref.Volume+=SDL_MIX_MAXVOLUME/10.0;
	      if(Pref.Volume>SDL_MIX_MAXVOLUME) Pref.Volume=SDL_MIX_MAXVOLUME;
	      Sons.DoVolume();
	      Sons.Play(sLive);
	      break;
	    case 1:
	    case 7:
       	    case 8:
	      Pref.VolumeM+=SDL_MIX_MAXVOLUME/10.0;
	      if(Pref.VolumeM>SDL_MIX_MAXVOLUME) Pref.VolumeM=SDL_MIX_MAXVOLUME;
	      Sons.DoVolume();
	      break;
	    }
	    break;
	  case SDLK_UP:
	    PyE--;
	    if(PyE<0) PyE=4;
	    break;
	  case SDLK_DOWN:
	    PyE++;
	    if(PyE>=5) PyE=0;
	    break;
	  case SDLK_F12: // Sauve un screenshot
	    SDL_SaveBMP(sdlVideo,"screenshot.bmp");
	    break;    
	  case ' ':
	  case SDLK_RETURN:
	  case SDLK_KP_ENTER:
	    switch(PyE) {
	    case 0:
	    case 1:
	      break;
	    case 2: // Type d'affichage
	      Pref.FullScreen=(Pref.FullScreen+1)&1;
	      ChangeVideo();
	      InitMain_Options();
	      PyE=2;
	      break;
	    case 3: // Choix de la langue
	      SDLMain_Langue();
	      InitMain_Options();
	      PyE=3;
	      break;
	    case 5: // Diminue volume sons
	      Pref.Volume-=SDL_MIX_MAXVOLUME/10.0;
	      if(Pref.Volume<0) Pref.Volume=0;
	      Sons.DoVolume();
	      Sons.Play(sLive);
	      break;
	    case 6:
	      Pref.Volume+=SDL_MIX_MAXVOLUME/10.0;
	      if(Pref.Volume>SDL_MIX_MAXVOLUME) Pref.Volume=SDL_MIX_MAXVOLUME;
	      Sons.DoVolume();
	      Sons.Play(sLive);
	      break;
	    case 7: // Diminue volume music
	      Pref.VolumeM-=SDL_MIX_MAXVOLUME/10.0;
	      if(Pref.VolumeM<0) Pref.VolumeM=0;
	      Sons.DoVolume();
	      break;
	    case 8:
	      Pref.VolumeM+=SDL_MIX_MAXVOLUME/10.0;
	      if(Pref.VolumeM>SDL_MIX_MAXVOLUME) Pref.VolumeM=SDL_MIX_MAXVOLUME;
	      Sons.DoVolume();
	      break;
	    default:
	      return mMenu;
	    }
	  }
	}
	break;
      case SDL_QUIT:
	return mQuit;
	break;
      }
    }
    
    // Gère les variables
    HorlogeAvant=Horloge;
    Horloge=SDL_GetTicks();
    Sleeping();
    
    // Gère l'Affichage
    Ec[NumEc].Efface(fmenu);

    if(Pref.FullScreen) {
      Ec[NumEc].Affiche(fleches,1,350,300);
      Ec[NumEc].Affiche(fleches,3,450,300);
    }
    else {
      Ec[NumEc].Affiche(fleches,0,350,300);
      Ec[NumEc].Affiche(fleches,4,450,300);
    }

    NumSp=(Horloge/30)%25;
    Ec[NumEc].Affiche(bruitage,NumSp,150,110);
    NumSp=(Horloge/30)%25;
    Ec[NumEc].Affiche(music,NumSp,150,200);
    NumSp=(Horloge/50)%50;
    Ec[NumEc].Affiche(monde,NumSp,180,400);

    N=(int)(Pref.Volume*10+1)/SDL_MIX_MAXVOLUME;
    NumSp=(Horloge/50)%40+120;
    for(i=0;i<N;i++) {
      if(i==N-1) Ec[NumEc].Affiche(locomotive,NumSp,(690-300)/10*i+300,110);
      else Ec[NumEc].Affiche(buches,NumSp,(690-300)/10*i+300,110);
    }

    N=(int)(Pref.VolumeM*10+1)/SDL_MIX_MAXVOLUME;
    for(i=0;i<N;i++) {
      if(i==N-1) Ec[NumEc].Affiche(locomotive,NumSp,(690-300)/10*i+300,200);
      else Ec[NumEc].Affiche(buches,NumSp,(690-300)/10*i+300,200);
    }

    switch(PyE) {
    case 3:
      Affiche_Main(180);
      break;
    case 4:
      Affiche_Main(CentreM);
      break;
    case 5:
      PyE=0;
      Affiche_Main();
      PyE=5;
      break;
    case 6:
      PyE=0;
      Affiche_Main();
      PyE=6;
      break;
    case 7:
      PyE=1;
      Affiche_Main();
      PyE=7;
      break;
    case 8:
      PyE=1;
      Affiche_Main();
      PyE=8;
      break;
    default:
      Affiche_Main();
    }
    
    Sourie.Affiche(NumEc);
    
    // Echange les buffets video
    SDL_Flip(sdlVideo);
    NumEc=(NumEc+1)&1;
    
  } while(true);
  
  return mQuit;
}

/*** SDL Main Menu Choix de la difficulté ***/
/********************************************/
eMenu Menu::SDLMain_Speed(void)
{
  // Initialisations Divers
  Sourie.Init(Menu_Py); // Initialise la sourie
  PyE=1;
  
  // Prend l'image du fond et fait l'affichage 
  Sprites[fond_menu].Affiche(400,300,0,Sprites[fmenu].Image[0]);
  Sprites[menu].Affiche(400,340,0,Sprites[fmenu].Image[0]);
  Sprites[titre].Affiche(400,65,0,Sprites[fmenu].Image[0]);

  AfficheText(400,225,T_easy,Sprites[fmenu].Image[0]);
  AddBouton(0,T_easy,400,225);
  AfficheText(400,340,T_normal,Sprites[fmenu].Image[0]);
  AddBouton(1,T_normal,400,340);
  AfficheText(400,455,T_hard,Sprites[fmenu].Image[0]);
  AddBouton(2,T_hard,400,455);
  Menu_Py[3].DepX=-1;

  // Efface le fond
  NumEc=0;
  Ec[1].Cls(fmenu);
  SDL_Flip(sdlVideo);
  Ec[0].Cls(fmenu);
  
  // Prend les evenements
  do {
    SDL_Event event;
    while(SDL_PollEvent(&event)) {
      Sourie.GetEvent(event,PyE); // Prend les evenements de la sourie
      switch(event.type) {
      case SDL_ACTIVEEVENT:
	if(event.active.gain==1) {
	  NumEc=0;
	  Ec[1].Cls(fmenu);
	  SDL_Flip(sdlVideo);
	  Ec[0].Cls(fmenu);
	}
	break;
      case SDL_KEYDOWN:
	if(event.key.state==SDL_PRESSED) {
	  Sons.Play(sClic);
	  switch(event.key.keysym.sym) {
	  case SDLK_ESCAPE:
	    return mMenu;
	  case SDLK_UP:
	    PyE--;
	    if(PyE<0) PyE=2;
	    break;
	  case SDLK_DOWN:
	    PyE++;
	    if(PyE>2) PyE=0;
	    break;
	  case SDLK_F12: // Sauve un screenshot
	    SDL_SaveBMP(sdlVideo,"screenshot.bmp");
	    break;    
	  case ' ':
	  case SDLK_RETURN:
	  case SDLK_KP_ENTER:
	    switch(PyE) {
	    case 0:
	      Pref.Difficulte=Easy;
	      return mJeux;
	    case 1:
	      Pref.Difficulte=Normal;
	      return mJeux;
	    case 2:
	      Pref.Difficulte=Hard;
	      return mJeux;
	    }
	    break;
	  }
	}
	break;
      case SDL_QUIT:
	return mQuit;
	break;
      }
    }
    
    // Gère les variables
    HorlogeAvant=Horloge;
    Horloge=SDL_GetTicks();
    Sleeping();
    
    // Gère l'Affichage
    Ec[NumEc].Efface(fmenu);
    Affiche_Main();
    Sourie.Affiche(NumEc);
    
    // Echange les buffets video
    SDL_Flip(sdlVideo);
    NumEc=(NumEc+1)&1;
    
  } while(true);
  
  return mQuit;
}

/*** SDL Main Menu Choix du niveau ***/
/*************************************/
eMenu Menu::SDLMain_Niveau(void)
{
  // Initialisations Divers
  Sourie.Init(Menu_Py); // Initialise la sourie
  PyE=0;
  Niv=Pref.NiveauMax;
  Pref.Niveau=0;

  if(Niv==0) return mMenuSpeed; // Inutile si la première fois

  // Prend l'image du fond et fait l'affichage 
  Sprites[fond_menu].Affiche(400,300,0,Sprites[fmenu].Image[0]);
  Sprites[menu].Affiche(400,340,0,Sprites[fmenu].Image[0]);
  Sprites[titre].Affiche(400,65,0,Sprites[fmenu].Image[0]);

  AfficheText(400,225,T_new_game,Sprites[fmenu].Image[0]);
  AddBouton(0,T_new_game,400,225);
  AfficheText(400,320,T_old_level,Sprites[fmenu].Image[0]);
  AddBouton(1,T_old_level,400,320);
  AfficheText(400,455,T_menu,Sprites[fmenu].Image[0]);
  AddBouton(2,T_menu,400,455);
  
  AddBouton(3,fleches,330,380);
  AddBouton(4,fleches,470,380);

  Menu_Py[5].DepX=-1;
  
  // Efface le fond
  NumEc=0;
  Ec[1].Cls(fmenu);
  SDL_Flip(sdlVideo);
  Ec[0].Cls(fmenu);
  
  // Prend les evenements
  do {
    SDL_Event event;
    while(SDL_PollEvent(&event)) {
      Sourie.GetEvent(event,PyE); // Prend les evenements de la sourie
      switch(event.type) {
      case SDL_ACTIVEEVENT:
	if(event.active.gain==1) {
	  NumEc=0;
	  Ec[1].Cls(fmenu);
	  SDL_Flip(sdlVideo);
	  Ec[0].Cls(fmenu);
	}
	break;
      case SDL_KEYDOWN:
	if(event.key.state==SDL_PRESSED) {
	  Sons.Play(sClic);
	  switch(event.key.keysym.sym) {
	  case SDLK_ESCAPE:
	    return mMenu;
	  case SDLK_UP:
	    PyE--;
	    if(PyE<0 || PyE>2) PyE=2;
	    break;
	  case SDLK_DOWN:
	    PyE++;
	    if(PyE>2) PyE=0;
	    break;
	  case SDLK_LEFT:
	    if(Niv>0) Niv--;
	    PyE=1;
	    break;
	  case SDLK_RIGHT:
	    if(Niv<Pref.NiveauMax) Niv++;
	    PyE=1;
	    break;
	  case SDLK_F12: // Sauve un screenshot
	    SDL_SaveBMP(sdlVideo,"screenshot.bmp");
	    break;    
	  case ' ':
	  case SDLK_RETURN:
	  case SDLK_KP_ENTER:
	    switch(PyE) {
	    case 0:
	      return mMenuSpeed;
	    case 1:
	      Pref.Niveau=Niv;
	      return mMenuSpeed;
	    case 2:
	      return mMenu;
	    case 3:
	      if(Niv>0) Niv--;
	      break;
	    case 4:
	      if(Niv<Pref.NiveauMax) Niv++;
	      break;
	    }
	    break;
	  }
	}
	break;
      case SDL_QUIT:
	return mQuit;
	break;
      }
    }
    
    // Gère les variables
    HorlogeAvant=Horloge;
    Horloge=SDL_GetTicks();
    Sleeping();
    
    // Gère l'Affichage
    Ec[NumEc].Efface(fmenu);
    EffaceChiffre(400,380,999,Sprites[fmenu].Image[0]);

    // Affiche les flèches
    if(Niv>0) {
      if(PyE==3) Ec[NumEc].Affiche(fleches,2,330,380);
      else Ec[NumEc].Affiche(fleches,1,330,380);
    }
    else Ec[NumEc].Affiche(fleches,0,330,380);
    
    if(Niv<Pref.NiveauMax) {
      if(PyE==4) Ec[NumEc].Affiche(fleches,5,470,380);
      else Ec[NumEc].Affiche(fleches,4,470,380);
    }
    else Ec[NumEc].Affiche(fleches,3,470,380);
    
    AfficheChiffre(400,380,Niv+1);
    
    if(PyE!=3 && PyE!=4) Affiche_Main();
    Sourie.Affiche(NumEc);
    
    // Echange les buffets video
    SDL_Flip(sdlVideo);
    NumEc=(NumEc+1)&1;
    
  } while(true);
  
  return mQuit;
}

/*** SDL Main questions sur les droits de l'homme ***/
/****************************************************/
#ifndef DCHILDREN
eMenu Menu::SDLMain_HR(void)
{
  int Fini=-1;
  int N1,N2,Ordre;
  SDL_Rect Position;

  // InitialisationsDivers
  Sourie.Init(Menu_Py); // Initialise la sourie
  PyE=rand()%2;
  
  // Choix de la question N1=reponse
  N1=rand()%30;
  do {
    N2=rand()%30;
  } while (N2==N1);
  Ordre=rand()%2; // Si 0=bonne ordre, 1=inverse

  // Prend l'image du fond et fait l'affichage
  Position.x=Position.y=0;
  Position.w=Sprites[fmenu].Dim[0].L;
  Position.h=Sprites[fmenu].Dim[0].H;
  SDL_BlitSurface(sdlVideo,&Position,Sprites[fmenu].Image[0],&Position);
  
  Sprites[menu].Affiche(340,300,0,Sprites[fmenu].Image[0]);
  Sprites[fond_hr].Affiche(340,74,0,Sprites[fmenu].Image[0]);
  AfficheText(338,70,e_Sprite(T_question),Sprites[fmenu].Image[0]);

  Sprites[locomotive].Affiche(115,110,rand()%320,Sprites[fmenu].Image[0]);
  Sprites[deco].Affiche(100,160+(rand()%130),rand()%18,Sprites[fmenu].Image[0]);
  Sprites[deco].Affiche(100,470-(rand()%130),rand()%18,Sprites[fmenu].Image[0]);
  Sprites[deco].Affiche(580,100+(rand()%130),rand()%18,Sprites[fmenu].Image[0]);
  Sprites[deco].Affiche(580,470-(rand()%130),rand()%18,Sprites[fmenu].Image[0]);

  AfficheText(340,300,e_Sprite(T_art1+N1),Sprites[fmenu].Image[0]);

  if(Ordre) {
    AddBouton(0,e_Sprite(T_tart1+N1),240,492);
    AddBouton(1,e_Sprite(T_tart1+N2),440,492);
  }
  else {
    AddBouton(0,e_Sprite(T_tart1+N1),440,492);
    AddBouton(1,e_Sprite(T_tart1+N2),240,492);
  }
  Menu_Py[0].DepY-=20;
  Menu_Py[0].FinY+=20;
  Menu_Py[1].DepY-=20;
  Menu_Py[1].FinY+=20;

  Menu_Py[2].DepX=-1;

  // Efface le fond
  NumEc=0;
  Ec[1].Cls(fmenu);
  SDL_Flip(sdlVideo);
  Ec[0].Cls(fmenu);
  
  // Prend les evenements
  do {
    SDL_Event event;
    while(SDL_PollEvent(&event)) {
      Sourie.GetEvent(event,PyE); // Prend les evenements de la sourie
      switch(event.type) {
      case SDL_ACTIVEEVENT:
	if(event.active.gain==1) {
	  NumEc=0;
	  Ec[1].Cls(fmenu);
	  SDL_Flip(sdlVideo);
	  Ec[0].Cls(fmenu);
	}
	break;
      case SDL_KEYDOWN:
	if(Fini==-1 && event.key.state==SDL_PRESSED) {
	  Sons.Play(sClic);
	  switch(event.key.keysym.sym) {
	  case SDLK_ESCAPE:
	    return mJeux;
	  case SDLK_UP:
	    PyE--;
	    if(PyE<0) PyE=1;
	    break;
	  case SDLK_DOWN:
	    PyE++;
	    if(PyE>1) PyE=0;
	    break;
	  case SDLK_LEFT:
	    if(Ordre) PyE=0;
	    else PyE=1;
	    break;
	  case SDLK_RIGHT:
	    if(Ordre) PyE=1;
	    else PyE=0;
	    break;
	  case SDLK_F12: // Sauve un screenshot
	    SDL_SaveBMP(sdlVideo,"screenshot.bmp");
	    break;    
	  case ' ':
	  case SDLK_RETURN:
	  case SDLK_KP_ENTER:
	    switch(PyE) {
	    case 0:
	      Sons.Play(sEnd);
	      Pref.Score+=50;
	      Fini=Horloge+2000;
	      break;
	    case 1:
	      Sons.Play(sLose);
	      Fini=Horloge+2000;
	      break;
	    }
	    break;
	  }
	}
	break;
      case SDL_QUIT:
	return mQuit;
	break;
      }
    }

    // Test si fini
    if(Fini!=-1 && Fini<Horloge) return mJeux;
    
    // Gère les variables
    HorlogeAvant=Horloge;
    Horloge=SDL_GetTicks();
    Sleeping();
    
    // Gère l'Affichage
    Ec[NumEc].Efface(fmenu);
  
    if(Ordre) {
      Ec[NumEc].Affiche(fond_hrr,0,240,492);
      AfficheText(240,492,e_Sprite(T_tart1+N1));
    }
    else {
      Ec[NumEc].Affiche(fond_hrr,0,440,492);
      AfficheText(440,492,e_Sprite(T_tart1+N1));
    }
    
    if(Fini==-1) {
      if(Ordre) {
	Ec[NumEc].Affiche(fond_hrr,0,440,492);
	AfficheText(440,492,e_Sprite(T_tart1+N2));
	if(PyE==0) Affiche_Main(240);
	else Affiche_Main(440);
      }
      else {
	Ec[NumEc].Affiche(fond_hrr,0,240,492);
	AfficheText(240,492,e_Sprite(T_tart1+N2));
	if(PyE==1) Affiche_Main(240);
	else Affiche_Main(440);
      }      
      Sourie.Affiche(NumEc);
    }
    
    // Echange les buffets video
    SDL_Flip(sdlVideo);
    NumEc=(NumEc+1)&1;
    
  } while(true);

  return mQuit;
}
#endif

/*** SDL Main dans la partie ***/
/*******************************/
void Menu::Affiche_InGame(void)
{
  SDL_Rect Position;
  
  // InitialisationsDivers
  Sourie.Init(Menu_Py); // Initialise la sourie
  PyE=0;
  
  // Prend l'image du fond et fait l'affichage
  Position.x=Position.y=0;
  Position.w=Sprites[fmenu].Dim[0].L;
  Position.h=Sprites[fmenu].Dim[0].H;
  SDL_BlitSurface(sdlVideo,&Position,Sprites[fmenu].Image[0],&Position);
  
  Sprites[menu].Affiche(340,300,0,Sprites[fmenu].Image[0]);

  AfficheText(340,185,T_continue,Sprites[fmenu].Image[0]);
  AddBouton(0,T_continue,340,185);
  AfficheText(340,300,T_moptions,Sprites[fmenu].Image[0]);
  AddBouton(1,T_moptions,340,300);
  AfficheText(340,415,T_exit_game,Sprites[fmenu].Image[0]);
  AddBouton(2,T_exit_game,340,415);
  Menu_Py[3].DepX=-1;

  // Efface le fond
  NumEc=0;
  Ec[1].Cls(fmenu);
  SDL_Flip(sdlVideo);
  Ec[0].Cls(fmenu);
}

/*** Gestion du menu dans le jeu ***/
/***********************************/
eMenu Menu::SDLMain_InGame(void)
{
  Affiche_InGame();
  
  // Prend les evenements
  do {
    SDL_Event event;
    while(SDL_PollEvent(&event)) {
      Sourie.GetEvent(event,PyE); // Prend les evenements de la sourie
      switch(event.type) {
      case SDL_ACTIVEEVENT:
	if(event.active.gain==1) {
	  NumEc=0;
	  Ec[1].Cls(fmenu);
	  SDL_Flip(sdlVideo);
	  Ec[0].Cls(fmenu);
	}
	break;
      case SDL_KEYDOWN:
	if(event.key.state==SDL_PRESSED) {
	  Sons.Play(sClic);
	  switch(event.key.keysym.sym) {
	  case SDLK_ESCAPE:
	    return mJeux;
	  case SDLK_UP:
	    PyE--;
	    if(PyE<0) PyE=2;
	    break;
	  case SDLK_DOWN:
	    PyE++;
	    if(PyE>2) PyE=0;
	    break;
	  case SDLK_F12: // Sauve un screenshot
	    SDL_SaveBMP(sdlVideo,"screenshot.bmp");
	    break;    
	  case ' ':
	  case SDLK_RETURN:
	  case SDLK_KP_ENTER:
	    switch(PyE) {
	    case 0:
	      return mJeux;
	    case 1:
	      SDLMain_Options();
	      Affiche_InGame();
	      PyE=1;
	      break;
	    case 2:
	      return mScoreEdit;
	    }
	    break;
	  }
	}
	break;
      case SDL_QUIT:
	return mQuit;
	break;
      }
    }
    
    // Gère les variables
    HorlogeAvant=Horloge;
    Horloge=SDL_GetTicks();
    Sleeping();
    
    // Gère l'Affichage
    Ec[NumEc].Efface(fmenu);
    Affiche_Main(340);
    Sourie.Affiche(NumEc);
    
    // Echange les buffets video
    SDL_Flip(sdlVideo);
    NumEc=(NumEc+1)&1;
    
  } while(true);
  
  return mQuit;
}

/*** SDL Main Menu Choix de la difficulté ***/
/********************************************/
eMenu Menu::SDLMain_Score(bool EditScore)
{
  int i;
  int NEdit=-1;
  char Provi[256];
  int PosCur=0;
  char key;

  // Cherche le numéro du score à remplacer si edition des scores
  if(EditScore) {
    for(i=7;i>=0;i--) if(Pref.Sco[i].Score<Pref.Score) NEdit=i;
    if(NEdit==-1) return mMenu;

    if(NEdit<7) // Si doit fair un décalage
      for(i=7;i>NEdit;i--) {
	Pref.Sco[i].Score=Pref.Sco[i-1].Score;
	strcpy(Pref.Sco[i].Name,Pref.Sco[i-1].Name);
      }

    // Efface le nouveau nom et met le score
    Pref.Sco[NEdit].Score=Pref.Score;
    Pref.Sco[NEdit].Name[0]=0;
  }
  
  // Met la sourie sur tous l'ecran
  Sourie.Init(Menu_Py); // Initialise la sourie
  Menu_Py[0].DepX=0;
  Menu_Py[0].DepY=0;
  Menu_Py[0].FinX=800;
  Menu_Py[0].FinY=600;
  Menu_Py[0].Py=0;
  Menu_Py[0].Valide=true;
  Menu_Py[1].DepX=-1;
  
  // Prend l'image du fond et fait l'affichage 
  Sprites[fond_menu].Affiche(400,300,0,Sprites[fmenu].Image[0]);
  
  // Affiche le titre et les commandes
  AfficheText(400,50,T_better_scores,Sprites[fmenu].Image[0]);
  AfficheText(400,550,T_press_any_key,Sprites[fmenu].Image[0]);

  // Affiche les scores
  for(i=0;i<8;i++) {
    sprintf(Provi,"%u",i+1);
    AfficheString(70,120+i*(360/7),Provi,Sprites[fmenu].Image[0]);

    if(EditScore==false || NEdit!=i) {
      if(Pref.Sco[i].Name[0]) AfficheString(140,120+i*(360/7),Pref.Sco[i].Name,Sprites[fmenu].Image[0]);
      else AfficheString(140,120+i*(360/7),Points,Sprites[fmenu].Image[0]);
    }
    
    sprintf(Provi,"%i",Pref.Sco[i].Score);
    AfficheString(740-LongueurString(Provi),120+i*(360/7),Provi,Sprites[fmenu].Image[0]);
  }
  
  // Efface le fond
  NumEc=0;
  Ec[1].Cls(fmenu);
  SDL_Flip(sdlVideo);
  Ec[0].Cls(fmenu);
  
  // Prend les evenements
  do {
    SDL_Event event;
    while(SDL_PollEvent(&event)) {
      Sourie.GetEvent(event,PyE); // Prend les evenements de la sourie
      switch(event.type) {
      case SDL_ACTIVEEVENT:
	if(event.active.gain==1) {
	  NumEc=0;
	  Ec[1].Cls(fmenu);
	  SDL_Flip(sdlVideo);
	  Ec[0].Cls(fmenu);
	}
	break;
      case SDL_KEYDOWN: // Prend un touche au clavier
	if(event.key.state==SDL_PRESSED) {
	  Sons.Play(sClic);
	  if(EditScore==false && event.key.keysym.sym!=SDLK_F12) event.key.keysym.sym=SDLK_RETURN;
	  switch(event.key.keysym.sym) {
	  case SDLK_F12: // Sauve un screenshot
	    SDL_SaveBMP(sdlVideo,"screenshot.bmp");
	    break;   
	  case SDLK_ESCAPE: // Valide l'entrée
	  case SDLK_RETURN:
	  case SDLK_KP_ENTER:
	    return mMenu;
	  case SDLK_BACKSPACE: // Fait un retour de chariot
	    if(PosCur) {
	      PosCur--;
	      Pref.Sco[NEdit].Name[PosCur]=0;
	    }
	    break;
	  default: // Prend les touches
	    key=event.key.keysym.unicode&0x7F; // Prend le caracataire correspondant à la touche
	    if(PosCur<79 && CharExiste(key)==true) { // Prend le caractaire
	      Pref.Sco[NEdit].Name[PosCur]=key;
	      Pref.Sco[NEdit].Name[PosCur+1]=0;
	      if(LongueurString(Pref.Sco[NEdit].Name)<LSCOREMAX) { // Si longueur encore Ok
		PosCur++;
	      }
	      else {
		Pref.Sco[NEdit].Name[PosCur]=0; // Ne valide pas le caracataire
	      }
	    }
	    break;
	  }
	}
	break;
      case SDL_QUIT:
	return mQuit;
      }
    }

    // Gère les variables
    HorlogeAvant=Horloge;
    Horloge=SDL_GetTicks();
    Sleeping();
    
    // Gère l'Affichage
    Ec[NumEc].Efface(fmenu);

    if(EditScore) { // Gère l'affiche pour l'édition des scores
      Sprites[fmenu].EffaceCarre(140,120-360/14+NEdit*(360/7),140+LSCOREMAX,120-360/14+(NEdit+1)*(360/7),
				 Sprites[fmenu].Image[0]);
      AfficheString(140,120+NEdit*(360/7),Pref.Sco[NEdit].Name);
      
      i=(Horloge/50)%20; // Affiche les curseurs
      Ec[NumEc].Affiche(fleche_gauche,i,110,120+NEdit*(360/7));
      Ec[NumEc].Affiche(fleche_droite,i,180+LongueurString(Pref.Sco[NEdit].Name),120+NEdit*(360/7));

    }
  
    // Echange les buffets video
    SDL_Flip(sdlVideo);
    NumEc=(NumEc+1)&1;
    
  } while(true);
  
  return mQuit;
}

/*** Affiche le menu Principale ***/
/**********************************/
void Menu::Affiche_Main(int Centre)
{
  int NumSp=(Horloge/50)%20;
  int x1=Menu_Py[PyE].DepX-25;
  int x2=(Centre-x1)+Centre;
  int y=(Menu_Py[PyE].FinY+Menu_Py[PyE].DepY)/2;
  
  Ec[NumEc].Affiche(fleche_gauche,NumSp,x1,y);
  Ec[NumEc].Affiche(fleche_droite,NumSp,x2,y);
}

/*** Centre les flèches sur le boutton ***/
/*****************************************/
void Menu::Affiche_Main_Centre()
{
  int NumSp=(Horloge/50)%20;
  int x1=Menu_Py[PyE].DepX-5;
  int x2=Menu_Py[PyE].FinX+5;
  int y=(Menu_Py[PyE].FinY+Menu_Py[PyE].DepY)/2;
  
  Ec[NumEc].Affiche(fleche_gauche,NumSp,x1,y);
  Ec[NumEc].Affiche(fleche_droite,NumSp,x2,y);
}
