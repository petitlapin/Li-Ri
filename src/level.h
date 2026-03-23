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

/*** struct definition for levels ***/
/************************************/
struct s_Deco
{
    int NumSpr; // Number and position of decorative sprite
    int x;
    int y;
};

struct s_Level
{
    unsigned char T[LT * HT]; // Circuit and settings definition
    int StartX; // Locomotive starting point.
    int StartY;
    int StartDir; // Locomotive starting direction
    int NDeco; // Number of decoration elements
    struct s_Deco Deco[32];
};

class Level
{
public:
    Level();
    ~Level() = default;

    bool Load(); // Loads level/maps
    bool Save(); // Save level
    void Del(int Num); // Delete level
    void Ins(int Num); // Insert (empty) Level
    void Clear(int Num); // Empty/Clear level

    int N { 0 }; // Number of levels
    s_Level T[MAX_N_LEVEL_IN_MEMORY]; // Pointer to levels
};

#endif
