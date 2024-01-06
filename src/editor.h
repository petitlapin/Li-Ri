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

/*** Définition de la class ***/
/******************************/
class Editor
{
public:
    Editor() = default;
    ~Editor() = default;

    /*** Fonctions ***/
    /*****************/
    eMenu SDLMain(int NumNiveau); // Boucle principale
    void Affiche() const; // Charge un tableau
    void PrendTouche(int Touche); // Prend les touches enfoncées

private:
    /*** Variables ***/
    int N { 0 }; // Numero du tableau à éditer
    e_Sprite Option;
    int NumDeco { 0 };
    int NumN;
};
#endif
