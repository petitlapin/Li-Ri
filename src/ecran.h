//      (_||_/
//      (    )       Classe Ecran (Gère un buffet d'écran)
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

#ifndef _ECRAN_DOM_
#define _ECRAN_DOM_

#include <SDL/SDL.h>
#include "sprite.h"
#include "preference.h"

/*** Définitions générales ***/
/*****************************/
struct s_Ecran {
  e_Sprite NumSpr;
  int Num;
  int x,y;
  int fx,fy;
};

/*** Definitions de la class Ecran ***/
/*************************************/
class Ecran
{
 public:
  Ecran(void);
  ~Ecran(void);

  /*** Fonctions ***/
  void Affiche(e_Sprite NumSpr,int Num,int x,int y); // Affiche un sprite
  void AfficheCable(int dx,int dy,int fx,int fy); // Affiche un cable
  void Affiche_Text(e_Sprite Text,int x,int y);    // Affiche un text à l'ecran
  void AfficheOptions(int NVies,int NouveauScore); // Affiches les options sur le coté
  void Efface(e_Sprite NumSpriteFondEcran); // Efface tous ce qui a été affiché
  void Cls(e_Sprite NumSpriteFondEcran); // Efface l'ecran avec l'image de fond

  /*** Variables ***/
  int N;               // Nombre de sprites mémorisés aprés l'affichage
  int NVie;            // Mémorise le nombre de vies affiché
  int Score;           // Mémorise le score affiché
  s_Ecran B[LT*HT*2];  // N° des sprites à effacer plus tard
};

#endif
