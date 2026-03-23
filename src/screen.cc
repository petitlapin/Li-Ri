//      (_||_/
//      (    )
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

#include "preference.h"
#include "sprite.h"
#include "screen.h"

extern Sprite *Sprites;

/*** Display a sprite ***/
/************************/
void Screen::PrintSprite(e_Sprite NumSpr, int Num, int x, int y)
{
    Sprites[NumSpr].Draw(x, y, Num);
}

void Screen::PrintCable(int dx, int dy, int fx, int fy)
{
    // Show the rope between the wagons
    Sprites[rope].PrintRope(dx, dy, fx, fy);
}

/*** Display a text ***/
/**********************/
void Screen::PrintText(e_Sprite Text, int x, int y)
{
    Sprites[Text].Draw(x, y, 0);
}

/*** Display game settings ***/
/*****************************/
void Screen::PrintOptions(int Nlives, int NScore)
{
    int x, y;

    Score = NScore;
    DrawNumber(740, 215, Score);

    if (Nlives > 10) {
        Nlives = 10; // Clamp to avoid going off screen
    }
    for (int i = 0; i < Nlives; ++i) { // Display lives
        x = i % 2;
        x = x * 44 + 38 + LT * D_Case;
        y = i / 2;
        y = 384 + y * 46;
        Sprites[logo_health].Draw(x, y, 0);
    }
}

/*** Erase display with background image ***/
/*******************************************/
void Screen::CleanSpriteAndScreen(e_Sprite NumSp)
{
    Sprites[NumSp].Draw(Sprites[NumSp].Dim[0].L / 2, Sprites[NumSp].Dim[0].H / 2, 0);
    Score = -1;
}
