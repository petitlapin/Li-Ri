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
    loading,

    T_level,
    T_lives,
    T_options,
    T_score,
    T_press_any_key,
    T_continue,
    T_moptions,
    T_exit_game,
    T_play,
    T_scores,
    T_better_scores,
    T_quit,
    T_new_game,
    T_old_level,
    T_menu,
    T_easy,
    T_normal,
    T_hard,
#ifndef DCHILDREN
    T_question,
    T_tart1,
    T_tart2,
    T_tart3,
    T_tart4,
    T_tart5,
    T_tart6,
    T_tart7,
    T_tart8,
    T_tart9,
    T_tart10,
    T_tart11,
    T_tart12,
    T_tart13,
    T_tart14,
    T_tart15,
    T_tart16,
    T_tart17,
    T_tart18,
    T_tart19,
    T_tart20,
    T_tart21,
    T_tart22,
    T_tart23,
    T_tart24,
    T_tart25,
    T_tart26,
    T_tart27,
    T_tart28,
    T_tart29,
    T_tart30,
    T_art1,
    T_art2,
    T_art3,
    T_art4,
    T_art5,
    T_art6,
    T_art7,
    T_art8,
    T_art9,
    T_art10,
    T_art11,
    T_art12,
    T_art13,
    T_art14,
    T_art15,
    T_art16,
    T_art17,
    T_art18,
    T_art19,
    T_art20,
    T_art21,
    T_art22,
    T_art23,
    T_art24,
    T_art25,
    T_art26,
    T_art27,
    T_art28,
    T_art29,
    T_art30,
#endif
    T_Language,

    T_ENDTEXT
};

void DrawLoading(); // displays the loader on the startup page
bool LoadLanguage(); // Loads sprites of a language
bool LoadSprites(); // Loads all sprites

int NumberLength(int C); // Returns the length in pixels of a number
int StringLength(char *Text); // Returns the length in pixels of a string.
bool CharExist(char C); // Checks if a character exists

void DrawNumber(int x, int y, int Number, SDL_Texture *Background = nullptr); // Displays a number
void DrawString(int x, int y, char *Text, SDL_Texture *Background = nullptr); // Displays a string

void DrawText(int x, int y, e_Sprite Text, SDL_Texture *Background = nullptr); // Displays a text in the language

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
