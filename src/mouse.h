//      (_||_/
//      (    )       Mouse class
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

#ifndef MOUSE_DOM_
#define MOUSE_DOM_

#include <SDL2/SDL_events.h>

/*** Structure pour la position dans le menu Py ***/
/**************************************************/
struct mPy
{ // DepX==-1 si derniere entrée
    int DepX, DepY;
    int FinX, FinY;
    int Py;
    bool Valide; // Si doit valider un enter quand click de la sourie
};

struct mButton
{ // DepX=-1 si derniere entrée
    int DepX, DepY;
    int FinX, FinY;
    int Valeur;
    int *Adr;
};

/*** Définition de la class Tableau ***/
/**************************************/
class Mouse
{
public:
    Mouse() = default;
    ~Mouse() = default;

    void InitStart(); // Initialise les coordonnées de la sourie
    void Init(struct mPy *TablePy, struct mButton *B = nullptr); // Initialise la sourie
    void GetEvent(SDL_Event &event, int &Py); // Prend les evenements
    void Print() const; // Affiche le curseur

    int Px { 400 }, Py { 300 }; // Mouse screen position

private:
    struct mPy *tPy; // Pointe sur coordonées pour Py
    struct mButton *Bo; // Pointe sur les coordonnées des bouttons
};

#endif
