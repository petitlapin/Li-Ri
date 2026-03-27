//      (_||_/
//      (    )       Sprite class
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

#ifndef SPRITE_DOM_
#define SPRITE_DOM_

#include <SDL2/SDL_render.h>

#define FCOLOR 128
#define ALPHA_SHADOW 125
#define GAP_BETWEEN_NUMBERS 1
#define GAP_BETWEEN_LETTERS 2
#define SPACE_LENGTH 30

/*** Dim struct definition ***/
/*****************************/
struct s_Dim
{
    int L, H;
    int cx, cy; // Image center after cropping
    int bpp; // Number of bytes per pixels, 3 or 4 with alpha
};

/*** Sprite names ***/
/********************/
enum e_Sprite {
    locomotive = 0,
    coal_wagon,
    logs_wagon,
    cargo_wagon,
    engine_wagon,
    cistern_wagon,
    wagon,
    expander,
    shrinker,
    speed,
    life,
    new_wagon,
    logo_health,
    rail,
    dir,
    letters,
    digits,
    title,
    copyright,
    deco,
    cursor,
    arrow_left,
    arrow_right,
    arrows,
    earth,
    music,
    sound,
    fscreen,
    window,
    keys,
    gmenu,
    menu,
    background_hr,
    background_hrr,
    background_menu,
    background,

    fgame,
    fmenu,
    rope,
    loading
};

void DrawLoading(); // displays the loader on the startup page
bool LoadSprites(); // Loads all sprites

class Sprite
{
public:
    Sprite() = default;
    ~Sprite();

    bool Load(unsigned char *Buf, long &P); // Loads images
    void Draw(int X, int Y, int NumSpr, SDL_Texture *Background = nullptr) const; // Displays a sprite
    void PrintRope(int dx, int dy, int fx, int fy);
    bool New(int Lx, int Ly); // Allocates a new empty sprite without transparency
    void Delete(); // Erases memory from the sprite

    int N { 0 }; // Amount of sprites
    SDL_Texture **Image { nullptr }; // Pointer to the sprites
    s_Dim *Dim { nullptr }; // Dimensions of the sprites
};

#endif
