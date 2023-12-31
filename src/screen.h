//      (_||_/
//      (    )       Screen class
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

#ifndef SCREEN_DOM_
#define SCREEN_DOM_

#include "sprite.h"
#include "preference.h"

/*** Définitions générales ***/
/*****************************/
struct s_Screen
{
    e_Sprite NumSpr;
    int Num;
    int x, y;
    int fx, fy;
};

/*** Definitions de la class Ecran ***/
/*************************************/
class Screen
{
public:
    Screen() = default;
    ~Screen() = default;

    /*** Fonctions ***/
    void PrintSprite(e_Sprite NumSpr, int Num, int x, int y); // Affiche un sprite
    void PrintCable(int dx, int dy, int fx, int fy); // Affiche un cable
    void PrintText(e_Sprite Text, int x, int y); // Affiche un text à l'ecran
    void PrintOptions(int NVies, int NScore); // Affiche les options sur le coté
    void ClearSprite(e_Sprite NumSpriteFondEcran); // Efface tous ce qui a été affiché
    void CleanSpriteAndScreen(e_Sprite NumSpriteFondEcran); // Efface l'ecran avec l'image de fond

    /*** Variables ***/
    int N { 0 }; // Nombre de sprites mémorisés aprés l'affichage
    int Score { -1 }; // Mémorise le score affiché
    s_Screen B[LT * HT * 2]; // N° des sprites à effacer plus tard
};

#endif
