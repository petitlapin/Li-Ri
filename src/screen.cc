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
#include "utils.h"

/*** Variables globales ***/
/**************************/
extern Sprite *Sprites;
extern SDL_Renderer *sdlRenderer;

void Screen::Init()
{
    char fontPath[512];
    strcpy(fontPath, "Andika-R.ttf");
    Utils::GetPath(fontPath);

    m_font = TTF_OpenFont(fontPath, 28);
}

void Screen::Delete()
{
    for (auto &texture: cachedString) {
        SDL_DestroyTexture(texture.second);
    }
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

void Screen::PrintText(const std::string &Text, int x, int y)
{
    SDL_Texture *texture = nullptr;
    if (cachedString.count(Text) > 0) {
        texture = cachedString[Text];
    }
    else {
        SDL_Color fgColor = { 255, 255, 255, 255 };
        SDL_Surface *surf = TTF_RenderUTF8_Blended(m_font, Text.c_str(), fgColor);
        texture = SDL_CreateTextureFromSurface(sdlRenderer, surf);
        SDL_FreeSurface(surf);
        cachedString.insert({ Text, texture });
    }

    SDL_Rect Position;

    SDL_QueryTexture(texture, NULL, NULL, &Position.w, &Position.h);
    Position.x = x - Position.w;
    Position.y = y - Position.h;

    SDL_RenderCopy(sdlRenderer, texture, nullptr, &Position);
}

/*** Affiche un text ***/
/***********************/
void Screen::PrintText(e_Sprite Text, int x, int y)
{
    Sprites[Text].Draw(x, y, 0);
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
