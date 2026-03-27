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
#include "screen.h"
#include "sprite.h"
#include "utils.h"

extern Sprite *Sprites;
extern SDL_Renderer *sdlRenderer;

/*** Constructor ***/
/*******************/
Screen::Screen()
{
    // Loading font
    char fontPath[512];
    strcpy(fontPath, "Assets/Fonts/comic.ttf");
    Utils::GetPath(fontPath);
    m_font = TTF_OpenFont(fontPath, 28);

    // Selecting font color
    fColor.r = 255;
    fColor.g = 255;
    fColor.b = 255;
    fColor.a = 255;
}

/*** Destructor ***/
/******************/
Screen::~Screen()
{
    // Destroying font
    SDL_DestroyTexture(fontTexture);
    TTF_CloseFont(m_font);
}

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

void Screen::ChangeFontSize(int size){
    TTF_SetFontSize(m_font, size);
}

void Screen::ChangeFontColor(float r, float g, float b){
    fColor.r = r;
    fColor.g = g;
    fColor.b = b;
    fColor.a = 255;
}

/*** Display a text ***/
/**********************/
void Screen::PrintText(const std::string &Text, int x, int y)
{
    fontSurface = TTF_RenderUTF8_Blended(m_font, Text.c_str(), fColor); // Rendering text
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

int Screen::TextLength(std::string Text){
    int w,h;
    TTF_SizeUTF8(m_font, Text.c_str(), &w, &h);
    return w;
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
