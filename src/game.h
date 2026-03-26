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
class Screen;
class Level;

class Game
{
public:
    explicit Game(Audio &sounds, Screen &screen, Level &level, Gamepad &gamepad);
    ~Game() = default;

    void setMenu(Menu *menu) { m_menu = menu; }
    eMenu SDLMain(); // Main loop
    bool Load(int LevelN); // Load a level/map
    bool DrawLevel(int LevelN); // draws the level

    void TurnArrow(); // Turn intersection arrow on key presses
    void BufKeyPress(int Key); // Store key presses in Keypress array
    int TestArrows(int Up, int Down, int Left, int Right); // Attempts possible directions for the arrows
    void DisplayScreen(); // Displays game screen

private:
    bool Pause { false }; // if paused

    long GameClock { 0 }; // In game time clock

    int Key { 0 }; // Stores keypress
    int KeyPress[4]; // Stores wanted direction
    int T[LT * HT]; // Pointer to level

    bool Help { true }; // Show intersection arrows

    Audio &m_sounds;
    Screen &m_screen;
    Level &m_level;
    Gamepad &m_gamepad;
    Menu *m_menu { nullptr };

    Loco Lo; // locomotive
};
#endif
