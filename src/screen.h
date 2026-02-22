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
#include "sprite.h"
#include "preference.h"
#include "string"
#include "unordered_map"

class Screen
{
public:
    Screen();
    ~Screen();

    void PrintSprite(e_Sprite NumSpr, int Num, int x, int y); // Affiche un sprite
    void PrintCable(int dx, int dy, int fx, int fy); // Affiche un cable
    void ChangeFontSize(int size); // Change font size
    void PrintText(const std::string &Text, int x, int y); // Affiche un text à l'ecran
    void PrintOptions(int NVies, int NScore); // Affiche les options sur le coté
    void CleanSpriteAndScreen(e_Sprite NumSpriteFondEcran); // Efface l'ecran avec l'image de fond

private:
    /*** Variables ***/
    int Score { -1 }; // Mémorise le score affiché
    TTF_Font* m_font;
    std::unordered_map<std::string, SDL_Texture*> cachedString;
};

#endif
