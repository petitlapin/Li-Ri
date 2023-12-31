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
#include <string.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#include "preference.h"
#include "game.h"
#include "audio.h"
#include "sprite.h"
#include "screen.h"
#include "mouse.h"
#include "menu.h"
#include "level.h"
#include "editor.h"
#include "utils.h"

/*** Variables globales ***/
/************************/
SDL_Window *sdlWindow; // Pointe sur l'écran video
SDL_Renderer *sdlRenderer; // Pointe sur l'écran video

char Titre[] = "Li-ri V3.0.1";

Sprite *Sprites = nullptr; // Pointe sur les sprites
int NSprites = 0; // Nombre de sprites en mémoire
Screen Ec; // Pointe sur les 2 buffets vidéo
sPreference Pref; // Tableau des préférences.
Game game; // Gère le jeu
Mouse mouse; // Gère les mouvements de sourie
Menu MainMenu; // Gère les menus
Level level; // Gère les niveaux
Editor Edit; // Gère le menu de l'éditeur
Audio Sons; // Gère les sons

int Horloge = 0; // Horloges du jeu
int HorlogeAvant = 0;

#ifdef __unix__
char DefPath[256]; // Chemin par defaut dans arg
#endif

/*** Initialise les preferences ***/
/**********************************/
void InitPref()
{
#ifdef __unix__
    DefPath[0] = 0;
#endif

    Pref.NiveauMax = 0;
    Pref.FullScreen = false;
    Pref.Langue = -1;
    Pref.Volume = (float)SDL_MIX_MAXVOLUME;
    Pref.VolumeM = (float)SDL_MIX_MAXVOLUME; //*6.0/10.0;

    for (int i = 0; i < 8; i++) { // Vide les scores
        Pref.Sco[i].Score = 0;
        Pref.Sco[i].Name[0] = 0;
    }
    /*   Pref.Sco[0].Score=11425; */
    /*   sprintf(Pref.Sco[0].Name,"%s","Dominique"); */
    /*   Pref.Sco[1].Score=678; */
    /*   sprintf(Pref.Sco[1].Name,"%s","Veronique"); */

    Utils::LoadPref();

    Pref.Difficulte = Normal;
    Pref.Vitesse = VITESSE_MOY;
    Pref.VitesseMoy = VITESSE_MOY;
    Pref.NVie = N_VIES_DEP;
    Pref.EcartWagon = ECARTWAGON_MOY;
}

/*** Preogramme principale ***/
/*****************************/
int main(int narg, char *argv[])
{
    int i;
    Sprite Spr;
    eMenu RetM, RetMenu = mMenu;

    // Initialise les préferences
    InitPref();
#ifdef __unix__
    if (narg > 1)
        strcpy(DefPath, argv[1]);
#endif

    // Initiliase SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_AUDIO) < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unable to initialize SDL: %s", SDL_GetError());
        exit(-1);
    }
    // Ferme le programme correctement quand quit
    atexit(SDL_Quit);

    // Demande la resolution Video
    int vOption = SDL_WINDOW_RESIZABLE;
    if (Pref.FullScreen)
        vOption |= SDL_WINDOW_FULLSCREEN_DESKTOP;

    sdlWindow = SDL_CreateWindow(Titre, 0, 0, 800, 600, vOption);
    sdlRenderer = SDL_CreateRenderer(sdlWindow, -1, 0);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
    SDL_RenderSetLogicalSize(sdlRenderer, 800, 600);

    SDL_ShowCursor(0); // Masque le curseur

    // Chargement des sprites
    Sons.Init();
    if (LoadSprites() == false)
        exit(-1);
    if (level.Load() == false)
        exit(-1);

    Sons.PlayMusic();
    mouse.InitStart();

    // Initialise l'horloge et le hazard
    HorlogeAvant = Horloge = SDL_GetTicks();
    srand(SDL_GetTicks());

    // Si pas de langues demande la langue
    if (Pref.Langue == -1)
        MainMenu.SDLMain_Language();

    // Gère les menus
    do {
        switch (RetMenu) {
        case mMenu:
            RetM = MainMenu.SDLMain();
            break;
        case mLangue:
            RetM = MainMenu.SDLMain_Language();
            break;
        case mOption:
            RetM = MainMenu.SDLMain_Options();
            break;
        case mScoreEdit:
            RetM = MainMenu.SDLMain_Score(true);
            break;
        case mScore:
            RetM = MainMenu.SDLMain_Score();
            break;
        case mMenuSpeed:
            RetM = MainMenu.SDLMain_Speed();
            break;
        case mMenuNiveau:
            RetM = MainMenu.SDLMain_Level();
            break;
        case mGame:
            Sons.LoadMusic(1);
            RetM = game.SDLMain();
            Sons.LoadMusic(0);
            break;
        case mEdit:
            RetM = Edit.SDLMain(0);
            break;
        default:
            RetM = mQuit;
        }
        RetMenu = RetM;
    } while (RetMenu != mQuit);

    // Ferme proprement le programme
    Mix_HaltMusic(); // Arrete la music
    Mix_FreeMusic(Sons.Music); // Efface la music

    for (i = 0; i < NSprites; i++) { // Efface les sprites
        Sprites[i].Delete();
    }
    delete[] Sprites;

    Utils::SauvePref(); // Sauve les preferences
    SDL_DestroyRenderer(sdlRenderer);
    SDL_DestroyWindow(sdlWindow);

    SDL_Quit();
    return 0;
}
