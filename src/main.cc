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

#include <cstdlib>
#include <cstring>
#include <SDL2/SDL_error.h> // for SDL_GetError
#include <SDL2/SDL_hints.h> // for SDL_SetHint, SDL_HINT_RENDER_SCALE_QUALITY
#include <SDL2/SDL_log.h> // for SDL_LogError, SDL_LOG_CATEGORY_APPLICATION
#include <SDL2/SDL_mouse.h> // for SDL_ShowCursor
#include <SDL2/SDL_render.h> // for SDL_CreateRenderer, SDL_DestroyRenderer
#include <SDL2/SDL_timer.h> // for SDL_GetTicks
#include <SDL2/SDL_video.h> // for SDL_CreateWindow, SDL_DestroyWindow
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#include "config.h"
#include "preference.h"
#include "game.h"
#include "gamepad.h"
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

char Titre[] = "Li-ri V" VERSION;

Sprite *Sprites = nullptr; // Pointe sur les sprites
int NSprites = 0; // Nombre de sprites en mémoire
Screen Ec; // Pointe sur les 2 buffets vidéo
sNewPreference Pref; // Tableau des préférences.
Level level; // Gère les niveaux

int Horloge = 0; // Horloges du jeu
int HorlogeAvant = 0;

#if defined(__unix__) || defined(__HAIKU__)
char DefPath[256]; // Chemin par defaut dans arg
#endif

/*** Initialise les preferences ***/
/**********************************/
void InitPref()
{
#if defined(__unix__) || defined(__HAIKU__)
    DefPath[0] = 0;
#endif

    for (int i = 0; i < 8; i++) { // Vide les scores
        Pref.Sco[i].Score = 0;
        Pref.Sco[i].Name[0] = 0;
    }

    Utils::LoadPref();
}

/*** Preogramme principale ***/
/*****************************/
int main(int narg, char *argv[])
{
    int i;
    eMenu RetM, RetMenu = mMenu;

    // Initialise les préferences
    InitPref();
#if defined(__unix__) || defined(__HAIKU__)
    if (narg > 1) {
        strcpy(DefPath, argv[1]);
    }
#endif

    SDL_SetHint(SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS, "1");
    // Initiliase SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER) < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Unable to initialize SDL: %s", SDL_GetError());
        exit(-1);
    }
    // Ferme le programme correctement quand quit
    atexit(SDL_Quit);

    // Demande la resolution Video
    int vOption = SDL_WINDOW_RESIZABLE;
    if (Pref.FullScreen) {
        vOption |= SDL_WINDOW_FULLSCREEN_DESKTOP;
    }

    sdlWindow = SDL_CreateWindow(Titre, 0, 0, 800, 600, vOption);
    sdlRenderer = SDL_CreateRenderer(sdlWindow, -1, 0);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
    SDL_RenderSetLogicalSize(sdlRenderer, 800, 600);

    SDL_ShowCursor(0); // Masque le curseur

    Audio audio;
    audio.Init();
    if (LoadSprites() == false) {
        exit(-1);
    }
    if (level.Load() == false) {
        exit(-1);
    }

    audio.PlayMusic();
    Mouse mouse { audio };
    mouse.InitStart();

    Gamepad gamepad;
    gamepad.Initialize();

    Game game { audio, gamepad };
    Editor editor { mouse, game, gamepad };

    Menu MainMenu { game, audio, mouse, gamepad };
    game.setMenu(&MainMenu);

    HorlogeAvant = Horloge = SDL_GetTicks();
    srand(SDL_GetTicks());

    // ask locale if first run
    if (Pref.Langue == -1) {
        RetMenu = MainMenu.SDLMain_Language();
        LoadLangue();
    }

    // menu switch
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
            audio.LoadMusic(1);
            RetM = game.SDLMain();
            audio.LoadMusic(0);
            break;
        case mEdit:
            RetM = editor.SDLMain(0);
            break;
        default:
            RetM = mQuit;
        }
        RetMenu = RetM;
    } while (RetMenu != mQuit);

    // close program
    Mix_HaltMusic();
    Mix_FreeMusic(audio.Music);

    for (i = 0; i < NSprites; i++) {
        Sprites[i].Delete();
    }
    delete[] Sprites;

    Utils::SauvePref();
    SDL_DestroyRenderer(sdlRenderer);
    SDL_DestroyWindow(sdlWindow);

    Mix_Quit();
    SDL_Quit();
    return 0;
}
