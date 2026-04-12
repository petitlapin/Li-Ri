//      (_||_/
//      (    )       Menu class
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

#ifndef MENU_DOM_
#define MENU_DOM_

#include "preference.h"

#include <string>

void Sleeping();

class Game;
class Audio;
class Gamepad;
class Mouse;
class Screen;

class Menu
{
public:
    Menu(Game &game, Audio &audio, Screen &screen, Mouse &mouse, Gamepad &gamepad) :
        m_game(game), m_audio(audio), m_screen(screen), m_mouse(mouse), m_gamepad(gamepad) { };
    ~Menu() = default;

    eMenu SDLMain(); // Main menu
    eMenu SDLMain_Language(); // Language choice menu
    void InitMain_Options(); // Preparing settings menu display
    eMenu SDLMain_Options(); // Settings menun
    eMenu SDLMain_Speed(); // Difficulty menu
    eMenu SDLMain_Level(); // Level selection menu
#ifndef DCHILDREN
    eMenu SDLMain_HR(); // Human rights questions menu
#endif
    void Print_InGame(); // Print menu InGame
    eMenu SDLMain_InGame(); // InGame Menu
    eMenu SDLMain_Score(bool EditScore = false); // Display scores

    void Print_Main(int Center = 400) const; // Display main menu
    void Center_Arrows() const; // Center arrows on the button

private:
    int PyE { 0 }; // Position of the cursor in the menu
    int Level { 0 };

    Game &m_game;
    Audio &m_audio;
    Screen &m_screen;
    Mouse &m_mouse;
    Gamepad &m_gamepad;

    int Year = 2026;
    std::string CopyrightText = "G.P.L. Game - Copyright 2023-" + std::to_string(Year) + " By Johnny Jazeix (fork of Ri-Li originally written by D. Roux-Serret), Music by MAF";
};

#endif
