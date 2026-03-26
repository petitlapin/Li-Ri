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

#include <SDL2/SDL_keycode.h> // for SDLK_RETURN, SDL_Keycode
#include <SDL2/SDL_mouse.h> // for SDL_WarpMouseInWindow
#include <SDL2/SDL_video.h> // for SDL_Window
#include "audio.h"
#include "mouse.h"
#include "screen.h"
#include "sprite.h" // for e_Sprite

extern int currentTime;
extern SDL_Window *sdlWindow;

/*** Mouse init ***/
/******************/
void Mouse::InitStart()
{
    // Init mouse coordinates
    Px = 400;
    Py = 300;
    SDL_WarpMouseInWindow(sdlWindow, Px, Py);
}

/*** Initialise the mouse for use ***/
/************************************/
void Mouse::Init(struct mPy *TablePy, struct mButton *B)
{
    // Save useful pointers
    tPy = TablePy;
    Bo = B;
}

/*** Fetch mouse events ***/
/**************************/
void Mouse::GetEvent(SDL_Event &event, int &pPy)
{
    int i;

    switch (event.type) {
    case SDL_MOUSEMOTION: // If mouse moves
        Px = event.motion.x;
        Py = event.motion.y;
        // Check if py's position must move
        if (tPy) { // If the table exists
            i = 0;
            while (tPy[i].StartX != -1) { // iterate through all coordinates
                if (Px >= tPy[i].StartX && Px <= tPy[i].EndX && Py >= tPy[i].StartY && Py <= tPy[i].EndY) {
                    if (pPy != tPy[i].Py) {
                        pPy = tPy[i].Py;
                        m_audio.Play(sClick);
                    }
                }
                i++;
            };
        }
        break;
    case SDL_MOUSEBUTTONDOWN:
        if (event.button.state == SDL_PRESSED) {
            Px = event.button.x;
            Py = event.button.y;

            // Trigger Enter key if clicking on a valid region
            if (tPy) { // If table exists
                i = 0;
                while (tPy[i].StartX != -1) { // iterate through defined regions
                    if (Px >= tPy[i].StartX && Px <= tPy[i].EndX && Py >= tPy[i].StartY && Py <= tPy[i].EndY) {
                        if (tPy[i].Valid == true) {
                            event.type = SDL_KEYDOWN;
                            event.key.state = SDL_PRESSED;
                            event.key.keysym.sym = SDLK_RETURN;
                        }
                    }
                    i++;
                };
            }

            // Handle button interactions
            if (Bo) { // If table exists
                i = 0;
                while (Bo[i].StartX != -1) { // Iterate through defined regions
                    if (Px >= Bo[i].StartX && Px <= Bo[i].EndX && Py >= Bo[i].StartY && Py <= Bo[i].EndY) {
                        if (Bo[i].Adr == nullptr) { // If a key press should be triggered
                            event.type = SDL_KEYDOWN;
                            event.key.state = SDL_PRESSED;
                            event.key.keysym.sym = (SDL_Keycode)Bo[i].Value;
                        }
                        else { // If a variable should be changed
                            *(Bo[i].Adr) = Bo[i].Value;
                        }
                    }
                    i++;
                };
            }
            break;
        }
    }
}

/*** Display cursor ***/
/**********************/
void Mouse::Print() const
{
    int X = Px, Y = Py;
    int const NumSp = (currentTime / 50) % 20;

    // Correct cursor position if it goes off screen
    if (X < 5) {
        X = 5;
    }
    if (X >= 800 - 38) {
        X = 800 - 39;
    }
    if (Y < 10) {
        Y = 10;
    }
    if (Y >= 600 - 35) {
        Y = 600 - 36;
    }

#ifndef ANDROID
    // Display cursor
    m_screen.PrintSprite(cursor, NumSp, X, Y);
#endif
}
