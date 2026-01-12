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

/***   Table/level structure define  ***/
/***************************************/
struct s_Deco
{
    int NumSpr; // Number and position of the decorative sprite
    int x;
    int y;
};

struct s_Level
{
    unsigned char T[LT * HT]; // Circuit definition and options
    int DepX; // Departure of the locomotive
    int DepY;
    int DepDir; // Direction
    int NDeco; // Number of decorative elements
    struct s_Deco Deco[32];
};

/*** Table/level class define ***/
/********************************/
class Level
{
public:
    Level();
    ~Level() = default;

    /*** Functions ***/
    bool Load(); // Load the table
    bool Save(); // Save the table
    void Del(int Num); // Clear the table
    void Ins(int Num); // Insert a Table
    void Clear(int Num); // Clear a table

    /*** Variables ***/
    int N { 0 }; // Number of levels
    s_Level T[MAX_N_LEVEL_IN_MEMORY]; // Table pointer
};

#endif
