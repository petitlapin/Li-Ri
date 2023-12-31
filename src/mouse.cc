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

#include <cstdio>
#include "mouse.h"
#include "preference.h"
#include "screen.h"
#include "audio.h"

/*** Variables Globales ***/
/**************************/
extern Audio Sons;
extern int Horloge;
extern Screen Ec;
extern SDL_Window *sdlWindow;

/*** Initialise la sourie ***/
/****************************/
void Mouse::InitStart()
{
    // Initialise les coordonnées de la sourie
    Px = 400;
    Py = 300;
    SDL_WarpMouseInWindow(sdlWindow, Px, Py);
}

/*** Initialise un bebut d'utilisation ***/
/*****************************************/
void Mouse::Init(struct mPy *TablePy, struct mButton *B)
{
    // Sauve les adresses utils
    tPy = TablePy;
    Bo = B;
}

/*** Prend les evenements de la sourie ***/
/*****************************************/
void Mouse::GetEvent(SDL_Event &event, int &pPy)
{
    int i;

    switch (event.type) {
    case SDL_MOUSEMOTION: // Si mouvement de la sourie
        Px = event.motion.x;
        Py = event.motion.y;
        // regarde si doit bouger la position de Py
        if (tPy) { // Si bien une table
            i = 0;
            while (tPy[i].DepX != -1) { // Fait toutes les coordonnées
                if (Px >= tPy[i].DepX && Px <= tPy[i].FinX && Py >= tPy[i].DepY && Py <= tPy[i].FinY) {
                    if (pPy != tPy[i].Py) {
                        pPy = tPy[i].Py;
                        Sons.Play(sClic);
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

            // regarde si doit valider un enter
            if (tPy) { // Si bien une table
                i = 0;
                while (tPy[i].DepX != -1) { // Fait toutes les coordonnées
                    if (Px >= tPy[i].DepX && Px <= tPy[i].FinX && Py >= tPy[i].DepY && Py <= tPy[i].FinY) {
                        if (tPy[i].Valide == true) {
                            event.type = SDL_KEYDOWN;
                            event.key.state = SDL_PRESSED;
                            event.key.keysym.sym = SDLK_RETURN;
                        }
                    }
                    i++;
                };
            }

            // Fait la gestion des bouttons
            if (Bo) { // Si bien une table
                i = 0;
                while (Bo[i].DepX != -1) { // Fait toutes les coordonnées
                    if (Px >= Bo[i].DepX && Px <= Bo[i].FinX && Py >= Bo[i].DepY && Py <= Bo[i].FinY) {
                        if (Bo[i].Adr == nullptr) { // Si doit fair une touche
                            event.type = SDL_KEYDOWN;
                            event.key.state = SDL_PRESSED;
                            event.key.keysym.sym = (SDL_Keycode)Bo[i].Valeur;
                        }
                        else { // Si doit changer une variable
                            *(Bo[i].Adr) = Bo[i].Valeur;
                        }
                    }
                    i++;
                };
            }
            break;
        }
    }
}

/*** Affiche le curseur ***/
/**************************/
void Mouse::Print() const
{
    int X = Px, Y = Py;
    int const NumSp = (Horloge / 50) % 20;

    // Corrige la position du curseur au cas ou déborde de l'écran
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
    // Affiche le curseur
    Ec.PrintSprite(curseur, NumSp, X, Y);
#endif
}
