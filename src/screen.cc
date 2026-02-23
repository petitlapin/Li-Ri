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

#include <SDL2/SDL_log.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_timer.h>
#include "preference.h"
#include "sprite.h"
#include "screen.h"
#include "utils.h"

/*** Variables globales ***/
/**************************/
extern Sprite *Sprites;
extern SDL_Renderer *sdlRenderer;

Screen::Screen()
{
    char fontPath[512];
    strcpy(fontPath, "comic.ttf");
    Utils::GetPath(fontPath);

    m_font = TTF_OpenFont(fontPath, 28);
}

Screen::~Screen()
{
    SDL_DestroyTexture(fontTexture);
    TTF_CloseFont(m_font);
}

/*** Affiche un Sprite ***/
/*************************/
void Screen::PrintSprite(e_Sprite NumSpr, int Num, int x, int y)
{
    Sprites[NumSpr].Draw(x, y, Num);
}

void Screen::PrintCable(int dx, int dy, int fx, int fy)
{
    // Show the rope between the wagons
    Sprites[rope].PrintRope(dx, dy, fx, fy);
}

void Screen::ChangeFontSize(int size){
	TTF_SetFontSize(m_font, size);
}

void Screen::PrintText(const std::string &Text, int x, int y)
{
    SDL_Color fColor;
    fColor.r = 255;
    fColor.g = 255;
    fColor.b = 255;

    fontSurface = TTF_RenderUTF8_Solid(m_font, Text.c_str(), fColor); // Rendering text
    fontTexture = SDL_CreateTextureFromSurface(sdlRenderer, fontSurface); // Creating texture

    // Setting position and size
    SDL_Rect dst;
    dst.x = x;
    dst.y = y;
    dst.w = fontSurface->w;
    dst.h = fontSurface->h;

    // Rendering text
    SDL_RenderCopy(sdlRenderer, fontTexture, nullptr, &dst);
}

/*** Affiche les options du jeu ***/
/**********************************/
void Screen::PrintOptions(int NV, int NScore)
{
    int x, y;

    Score = NScore;
    DrawNumber(740, 215, Score);

    if (NV > 10) {
        NV = 10; // Evite un dépassement de l'affichage
    }
    for (int i = 0; i < NV; ++i) { // Affiche les vies
        x = i % 2;
        x = x * 44 + 38 + LT * D_Case;
        y = i / 2;
        y = 384 + y * 46;
        Sprites[logo_vie].Draw(x, y, 0);
    }
}

/*** Efface l'ecran avec l'image de fond ***/
/*******************************************/
void Screen::CleanSpriteAndScreen(e_Sprite NumSp)
{
    Sprites[NumSp].Draw(Sprites[NumSp].Dim[0].L / 2, Sprites[NumSp].Dim[0].H / 2, 0);
    Score = -1;
}
