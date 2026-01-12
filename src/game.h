//      (_||_/
//      (    )       Game class
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

#ifndef GAME_DOM_
#define GAME_DOM_

#include "preference.h"
#include "loco.h"

class Audio;
class Menu;
class Gamepad;

/***    Game class define   ***/
/******************************/
class Game
{
public:
    explicit Game(Audio &sounds, Gamepad &gamepad);
    ~Game() = default;

    void setMenu(Menu *menu) { m_menu = menu; }
    /*** Functions ***/
    /*****************/
    eMenu SDLMain(); // SDL Main
    bool Load(int NivN); // Load a table
    bool DrawLevel(int NivN); // Draw background of game

    void TourneFleche(); // Rotates the arrow with a single touch.
    void BufTouche(int Touche); // Stores a key in the keypad
    int TestFleche(int Haut, int Bas, int Gauche, int Droite); // Test the possible directions for the arrows
    void AfficheEcran(); // Render a screenshot of the game

private:
    /*** Variables ***/
    bool Pause { false }; // Si en pose

    long DureeJeu { 0 }; // Game duration clock

    int Key { 0 }; // Key buffer
    int Touche[4]; // Direction requested
    int T[LT * HT]; // Table pointer

    bool Help { true }; // If it enabled should display the help arrows

    Audio &m_sounds;
    Gamepad &m_gamepad;
    Menu *m_menu { nullptr };

    Loco Lo; // Locomotive object
};
#endif
