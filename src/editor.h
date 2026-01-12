//      (_||_/
//      (    )       Editor class
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

#ifndef EDITOR_DOM_
#define EDITOR_DOM_

#include "preference.h"
#include "sprite.h"

class Mouse;
class Game;
class Gamepad;

/***  Editor class define   ***/
/******************************/
class Editor
{
public:
    Editor(Mouse &mouse, Game &game, Gamepad &gamepad) :
        m_mouse(mouse), m_game(game), m_gamepad(gamepad) { };
    ~Editor() = default;

    /*** Functions***/
    /*****************/
    eMenu SDLMain(int NumNiveau); // Mainloop
    void Affiche() const; // Load a table
    void PrendTouche(int Touche); // Takes pressed keys

private:
    /*** Variables ***/
    int N { 0 }; // Table number to be edited
    e_Sprite Option { locomotive };
    int NumDeco { 0 };
    int NumN { 0 };

    Mouse &m_mouse;
    Game &m_game;
    Gamepad &m_gamepad;
};
#endif
