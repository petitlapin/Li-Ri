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

#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>

#include "editor.h"
#include "menu.h"
#include "game.h"
#include "level.h"
#include "mouse.h"

/*** Variables globales ***/
/**************************/
extern SDL_Renderer *sdlRenderer;

extern Sprite *Sprites;
extern sPreference Pref;

extern int Horloge;
extern int HorlogeAvant;

extern Level level;
extern Mouse mouse;
extern Game game;

static int NumRail[] = { 10, 10, 10, 0, 10, 1, 2, 3, 10, 4, 5, 6, 7, 8, 9, 10 };

/*** Constructeur et Destructeur ***/
/***********************************/
Editor::Editor() :
    N(0), NumDeco(0)
{
}

Editor::~Editor()
{
}

/*** SDL Main ***/
/****************/
eMenu Editor::SDLMain(int NumNiv)
{
    int PyE;
    int cx = 0, cy = 0;
    bool Boutton = false;
    int TypeB = -1;
    int i, d, dx, dy;

    NumN = NumNiv;

    Affiche(); // Charge le tableau
    SDL_RenderPresent(sdlRenderer);

    Horloge = SDL_GetTicks(); // Prend l'horloge

    Option = rail;

    // Initialise la sourie
    mouse.Init(NULL);

    // Prend les evenements
    do {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            mouse.GetEvent(event, PyE); // Prend les evenements de la sourie
            switch (event.type) {
            case SDL_WINDOWEVENT:
                if (event.window.event == SDL_WINDOWEVENT_ENTER)
                    Affiche();
                break;
            case SDL_KEYDOWN:
                if (event.key.state == SDL_PRESSED) {
                    if (event.key.keysym.sym == SDLK_ESCAPE)
                        return mMenu;
                    else
                        PrendTouche(event.key.keysym.sym);
                }
                break;
            case SDL_MOUSEBUTTONDOWN:
                if (event.button.state == SDL_PRESSED) {
                    Boutton = true;
                    TypeB = -1;
                }
                break;
            case SDL_MOUSEBUTTONUP:
                Boutton = false;
                if (TypeB != -1 && Option == deco && cx >= LT) { // Si doit effacer une décoration
                    level.T[NumN].NDeco--;
                }
                TypeB = -1;
                break;
            case SDL_QUIT:
                return mQuit;
                break;
            }
        }

        // Gère l'appuis du boutton de la sourie
        cx = mouse.Px / D_Case;
        cy = mouse.Py / D_Case;

        if (Boutton && cx < LT)
            switch (Option) {
            case deco:
                if (TypeB == -1) { // Si première fois que appuis sur la touche
                    for (i = 0; i < level.T[NumN].NDeco; i++) { // Recherche si décoration proche du clic
                        dx = level.T[NumN].Deco[i].x - mouse.Px;
                        dy = level.T[NumN].Deco[i].y - mouse.Py;
                        d = dx * dx + dy * dy;
                        if (d <= (D_Case * 2) * (D_Case * 2)) {
                            TypeB = i;
                        }
                    }
                    if (TypeB == -1) { // Si doit fair un nouveau décor
                        level.T[NumN].NDeco++;
                        level.T[NumN].Deco[(level.T[NumN].NDeco - 1)].NumSpr = NumDeco;
                        level.T[NumN].Deco[(level.T[NumN].NDeco - 1)].x = mouse.Px;
                        level.T[NumN].Deco[(level.T[NumN].NDeco - 1)].y = mouse.Py;
                        TypeB = 1;
                    }
                    else { // Fait passe la selection au premier plan
                        level.T[NumN].Deco[level.T[NumN].NDeco].NumSpr = level.T[NumN].Deco[TypeB].NumSpr;
                        level.T[NumN].Deco[level.T[NumN].NDeco].x = level.T[NumN].Deco[TypeB].x;
                        level.T[NumN].Deco[level.T[NumN].NDeco].y = level.T[NumN].Deco[TypeB].y;
                        for (d = TypeB; d < level.T[NumN].NDeco; d++) {
                            level.T[NumN].Deco[d].NumSpr = level.T[NumN].Deco[d + 1].NumSpr;
                            level.T[NumN].Deco[d].x = level.T[NumN].Deco[d + 1].x;
                            level.T[NumN].Deco[d].y = level.T[NumN].Deco[d + 1].y;
                        }
                        NumDeco = level.T[NumN].Deco[(level.T[NumN].NDeco - 1)].NumSpr;
                    }
                }
                else { // Si pas la première fois remplace
                    level.T[NumN].Deco[(level.T[NumN].NDeco - 1)].NumSpr = NumDeco;
                    level.T[NumN].Deco[(level.T[NumN].NDeco - 1)].x = mouse.Px;
                    level.T[NumN].Deco[(level.T[NumN].NDeco - 1)].y = mouse.Py;
                }
                break;
            case rail:
                if (TypeB == -1) {
                    if (level.T[NumN].T[cy * LT + cx] != C_Rail)
                        TypeB = C_Rail;
                    else
                        TypeB = C_None;
                }
                level.T[NumN].T[cy * LT + cx] = TypeB;
                break;
            case wagon:
                level.T[NumN].T[cy * LT + cx] = C_Wagon;
                break;
            case pluslong:
                level.T[NumN].T[cy * LT + cx] = C_Allonge;
                break;
            case pluscourt:
                level.T[NumN].T[cy * LT + cx] = C_Reduit;
                break;
            case vitesse:
                level.T[NumN].T[cy * LT + cx] = C_Speed;
                break;
            case vie:
                level.T[NumN].T[cy * LT + cx] = C_Live;
                break;
            case (e_Sprite)(locomotive + D_Haut):
            case (e_Sprite)(locomotive + D_Bas):
            case (e_Sprite)(locomotive + D_Gauche):
            case (e_Sprite)(locomotive + D_Droite):
                level.T[NumN].DepX = cx;
                level.T[NumN].DepY = cy;
                level.T[NumN].DepDir = (int)(Option) - (int)(locomotive);
                break;
            default:
                break;
            }

        // Gère les Horloges et la pose
        HorlogeAvant = Horloge;
        Horloge = SDL_GetTicks();
        Sleeping();

        // Fait l'affichage
        Affiche();
        SDL_RenderPresent(sdlRenderer);

    } while (true);

    return mQuit;
}

/*** Charge un tableau ***/
/*************************/
void Editor::Affiche()
{
    int i, x, y, m, cx, cy;
    unsigned char *T;

    // Prend l'adresse du niveau
    T = level.T[NumN].T;

    // Fabrique le fond du jeu
    Sprites[fond].Affiche(400, 300, 0);

    // Affiche le circuit
    for (i = 0; i < LT * HT; i++) {
        if (T[i] >= C_Rail && T[i] < C_Fin) {
            y = i / LT * D_Case + D_Case / 2;
            x = i % LT * D_Case + D_Case / 2;

            m = 0;
            cx = i % LT;
            cy = i / LT;
            if (cy > 0 && T[i - LT] >= 1 && T[i - LT] < C_Fin)
                m += 8;
            if (cy < HT - 1 && T[i + LT] >= 1 && T[i + LT] < C_Fin)
                m += 4;
            if (cx > 0 && T[i - 1] >= 1 && T[i - 1] < C_Fin)
                m += 2;
            if (cx < LT - 1 && T[i + 1] >= 1 && T[i + 1] < C_Fin)
                m += 1;

            Sprites[rail].Affiche(x, y, NumRail[m]);
        }
    }

    // Affiche les décorations
    for (i = 0; i < level.T[NumN].NDeco; i++)
        Sprites[deco].Affiche(level.T[NumN].Deco[i].x, level.T[NumN].Deco[i].y, level.T[NumN].Deco[i].NumSpr);

    // Affiche numero du niveau
    AfficheChiffre(740, 130, NumN + 1);

    // Affiche les options
    for (i = 0; i < LT * HT; i++) {
        switch (T[i]) {
        case C_Wagon: // Si un loco
            Sprites[wagon].Affiche(i % LT * D_Case + D_Case / 2, i / LT * D_Case + D_Case / 2, 25);
            break;
        case C_Allonge: // Si plus long
            Sprites[pluslong].Affiche(i % LT * D_Case + D_Case / 2, i / LT * D_Case + D_Case / 2, 25);
            break;
        case C_Reduit: // Si plus court
            Sprites[pluscourt].Affiche(i % LT * D_Case + D_Case / 2, i / LT * D_Case + D_Case / 2, 25);
            break;
        case C_Speed: // Si plus vite
            Sprites[vitesse].Affiche(i % LT * D_Case + D_Case / 2, i / LT * D_Case + D_Case / 2, 25);
            break;
        case C_Live: // Si une vie
            Sprites[vie].Affiche(i % LT * D_Case + D_Case / 2, i / LT * D_Case + D_Case / 2, 25);
            break;
        }
    }

    // Affiche le départ de la locomotive
    switch (level.T[NumN].DepDir) {
    case D_Haut:
        Sprites[locomotive].Affiche(level.T[NumN].DepX * D_Case + D_Case / 2, level.T[NumN].DepY * D_Case + D_Case / 2, 0);
        break;
    case D_Bas:
        Sprites[locomotive].Affiche(level.T[NumN].DepX * D_Case + D_Case / 2, level.T[NumN].DepY * D_Case + D_Case / 2, 40);
        break;
    case D_Gauche:
        Sprites[locomotive].Affiche(level.T[NumN].DepX * D_Case + D_Case / 2, level.T[NumN].DepY * D_Case + D_Case / 2, 80);
        break;
    case D_Droite:
        Sprites[locomotive].Affiche(level.T[NumN].DepX * D_Case + D_Case / 2, level.T[NumN].DepY * D_Case + D_Case / 2, 120);
        break;
    }

    // Affiche l'option choisi dans le menu
    switch (Option) {
    case rail:
    case wagon:
    case pluslong:
    case pluscourt:
    case vitesse:
    case vie:
        Sprites[Option].Affiche(740, 200, 0);
        break;
    case (e_Sprite)(locomotive + D_Haut):
        Sprites[locomotive].Affiche(740, 200, 0);
        break;
    case (e_Sprite)(locomotive + D_Bas):
        Sprites[locomotive].Affiche(740, 200, 40);
        break;
    case (e_Sprite)(locomotive + D_Gauche):
        Sprites[locomotive].Affiche(740, 200, 80);
        break;
    case (e_Sprite)(locomotive + D_Droite):
        Sprites[locomotive].Affiche(740, 200, 120);
        break;
    case deco:
        Sprites[deco].Affiche(740, 200, NumDeco);
        break;
    default:
        break;
    }

    // Affiche le curseur
    if (Option != deco)
        Sprites[curseur].Affiche(mouse.Px, mouse.Py, 0);
    else
        Sprites[deco].Affiche(mouse.Px, mouse.Py, NumDeco);
}

/*** Prend les touches enfoncées ***/
/***********************************/
void Editor::PrendTouche(int Tou)
{
    int i, j;

    switch (Tou) {
    case SDLK_PAGEUP:
        if (NumN < level.N - 1)
            NumN++;
        else {
            j = 0;
            for (i = 0; i < LT * HT; i++)
                j += level.T[NumN].T[i];
            if (j != 0) {
                NumN++;
                level.N++;
                level.Clear(NumN);
            }
        }
        break;
    case SDLK_PAGEDOWN:
        if (NumN)
            NumN--;
        break;
    case 'p':
        Option = deco;
        NumDeco++;
        if (NumDeco >= Sprites[deco].N)
            NumDeco = 0;
        break;
    case 'o':
        Option = deco;
        NumDeco--;
        if (NumDeco < 0)
            NumDeco = Sprites[deco].N - 1;
        break;
    case SDLK_INSERT:
        level.Ins(NumN);
        break;
    case SDLK_DELETE:
        level.Del(NumN);
        break;
    case SDLK_UP:
        Option = (e_Sprite)(locomotive + D_Haut);
        break;
    case SDLK_DOWN:
        Option = (e_Sprite)(locomotive + D_Bas);
        break;
    case SDLK_LEFT:
        Option = (e_Sprite)(locomotive + D_Gauche);
        break;
    case SDLK_RIGHT:
        Option = (e_Sprite)(locomotive + D_Droite);
        break;
    case 'c':
        level.Clear(NumN);
        break;
    case '$':
        // test si le dernier niveau est vide
        for (j = i = 0; i < LT * HT; i++)
            j += level.T[level.N - 1].T[i];
        if (j == 0) {
            if (NumN < level.N - 1)
                level.N--; // Si vide ne le compte pas
        }

        // Sauve le niveau
        if (level.Save() == false) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Error while saving levels");
            exit(-1);
        }

        // test le niveau
        Pref.Niveau = NumN;
        game.SDLMain();
        mouse.Init(NULL);
        Affiche();
        break;
    case 'a':
        Option = rail;
        break;
    case 'z':
        Option = wagon;
        break;
    case 'e':
        Option = pluslong;
        break;
    case 'r':
        Option = pluscourt;
        break;
    case 't':
        Option = vitesse;
        break;
    case 'y':
        Option = vie;
        break;
    }
}
