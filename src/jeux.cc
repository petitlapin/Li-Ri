//      (_||_/
//      (    )       Classe Jeux
//     ( o  0 )
//-OOO°--(_)---°OOO---------------------------------------
//                   Copyright (C) 2006 By Dominique Roux-Serret
// .OOOo      oOOO.  roux-serret@ifrance.com
//-(   )------(   )---------------------------------------
//  ( (        ) /   Le 01/02/2006
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

#ifdef WINDOWS
#include <windows.h>
#endif

#include <iostream.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <SDL/SDL.h>

#include "jeux.h"
#include "ecran.h"
#include "menu.h"
#include "tableau.h"
#include "audio.h"

/*** Variables globales ***/
/**************************/
extern SDL_Surface *sdlVideo;

extern Sprite *Sprites;
extern sPreference Pref;

extern int Horloge;
extern int HorlogeAvant;

extern Ecran Ec[2];
extern Menu MenuPrincipale;

extern Tableau Niveau;
extern Audio Sons;

static int NumRail[]={ -1,-1,-1,0,-1,1,2,3,-1,4,5,6,7,8,9,10 };

int MasqueK; // Masque pour les touches de déplacement

/*** Constructeur et Destructeur ***/
/***********************************/
Jeux::Jeux()
{
  NumSS=0;
  NumVideo=0;
  Touche[0]=D_Haut;
  Touche[1]=D_Bas;
  Touche[2]=D_Gauche;
  Touche[3]=D_Droite;
}

Jeux::~Jeux()
{ }

/*** SDL Main ***/
/****************/
eMenu Jeux::SDLMain(void)
{
  eMenu mRet;
  int NumN=Pref.Niveau;
  
  Help=true;
  Load(NumN); // Charge le tableau
  Ec[NumVideo].Cls(fjeu);
  SDL_Flip(sdlVideo);
  NumVideo=(NumVideo+1)&1;
  Ec[NumVideo].Cls(fjeu);
  Pause=true;

  Horloge=SDL_GetTicks(); // Prend l'horloge
  DureeJeu=0;
  Key=0;
    
  // Met le options de départ du joueur
  Pref.NVie=N_VIES_DEP;
  Pref.Score=0;
  
  // Prend les evenements
  do {
    SDL_Event event;
    while(SDL_PollEvent(&event)) {
      switch(event.type) {
      case SDL_ACTIVEEVENT:
	if(event.active.gain==1) {
	  Ec[NumVideo].Cls(fjeu);
	  SDL_Flip(sdlVideo);
	  NumVideo=(NumVideo+1)&1;
	  Ec[NumVideo].Cls(fjeu);
	}
	else { // Si désactive l'ecran
	  Pause=true; // Met en Pause
	}
	break;
      case SDL_MOUSEBUTTONDOWN:
	if(event.button.state==SDL_PRESSED) TourneFleche();
	break;
      case SDL_KEYDOWN:
	if(event.key.state==SDL_PRESSED) {
	  if(event.key.keysym.sym==SDLK_ESCAPE) {
	    mRet=MenuPrincipale.SDLMain_InGame();
	    if(mRet==mJeux) {
	      DrawLevel(NumN);
	      Ec[NumVideo].Cls(fjeu);
	      SDL_Flip(sdlVideo);
	      NumVideo=(NumVideo+1)&1;
	      Ec[NumVideo].Cls(fjeu);
	      Pause=true;
	    }
	    else return mRet;
	  }
	  else {
	    Key=event.key.keysym.sym;
	    switch(Key) {
	    case SDLK_UP:
	      MasqueK|=1;
	      break;
	    case SDLK_DOWN:
	      MasqueK|=2;
	      break;
	    case SDLK_LEFT:
	      MasqueK|=4;
	      break;
	    case SDLK_RIGHT:
	      MasqueK|=8;
	      break;
	    }
	  }
	}
	break;
      case SDL_KEYUP:
	switch(event.key.keysym.sym) {
	case SDLK_UP:
	  MasqueK&=14;
	  break;
	case SDLK_DOWN:
	  MasqueK&=13;
	  break;
	case SDLK_LEFT:
	  MasqueK&=11;
	  break;
	case SDLK_RIGHT:
	  MasqueK&=7;
	  break;
	}
	if(!MasqueK) Key=0;
	break;
      case SDL_QUIT:
	return mQuit;
	break;
      }
    }
    
    // Gère l'appui des touches
    if(Key && Lo.Mort==-1) PrendTouche(Key);

    // Gère les Horloges et la pose
    HorlogeAvant=Horloge;
    Horloge=SDL_GetTicks();
    Sleeping();
    if(Pause==true || Lo.Mort>Horloge) HorlogeAvant=Horloge;
    DureeJeu+=Horloge-HorlogeAvant;
    
    // Fait avancer la loco
    if(Lo.Mort==-1 && Pause==false) Lo.Avance(Horloge-HorlogeAvant,DureeJeu,Touche,T);
    
    // Fait l'affichage
    AfficheEcran();
    SDL_Flip(sdlVideo);

    NumVideo=(NumVideo+1)&1;    

    // Test la fin d'une partie
    if(Lo.Mort>-1 && Lo.Mort<Horloge) { // Si est Mort test si doit continuer ou quitter
      if(Pref.NVie<0) return mScoreEdit; // Si mort fini
      if(Lo.Gagne) {
#ifndef DCHILDREN
	if(MenuPrincipale.SDLMain_HR()==mQuit) return mQuit;
#endif
	NumN++;
	if(Niveau.N==NumN) {
	  Pref.Score+=Pref.NVie*100;
	  return mScoreEdit;
	}
	if(Pref.NiveauMax<NumN) Pref.NiveauMax=NumN;
      }
      Sons.NextMusic();
      Load(NumN);              // Recharge le tableau
      Ec[NumVideo].Cls(fjeu);
      SDL_Flip(sdlVideo);
      NumVideo=(NumVideo+1)&1;
      Ec[NumVideo].Cls(fjeu);
      DureeJeu=0;
      Pause=true;
      Key=MasqueK=0;
    }    
    
  } while(true);
  
  return mQuit;
}

/*** Charge un tableau ***/
/*************************/
bool Jeux::Load(int NivN)
{
  int i;
  
  Pref.Niveau=NivN;
    
  // Recopie le tableau
  for(i=0;i<LT*HT;i++) T[i]=(int)Niveau.T[NivN].T[i];
  
  // Laisse ou efface la vie suivant le niveau
  switch(Pref.Difficulte) {
  case Easy:
    i=5;
    break;
  case Normal:
    i=9;
    break;
  default:
    i=15;
  }
  if( (rand()%i) !=0 ) {
    for(i=0;i<LT*HT;i++) if(T[i]==C_Live) T[i]=C_Rail;
  }

  // Initialise la locomotive
  Lo.Init(Niveau.T[NivN].DepX+Niveau.T[NivN].DepY*LT,Niveau.T[NivN].DepDir);
  BufTouche(Niveau.T[NivN].DepDir);
  MasqueK=0;

  // Met la vitesse suivant difficulté
  switch(Pref.Difficulte) {
  case Easy:
    Pref.Vitesse=Pref.VitesseMoy=VITESSE_MIN;
    break;
  case Hard:
    Pref.Vitesse=Pref.VitesseMoy=VITESSE_MAX;
    break;
  default:
    Pref.Vitesse=Pref.VitesseMoy=VITESSE_MOY;
  }

  return DrawLevel(NivN);
}

/*** Dessine le fond de l'ecran de jeu ***/
/*****************************************/
bool Jeux::DrawLevel(int NivN)
{
  int i,x,y,m,cx,cy;

  // Fabrique le fond du jeu
  Sprites[fond].Affiche(400,300,0,Sprites[fjeu].Image[0]);

  // Affiche le circuit
  for(i=0;i<LT*HT;i++) {
    if(T[i]>=C_Rail && T[i]<C_Fin) {
      y=i/LT*D_Case+D_Case/2;
      x=i%LT*D_Case+D_Case/2;

      m=0;
      cx=i%LT;
      cy=i/LT;
      if(cy>0 && T[i-LT]>=C_Rail && T[i-LT]<C_Fin) m+=8;
      if(cy<HT-1 && T[i+LT]>=C_Rail && T[i+LT]<C_Fin) m+=4;
      if(cx>0 && T[i-1]>=C_Rail && T[i-1]<=C_Fin) m+=2;
      if(cx<LT-1 && T[i+1]>=C_Rail && T[i+1]<=C_Fin) m+=1;

      Sprites[rail].Affiche(x,y,NumRail[m],Sprites[fjeu].Image[0]);
    }
  }

  // Affiche les décorations
#ifndef DCHILDREN
  for(i=0;i<Niveau.T[NivN].NDeco;i++)
    Sprites[deco].Affiche(Niveau.T[NivN].Deco[i].x,Niveau.T[NivN].Deco[i].y,Niveau.T[NivN].Deco[i].NumSpr,
			  Sprites[fjeu].Image[0]);
#endif
  
  // Affiche les textes suivant la langue
  AfficheText(740,110,T_level,Sprites[fjeu].Image[0]);
  AfficheText(740,180,T_score,Sprites[fjeu].Image[0]);
  AfficheText(740,260,T_options,Sprites[fjeu].Image[0]);
  AfficheText(740,340,T_lives,Sprites[fjeu].Image[0]);

  AfficheChiffre(740,140,Pref.Niveau+1,Sprites[fjeu].Image[0]);
  
  return true;
}

/*** Prend les touches enfoncées ***/
/***********************************/
void Jeux::PrendTouche(int Tou)
{
  char NomSS[40];

  if(Pause==true && (Tou==SDLK_RETURN || Tou>=32) && Tou<SDLK_RALT && 
     (Tou<SDLK_F12 || Tou>SDLK_F15) )
    Tou=(int)(' '); // Press any key!
  
  switch(Tou) {
  case SDLK_UP:
    BufTouche(D_Haut);
    break;
  case SDLK_DOWN:
    BufTouche(D_Bas);
    break;
  case SDLK_LEFT:
    BufTouche(D_Gauche);
    break;
  case SDLK_RIGHT:
    BufTouche(D_Droite);
    break;
  case SDLK_F12: // Sauve un screenshot
    sprintf(NomSS,"screenshot%i.bmp",NumSS);
    NumSS++;
    SDL_SaveBMP(sdlVideo,NomSS);
    break;    
  case 'p':
  case 'P':
    if(Pause==false) Pause=true;
    break;
  case ' ':
    if(Pause) Pause=false;
    break;
  case 'h':
  case 'H':
    if(Help) Help=false;
    else Help=true;
    break;
  }
}

/*** Fait tourner la fleche d'une simple touche ***/
/**************************************************/
void Jeux::TourneFleche(void)
{
  int To=Touche[0];
  bool Cherche=false;
  int x=Lo.PInter%LT;
  int y=Lo.PInter/LT;
  
  do {
    // Fait toucher la direction dans le sens des aiguilles d'une montre
    // et test si la nouvelle direction est possible
    switch(To) {
    case D_Haut:
      To=D_Droite;
      if(Lo.PEntree!=D_Droite && x+1<LT && T[Lo.PInter+1]!=0) Cherche=true;
      break;
    case D_Droite:
      To=D_Bas;
      if(Lo.PEntree!=D_Bas && y+1<HT && T[Lo.PInter+LT]!=0) Cherche=true;
      break;
    case D_Bas:
      To=D_Gauche;
      if(Lo.PEntree!=D_Gauche && x>0 && T[Lo.PInter-1]!=0) Cherche=true;
      break;
    case D_Gauche:
      To=D_Haut;
      if(Lo.PEntree!=D_Haut && y>0 && T[Lo.PInter-LT]!=0) Cherche=true;
      break;
    }
  }
  while(Cherche==false);
  
  BufTouche(To); // Mémorise la nouvelle touche par defaut.    
}
  
/*** Mémorise une touche dans le buffet des touches ***/
/******************************************************/
void Jeux::BufTouche(int Tou)
{
  int n=0;

  // Favorise la touche
  while(Touche[n]!=Tou) n++; // Prend position de la touche

  if(n) { // Si changement doit faire un décalage
    while(n) {
      Touche[n]=Touche[n-1];
      n--;
    }
    Touche[0]=Tou; // Mémorise la touche
  }
 
  // Cherche son oposé
  switch(Tou) {
  case D_Haut:
    Tou=D_Bas;
    break;
  case D_Bas:
    Tou=D_Haut;
    break;
  case D_Gauche:
    Tou=D_Droite;
    break;
  case D_Droite:
    Tou=D_Gauche;
    break;
  }

  // Défavorise son oposé.
  n=3;
  while(Touche[n]!=Tou) n--; // Prend position de la touche
  
  if(n<3) { // Si changement doit faire un décalage
    while(n<3) {
      Touche[n]=Touche[n+1];
      n++;
    }
    Touche[3]=Tou; // Mémorise la touche
  }  
}

/*** Test les directions possibles pour les fleches ***/
/******************************************************/
int Jeux::TestFleche(int Haut,int Bas,int Gauche,int Droite)
{
  int i;
  int x=Lo.PInter%LT;
  int y=Lo.PInter/LT;
  
  for(i=0;i<4;i++) {
    switch(Touche[i]) {
    case D_Haut:
      if(y>0 && Haut!=-1 && T[Lo.PInter-LT]!=0) return Haut;
      break;
    case D_Bas:
      if(y+1<HT && Bas!=-1 && T[Lo.PInter+LT]!=0) return Bas;
      break;
    case D_Gauche:
      if(x>0 && Gauche!=-1 && T[Lo.PInter-1]!=0) return Gauche;
      break;
    case D_Droite:
      if(x+1<LT && Droite!=-1 && T[Lo.PInter+1]!=0) return Droite;
      break;
    }
  }
  return 0;
}

/*** Affiche un ecran du jeu ***/
/*******************************/
void Jeux::AfficheEcran(void)
{
  int i;
  int ndir=0;

  // Prépare pour nouvelle Affichage
  Ec[NumVideo].Efface(fjeu);
  
  // Fait nouvelle Affichage
  Lo.Affiche(Ec[NumVideo]); // Affiche la loco

  if(Lo.PInter!=-1 && Help) { // Affiche la fleche sur la futur intersection
    switch(Lo.PEntree) {
    case D_Gauche:
      ndir=TestFleche(0,2,-1,1);
      break;
    case D_Droite:
      ndir=TestFleche(8,6,7,-1);
      break;
    case D_Haut:
      ndir=TestFleche(-1,4,5,3);
      break;
    case D_Bas:
      ndir=TestFleche(10,-1,9,11);
      break;
    }

    Ec[NumVideo].Affiche(dir,ndir,(Lo.PInter%LT)*D_Case+D_Case/2,(Lo.PInter/LT)*D_Case+D_Case/2);
  }
  
  // Affiche les options
  for(i=0;i<LT*HT;i++) {
    switch(T[i]) {
    case C_Wagon: // Si un loco
      Ec[NumVideo].Affiche(wagon,(DureeJeu*40/1000+i*7)%50,i%LT*D_Case+D_Case/2,i/LT*D_Case+D_Case/2);
      break;
    case C_Allonge: // Si plus long
      Ec[NumVideo].Affiche(pluslong,(DureeJeu*40/1000+i*7)%50,i%LT*D_Case+D_Case/2,i/LT*D_Case+D_Case/2);
      break;
    case C_Reduit: // Si plus court
      Ec[NumVideo].Affiche(pluscourt,(DureeJeu*40/1000+i*7)%50,i%LT*D_Case+D_Case/2,i/LT*D_Case+D_Case/2);
      break;
    case C_Speed: // Si plus vite
      Ec[NumVideo].Affiche(vitesse,(DureeJeu*40/1000+i*7)%50,i%LT*D_Case+D_Case/2,i/LT*D_Case+D_Case/2);
      break;
    case C_Live: // Si une vie
      Ec[NumVideo].Affiche(vie,(DureeJeu*40/1000+i*7)%50,i%LT*D_Case+D_Case/2,i/LT*D_Case+D_Case/2);
      break;
    }
  }

  // Si en pose demande une touche
  if(Pause) Ec[NumVideo].Affiche_Text(T_press_any_key,LT*D_Case/2,300);

  // Affiche tableau de bord
  Ec[NumVideo].AfficheOptions(Pref.NVie,Pref.Score);
  if(Pref.EcartWagon<ECARTWAGON_MOY) Ec[NumVideo].Affiche(pluscourt,(DureeJeu*40/1000)%50,715,295);
  if(Pref.EcartWagon>ECARTWAGON_MOY) Ec[NumVideo].Affiche(pluslong,(DureeJeu*40/1000)%50,715,295);
  if(Pref.VitesseMoy>Pref.Vitesse) Ec[NumVideo].Affiche(vitesse,(DureeJeu*40/1000+7)%50,765,295);
}
