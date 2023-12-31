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

/*** Variables globales ***/
/**************************/
extern Sprite *Sprites;

/*** Affiche un Sprite ***/
/*************************/
void Screen::PrintSprite(e_Sprite NumSpr, int Num, int x, int y)
{
    B[N].NumSpr = NumSpr;
    B[N].Num = Num;
    B[N].x = x;
    B[N].y = y;
    N++;

    Sprites[NumSpr].Affiche(x, y, Num);
}

/*** Affiche un cable ***/
/************************/
void Screen::PrintCable(int dx, int dy, int fx, int fy)
{
    // Affiche la corde
    Sprites[corde].AfficheCorde(dx, dy, fx, fy);

    // Sauve la position du cable pour l'effacer
    B[N].NumSpr = corde;
    B[N].Num = 0;
    B[N].x = dx;
    B[N].y = dy;
    B[N].fx = fx;
    B[N].fy = fy;
    N++;
}

/*** Affiche un text ***/
/***********************/
void Screen::PrintText(e_Sprite Text, int x, int y)
{
    B[N].NumSpr = Text;
    B[N].Num = 0;
    B[N].x = x;
    B[N].y = y;
    N++;

    Sprites[Text].Affiche(x, y, 0);
}

/*** Affiche les options du jeu ***/
/**********************************/
void Screen::PrintOptions(int NV, int NScore)
{
    int x, y;

    Score = NScore;
    AfficheChiffre(740, 215, Score);

    if (NV > 10)
        NV = 10; // Evite un dépassement de l'affichage
    for (int i = 0; i < NV; ++i) { // Affiche les vies
        x = i % 2;
        x = x * 44 + 38 + LT * D_Case;
        y = i / 2;
        y = 384 + y * 46;
        Sprites[logo_vie].Affiche(x, y, 0);
    }
}

/*** Efface les sprites ***/
/**************************/
void Screen::ClearSprite(e_Sprite NumSp)
{
    N = 0;
}

/*** Efface l'ecran avec l'image de fond ***/
/*******************************************/
void Screen::CleanSpriteAndScreen(e_Sprite NumSp)
{
    Sprites[NumSp].Affiche(Sprites[NumSp].Dim[0].L / 2, Sprites[NumSp].Dim[0].H / 2, 0);
    N = 0;
    Score = -1;
}
