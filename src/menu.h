//      (_||_/
//      (    )       Menu class
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

#ifndef MENU_DOM_
#define MENU_DOM_

#include "preference.h"

/*** Fonction d'attente ***/
/**************************/
void Sleeping();

/*** Définition de la class ***/
/******************************/
class Menu
{
public:
    Menu() = default;
    ~Menu() = default;

    /*** Fonctions ***/
    /*****************/
    eMenu SDLMain(); // Menu principale
    eMenu SDLMain_Language(); // Menu du choix de la langue
    void InitMain_Options(); // Prépare l'affichage du menu options
    eMenu SDLMain_Options(); // Menu du choix des options
    eMenu SDLMain_Speed(); // Menu du choix de la difficulté
    eMenu SDLMain_Level(); // Choisi le niveau
#ifndef DCHILDREN
    eMenu SDLMain_HR(); // Menu de question sur les droits de l'homme
#endif
    void Print_InGame(); // Affiche le menu InGame
    eMenu SDLMain_InGame(); // Menu dans le jeu
    eMenu SDLMain_Score(bool EditScore = false); // Affiche les scores

    void Print_Main(int Centre = 400) const; // Affiche le menu principale
    void Affiche_Main_Centre() const; // Centre les flèches sur le bouton

private:
    /*** Variables ***/
    /*****************/
    int PyE { 0 }; // Position du curseur dans le menu
    int Niv { 0 };
    int CentreM { 0 }; // Variable pour le menu options
};

#endif
