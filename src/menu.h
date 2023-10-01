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

#ifndef _MENU_DOM_
#define _MENU_DOM_

#include "preference.h"

/*** Fonction d'attente ***/
/**************************/
void Sleeping(void);

/*** Définition de la class ***/
/******************************/
class Menu {
 public:
  Menu(void);
  ~Menu(void);

  /*** Fonctions ***/
  /*****************/
  eMenu SDLMain(void); // Menu principale
  eMenu SDLMain_Langue(void);  // Menu du choix de la langue
  void InitMain_Options(void);   // Prépare l'affichage du menu options
  eMenu SDLMain_Options(void);  // Menu du choix des options
  eMenu SDLMain_Speed(void);  // Menu du choix de la difficulté
  eMenu SDLMain_Niveau(void); // Choisi le niveau
#ifndef DCHILDREN
  eMenu SDLMain_HR(void);      // Menu de question sur les doits de l'homme
#endif
  void Affiche_InGame(void);  // Affiche le menu InGame
  eMenu SDLMain_InGame(void); // Menu dans le jeu
  eMenu SDLMain_Score(bool EditScore=false);  // Affiche les scores
  
  void Affiche_Main(int Centre=400);      // Affiche le menu principale
  void Affiche_Main_Centre(void);          // Centre les flèches sur le bouton

  /*** Variables ***/
  /*****************/
  int PyE; // Position du curseur dans le menu
  int NumEc; // Numéro de l'ecran en cours d'affichage
  int Niv;
  int CentreM; // Variable pour le menu options
};

#endif
