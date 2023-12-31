//      (_||_/
//      (    )       Level class
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

#ifndef LEVEL_DOM_
#define LEVEL_DOM_

#include "preference.h"

/*** Définition de la structure d'un tableau ***/
/***********************************************/
struct s_Deco
{
    int NumSpr; // Numéro et position du sprite décoratif
    int x;
    int y;
};

struct s_Level
{
    unsigned char T[LT * HT]; // Définition du circuit et des options
    int DepX; // Départ de la locomotive
    int DepY;
    int DepDir; // Direction du départ
    int NDeco; // Nombre d'éléments décoratifs
    struct s_Deco Deco[32];
};

/*** Définition de la classe ***/
/*******************************/
class Level
{
public:
    Level();
    ~Level() = default;

    /*** Fonctions ***/
    bool Load(); // charge les tableaux
    bool Save(); // Sauve les tableaux
    void Del(int Num); // efface un tableau
    void Ins(int Num); // Insert un tableau vièrge
    void Clear(int Num); // Vide un tableau

    /*** Variables ***/
    int N { 0 }; // Nombre de niveau
    s_Level T[MAX_N_LEVEL_IN_MEMORY]; // Pointe sur les tableaux
};

#endif
