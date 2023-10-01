//      (_||_/
//      (    )       Classe Ecran
//     ( o  0 )
//-OOO°--(_)---°OOO---------------------------------------
//                   Copyright (C) 2006 By Dominique Roux-Serret
// .OOOo      oOOO.  roux-serret@ifrance.com
//-(   )------(   )---------------------------------------
//  ( (        ) /   Le 12/01/2006
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
#include <math.h>
#include "preference.h"
#include "sprite.h"
#include "ecran.h"

/*** Variables globales ***/
/**************************/
extern Sprite *Sprites;
extern sPreference Pref;

/*** Constructeur ***/
/********************/
Ecran::Ecran(void) : N(0), NVie(0), Score(-1)
{ }

Ecran::~Ecran(void)
{ }

/*** Affiche un Sprite ***/
/*************************/
void Ecran::Affiche(e_Sprite NumSpr,int Num,int x,int y)
{
  B[N].NumSpr=NumSpr;
  B[N].Num=Num;
  B[N].x=x;
  B[N].y=y;
  N++;

  Sprites[NumSpr].Affiche(x,y,Num);
}

/*** Affiche un cable ***/
/************************/
void Ecran::AfficheCable(int dx,int dy,int fx,int fy)
{
  // Affiche la corde
  Sprites[corde].AfficheCorde(dx,dy,fx,fy);
  
  // Sauve la position du cable pour l'effacer
  B[N].NumSpr=corde;
  B[N].Num=0;
  B[N].x=dx;
  B[N].y=dy;
  B[N].fx=fx;
  B[N].fy=fy;
  N++;
}

/*** Affiche un text ***/
/***********************/
void Ecran::Affiche_Text(e_Sprite Text,int x,int y)
{
  B[N].NumSpr=Text;
  B[N].Num=0;
  B[N].x=x;
  B[N].y=y;
  N++;

  Sprites[Text].Affiche(x,y,0);
}

/*** Affiche les options du jeu ***/
/**********************************/
void Ecran::AfficheOptions(int NV,int NScore)
{
  int x,y;

  if(NScore!=Score) { // Gère l'affichage des scores
    if(Score!=-1) EffaceChiffre(740,215,Score);
    Score=NScore;
    AfficheChiffre(740,215,Score);
  }

  if(NV>10) NV=10; // Evite un dépassement de l'affichage

  while(NVie>NV) { // Si doit effacer des vies
    x=(NVie-1)%2;
    x=x*44+38+LT*D_Case;
    y=(NVie-1)/2;
    y=384+y*46;
    Sprites[logo_vie].Efface(x,y,0,Sprites[fjeu].Image[0]);
    NVie--;
  }

  while(NVie<NV) { // Si doit afficher des vies
    x=NVie%2;
    x=x*44+38+LT*D_Case;
    y=NVie/2;
    y=384+y*46;
    Sprites[logo_vie].Affiche(x,y,0);
    NVie++;
  }
}

/*** Efface les sprites ***/
/**************************/
void Ecran::Efface(e_Sprite NumSp)
{
  int i;
  
  for(i=0;i<N;i++) {
    if(B[i].NumSpr==corde) Sprites[B[i].NumSpr].EffaceCarre(B[i].x,B[i].y,B[i].fx,B[i].fy,
							    Sprites[NumSp].Image[0]); 
    else Sprites[B[i].NumSpr].Efface(B[i].x,B[i].y,B[i].Num,Sprites[NumSp].Image[0]);
  }
  N=0;
}

/*** Efface l'ecran avec l'image de fond ***/
/*******************************************/
void Ecran::Cls(e_Sprite NumSp)
{
  Sprites[NumSp].Affiche(Sprites[NumSp].Dim[0].L/2,Sprites[NumSp].Dim[0].H/2,0);
  N=0;
  NVie=0;
  Score=-1;
}
