//      (_||_/
//      (    )       Screen class
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

#ifndef SCREEN_DOM_
#define SCREEN_DOM_

#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_surface.h>
#include <string>
#include "preference.h"
#include "sprite.h"

class Screen
{
public:
    Screen();
    ~Screen();

    void PrintSprite(e_Sprite NumSpr, int Num, int x, int y); // Displays a sprite
    void PrintCable(int dx, int dy, int fx, int fy); // Displays a cable/rope
    void ChangeFontSize(int size); // Change font size
    void ChangeFontColor(float r, float g, float b);
    void PrintText(const std::string &Text, int x, int y); // Displays text
    int TextLength(const std::string &Text);
    void PrintOptions(int Nlives, int NScore); // Displays information on the side
    void CleanSpriteAndScreen(e_Sprite NumSpritebackground); // Erases display with background image

private:
    int Score { -1 }; // Stores displayed score
    SDL_Color fColor;
    TTF_Font *m_font;
};

#endif
