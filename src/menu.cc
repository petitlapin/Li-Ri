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
#include <cstdlib>
#include <cstring>

#include <SDL2/SDL_audio.h> // for SDL_MIX_MAXVOLUME
#include <SDL2/SDL_events.h> // for SDL_PollEvent, SDL_Event, SDL_KEYDOWN
#include <SDL2/SDL_keyboard.h> // for SDL_StopTextInput, SDL_StartTextInput
#include <SDL2/SDL_keycode.h> // for SDLK_F12, SDLK_RETURN, SDLK_ESCAPE
#include <SDL2/SDL_mouse.h> // for SDL_ShowCursor
#include <SDL2/SDL_rect.h> // for SDL_Rect
#include <SDL2/SDL_render.h> // for SDL_RenderPresent, SDL_RenderClear
#include <SDL2/SDL_stdinc.h> // for Uint32
#include <SDL2/SDL_timer.h> // for SDL_GetTicks, SDL_Delay
#include <SDL2/SDL_video.h> // for SDL_WINDOWEVENT_ENTER, SDL_SetWindowF...

#include <array>
#include <utility>
#include "preference.h"
#include "menu.h"
#include "sprite.h"
#include "screen.h"
#include "gamepad.h"
#include "mouse.h"
#include "audio.h"
#include "utils.h"

#define PY 180

/*** Variables globales ***/
/**************************/
extern int Horloge;
extern int HorlogeAvant;
extern SDL_Window *sdlWindow;
extern SDL_Renderer *sdlRenderer;
extern Sprite *Sprites;
extern Screen Ec;
extern sNewPreference Pref;

static char Points[] = ". . . . . . . . . . . . . .";
static struct mPy Menu_Py[20];

/*** Fait une attente pour 50fps ***/
/***********************************/
void Sleeping()
{
    int delay;

    if ((Horloge - HorlogeAvant) < (1000 / 60)) {
        delay = 1000 / 60 - (Horloge - HorlogeAvant);
        SDL_Delay(delay);
        Horloge = SDL_GetTicks();
    }
}

/*** Ajoute une entrée dans le tableau des boutons ***/
/*****************************************************/
void AddBouton(int Num, e_Sprite NumSp, int X, int Y)
{
    int const NumS = (int)NumSp;

    Menu_Py[Num].DepX = X - Sprites[NumS].Dim[0].L / 2;
    Menu_Py[Num].DepY = Y - Sprites[NumS].Dim[0].H / 2;
    Menu_Py[Num].FinX = X + Sprites[NumS].Dim[0].L / 2;
    Menu_Py[Num].FinY = Y + Sprites[NumS].Dim[0].H / 2;
    Menu_Py[Num].Py = Num;
    Menu_Py[Num].Valide = true;
}

/*** Change le vidéo ***/
/***********************/
void ChangeVideo()
{
    Uint32 flag = SDL_WINDOW_RESIZABLE;
    if (Pref.FullScreen) {
        flag = SDL_WINDOW_FULLSCREEN_DESKTOP;
    }
    SDL_SetWindowFullscreen(sdlWindow, flag);
    SDL_ShowCursor(0); // Cache le curseur
}

/*** SDL Main Menu principale ***/
/********************************/
eMenu Menu::SDLMain()
{
    int i;
    char MCode[5] = { 0, 0, 0, 0, 0 };
    char key;

    // Initialisations Divers
    m_mouse.Init(Menu_Py); // Initialise la sourie
    PyE = 0;

    // Prend les evenements
    do {
        SDL_RenderClear(sdlRenderer);
        // Prend l'image du fond et fait l'affichage
        Sprites[fond_menu].Affiche(400, 300, 0, Sprites[fmenu].Image[0]);
        Sprites[menu].Affiche(400, 340, 0, Sprites[fmenu].Image[0]);
        Sprites[titre].Affiche(400, 65, 0, Sprites[fmenu].Image[0]);
        Sprites[copyright].Affiche(400, 587, 0, Sprites[fmenu].Image[0]);

        AfficheText(400, 229, T_play, Sprites[fmenu].Image[0]);
        AddBouton(0, T_play, 400, 229);
        AfficheText(400, 306, T_scores, Sprites[fmenu].Image[0]);
        AddBouton(1, T_scores, 400, 306);
        AfficheText(400, 384, T_moptions, Sprites[fmenu].Image[0]);
        AddBouton(2, T_moptions, 400, 384);
        AfficheText(400, 461, T_quit, Sprites[fmenu].Image[0]);
        AddBouton(3, T_quit, 400, 461);
        Menu_Py[4].DepX = -1;
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            m_mouse.GetEvent(event, PyE); // Handle mouse
            m_gamepad.GetEvent(event); // Handle gamepad
            switch (event.type) {
            case SDL_KEYDOWN:
                if (event.key.state == SDL_PRESSED) {
                    m_audio.Play(sClic);
                    switch (event.key.keysym.sym) {
                    case SDLK_ESCAPE:
                    case SDLK_AC_BACK: // Android back button
                        return mQuit;
                    case SDLK_UP:
                        PyE--;
                        if (PyE < 0) {
                            PyE = 3;
                        }
                        break;
                    case SDLK_DOWN:
                        PyE++;
                        if (PyE > 3) {
                            PyE = 0;
                        }
                        break;
                    case SDLK_F12: // Save screenshot
                        if (event.key.repeat == 0) {
                            Utils::doScreenshot(sdlRenderer);
                        }
                        break;
                    case ' ':
                    case SDLK_RETURN:
                    case SDLK_KP_ENTER:
                        switch (PyE) {
                        case 0:
                            return mMenuSpeed;
                        case 1:
                            return mScore;
                        case 2:
                            return mOption;
                        case 3:
                            return mQuit;
                        }
                        break;
                    default:
                        key = event.key.keysym.sym & 0x7F; // Prend le caracataire correspondant à la touche
                        if (CharExiste(key) == true) { // Si la caractaire existe bien
                            for (i = 2; i >= 0; i--) {
                                MCode[i + 1] = MCode[i]; // décale le code
                            }
                            MCode[0] = key;
                            if (strcmp(MCode, "tide") == 0 || strcmp(MCode, "TIDE") == 0) {
                                return mEdit; // Si editeur de niveau
                            }
                        }
                    }
                }
                break;
            case SDL_QUIT:
                return mQuit;
                break;
            }
        }

        // Gère les variables
        HorlogeAvant = Horloge;
        Horloge = SDL_GetTicks();
        Sleeping();

        // Gère l'Affichage
        Ec.ClearSprite(fmenu);
        Print_Main();
        m_mouse.Print();

        // Echange les buffets video
        SDL_RenderPresent(sdlRenderer);

    } while (true);

    return mQuit;
}

/*** SDL Main Menu Choix de la langue ***/
/****************************************/
eMenu Menu::SDLMain_Language()
{
    int NCol = 1;
    int NL;
    int Ecart;
    int i;
    int x, y;
    int const OldLangue = Pref.Langue;

    // Initialisations Divers
    m_mouse.Init(Menu_Py); // Initialise la sourie
    PyE = Pref.Langue;
    if (PyE == -1) {
        PyE = 1;
    }

    SDL_RenderClear(sdlRenderer);
    // Prend l'image du fond et fait l'affichage
    Sprites[fond_menu].Affiche(400, 300, 0, Sprites[fmenu].Image[0]);

    // Affiche les langues possibles
    NCol = 3;
    if (Pref.NLangues % NCol == 0) {
        NL = Pref.NLangues / NCol;
    }
    else {
        NL = Pref.NLangues / NCol + 1;
    }
    Ecart = 600 / (NL + 1);

    for (i = 0; i < Pref.NLangues; i++) {
        x = (i / NL) * (800 / 3) + (800 / 6);
        y = (i % NL) * Ecart + Ecart;

        Sprites[T_Langue + i].Affiche(x, y, 0, Sprites[fmenu].Image[0]);
        AddBouton(i, (e_Sprite)(T_Langue + i), x, y);
    }

    Menu_Py[Pref.NLangues].DepX = -1;

    // Efface le fond
    SDL_RenderPresent(sdlRenderer);

    // Prend les evenements
    do {
        SDL_RenderClear(sdlRenderer);
        Sprites[fond_menu].Affiche(400, 300, 0, Sprites[fmenu].Image[0]);
        for (i = 0; i < Pref.NLangues; i++) {
            x = (i / NL) * (800 / 3) + (800 / 6);
            y = (i % NL) * Ecart + Ecart;

            Sprites[T_Langue + i].Affiche(x, y, 0, Sprites[fmenu].Image[0]);
            AddBouton(i, (e_Sprite)(T_Langue + i), x, y);
        }

        SDL_Event event;
        while (SDL_PollEvent(&event)) {

            m_mouse.GetEvent(event, PyE); // Handle mouse
            m_gamepad.GetEvent(event); // Handle gamepad
            switch (event.type) {
            case SDL_WINDOWEVENT:
                if (event.window.event == SDL_WINDOWEVENT_ENTER) {
                    SDL_RenderPresent(sdlRenderer);
                }
                break;
            case SDL_KEYDOWN:
                if (event.key.state == SDL_PRESSED) {
                    m_audio.Play(sClic);
                    switch (event.key.keysym.sym) {
                    case SDLK_ESCAPE:
                    case SDLK_AC_BACK: // Android back button
                        if (Pref.Langue == -1) {
                            Pref.Langue = PyE;
                        }
                        return mMenu;
                    case SDLK_UP:
                        PyE--;
                        if (PyE < 0) {
                            PyE = Pref.NLangues - 1;
                        }
                        break;
                    case SDLK_DOWN:
                        PyE++;
                        if (PyE >= Pref.NLangues) {
                            PyE = 0;
                        }
                        break;
                    case SDLK_LEFT:
                        if (PyE - NL >= 0) {
                            PyE -= NL;
                        }
                        break;
                    case SDLK_RIGHT:
                        if (PyE + NL < Pref.NLangues) {
                            PyE += NL;
                        }
                        break;
                    case SDLK_F12: // Save screenshot
                        if (event.key.repeat == 0) {
                            Utils::doScreenshot(sdlRenderer);
                        }
                        break;
                    case ' ':
                    case SDLK_RETURN:
                    case SDLK_KP_ENTER:
                        Pref.Langue = PyE;
                        if (Pref.Langue != OldLangue) {
                            LoadLangue();
                        }
                        return mMenu;
                    default:
                        break;
                    }
                }
                break;
            case SDL_QUIT:
                return mQuit;
                break;
            }
        }

        // Gère les variables
        HorlogeAvant = Horloge;
        Horloge = SDL_GetTicks();
        Sleeping();

        // Gère l'Affichage
        Ec.ClearSprite(fmenu); // Something is done in the Efface that prevents a crash (or maybe more computation)!
        Affiche_Main_Centre();

        m_mouse.Print();

        // Echange les buffets video
        SDL_RenderPresent(sdlRenderer);

    } while (true);

    return mQuit;
}

/*** SDL Main Menu Choix des Options ***/
/***************************************/
void Menu::InitMain_Options()
{
    // Initialisations Divers
    m_mouse.Init(Menu_Py); // Initialise la sourie
    // PyE=4;

    // Prend l'image du fond et fait l'affichage
    Sprites[fond_menu].Affiche(400, 300, 0, Sprites[fmenu].Image[0]);
    Sprites[gmenu].Affiche(400, 300, 0, Sprites[fmenu].Image[0]);
    Sprites[keys].Affiche(610, 455, 0, Sprites[fmenu].Image[0]);

    AddBouton(0, bruitage, 140, 110);
    AddBouton(1, music, 160, 200);

    AddBouton(2, fscreen, 190, 300);
    Menu_Py[2].DepX -= 40;
    Menu_Py[2].FinX = 625 + 40 + Sprites[fscreen].Dim[0].L / 2;
    Sprites[fscreen].Affiche(185, 300, 0, Sprites[fmenu].Image[0]);
    Sprites[window].Affiche(625, 300, 0, Sprites[fmenu].Image[0]);

    AddBouton(3, monde, 180, 400);

    // Centre à gauche le text de menu
    CentreM = 120 + Sprites[T_menu].Dim[0].L / 2;
    AfficheText(CentreM, 490, T_menu, Sprites[fmenu].Image[0]);
    AddBouton(4, T_menu, CentreM, 490);

    // Boutons des bruitages
    Sprites[fleches].Affiche(250, 110, 1, Sprites[fmenu].Image[0]);
    Sprites[fleches].Affiche(700, 110, 4, Sprites[fmenu].Image[0]);
    Menu_Py[5].DepX = 230;
    Menu_Py[5].DepY = 70;
    Menu_Py[5].FinX = 475;
    Menu_Py[5].FinY = 145;
    Menu_Py[5].Py = 5;
    Menu_Py[5].Valide = true;

    Menu_Py[6].DepX = 476;
    Menu_Py[6].DepY = 70;
    Menu_Py[6].FinX = 720;
    Menu_Py[6].FinY = 145;
    Menu_Py[6].Py = 6;
    Menu_Py[6].Valide = true;

    // Boutons de musics
    Sprites[fleches].Affiche(250, 200, 1, Sprites[fmenu].Image[0]);
    Sprites[fleches].Affiche(700, 200, 4, Sprites[fmenu].Image[0]);
    Menu_Py[7].DepX = 230;
    Menu_Py[7].DepY = 155;
    Menu_Py[7].FinX = 475;
    Menu_Py[7].FinY = 245;
    Menu_Py[7].Py = 7;
    Menu_Py[7].Valide = true;

    Menu_Py[8].DepX = 476;
    Menu_Py[8].DepY = 155;
    Menu_Py[8].FinX = 720;
    Menu_Py[8].FinY = 245;
    Menu_Py[8].Py = 8;
    Menu_Py[8].Valide = true;

    Menu_Py[9].DepX = -1;
}

/*** Gestion du menu Options ***/
/*******************************/
eMenu Menu::SDLMain_Options()
{
    int i, N;
    int NumSp;

    PyE = 4;
    // Prend les evenements
    do {
        Ec.CleanSpriteAndScreen(fmenu); // To not crash...
        SDL_RenderClear(sdlRenderer);
        InitMain_Options(); // Prépare le menu
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            m_mouse.GetEvent(event, PyE); // Handle mouse
            m_gamepad.GetEvent(event); // Handle gamepad

            switch (event.type) {
            case SDL_WINDOWEVENT:
                if (event.window.event == SDL_WINDOWEVENT_ENTER) {
                    SDL_RenderPresent(sdlRenderer);
                }
                break;
            case SDL_KEYDOWN:
                if (event.key.state == SDL_PRESSED) {
                    m_audio.Play(sClic);
                    switch (event.key.keysym.sym) {
                    case SDLK_ESCAPE:
                    case SDLK_AC_BACK: // Android back button
                        return mMenu;
                    case SDLK_LEFT:
                        switch (PyE) {
                        case 2:
                            if (Pref.FullScreen == false) {
                                Pref.FullScreen = true;
                                ChangeVideo();
                                PyE = 2;
                            }
                            break;
                        case 0:
                        case 5: // Diminue volume sons
                        case 6:
                            Pref.Volume -= SDL_MIX_MAXVOLUME / 10.0;
                            if (Pref.Volume < 0) {
                                Pref.Volume = 0;
                            }
                            m_audio.DoVolume();
                            m_audio.Play(sLive);
                            break;
                        case 1:
                        case 7: // Diminue volume music
                        case 8:
                            Pref.VolumeM -= SDL_MIX_MAXVOLUME / 10.0;
                            if (Pref.VolumeM < 0) {
                                Pref.VolumeM = 0;
                            }
                            m_audio.DoVolume();
                            break;
                        }
                        break;
                    case SDLK_RIGHT:
                        switch (PyE) {
                        case 2:
                            if (Pref.FullScreen == true) {
                                Pref.FullScreen = false;
                                ChangeVideo();
                                PyE = 2;
                            }
                            break;
                        case 0:
                        case 5:
                        case 6:
                            Pref.Volume += SDL_MIX_MAXVOLUME / 10.0;
                            if (Pref.Volume > SDL_MIX_MAXVOLUME) {
                                Pref.Volume = SDL_MIX_MAXVOLUME;
                            }
                            m_audio.DoVolume();
                            m_audio.Play(sLive);
                            break;
                        case 1:
                        case 7:
                        case 8:
                            Pref.VolumeM += SDL_MIX_MAXVOLUME / 10.0;
                            if (Pref.VolumeM > SDL_MIX_MAXVOLUME) {
                                Pref.VolumeM = SDL_MIX_MAXVOLUME;
                            }
                            m_audio.DoVolume();
                            break;
                        }
                        break;
                    case SDLK_UP:
                        PyE--;
                        if (PyE < 0) {
                            PyE = 4;
                        }
                        break;
                    case SDLK_DOWN:
                        PyE++;
                        if (PyE >= 5) {
                            PyE = 0;
                        }
                        break;
                    case SDLK_F12: // Save screenshot
                        if (event.key.repeat == 0) {
                            Utils::doScreenshot(sdlRenderer);
                        }
                        break;
                    case ' ':
                    case SDLK_RETURN:
                    case SDLK_KP_ENTER:
                        switch (PyE) {
                        case 0:
                        case 1:
                            break;
                        case 2: // Type d'affichage
                            Pref.FullScreen = (Pref.FullScreen + 1) & 1;
                            ChangeVideo();
                            PyE = 2;
                            break;
                        case 3: // Choix de la langue
                            SDLMain_Language();
                            PyE = 3;
                            break;
                        case 5: // Diminue volume sons
                            Pref.Volume -= SDL_MIX_MAXVOLUME / 10.0;
                            if (Pref.Volume < 0) {
                                Pref.Volume = 0;
                            }
                            m_audio.DoVolume();
                            m_audio.Play(sLive);
                            break;
                        case 6:
                            Pref.Volume += SDL_MIX_MAXVOLUME / 10.0;
                            if (Pref.Volume > SDL_MIX_MAXVOLUME) {
                                Pref.Volume = SDL_MIX_MAXVOLUME;
                            }
                            m_audio.DoVolume();
                            m_audio.Play(sLive);
                            break;
                        case 7: // Diminue volume music
                            Pref.VolumeM -= SDL_MIX_MAXVOLUME / 10.0;
                            if (Pref.VolumeM < 0) {
                                Pref.VolumeM = 0;
                            }
                            m_audio.DoVolume();
                            break;
                        case 8:
                            Pref.VolumeM += SDL_MIX_MAXVOLUME / 10.0;
                            if (Pref.VolumeM > SDL_MIX_MAXVOLUME) {
                                Pref.VolumeM = SDL_MIX_MAXVOLUME;
                            }
                            m_audio.DoVolume();
                            break;
                        default:
                            return mMenu;
                        }
                    default:
                        break;
                    }
                }
                break;
            case SDL_QUIT:
                return mQuit;
                break;
            }
        }

        // SDL_RenderClear(sdlRenderer);
        //  Gère les variables
        HorlogeAvant = Horloge;
        Horloge = SDL_GetTicks();
        Sleeping();

        // Gère l'Affichage
        // Ec.Efface(fmenu);

        if (Pref.FullScreen) {
            Ec.PrintSprite(fleches, 1, 350, 300);
            Ec.PrintSprite(fleches, 3, 450, 300);
        }
        else {
            Ec.PrintSprite(fleches, 0, 350, 300);
            Ec.PrintSprite(fleches, 4, 450, 300);
        }

        NumSp = (Horloge / 30) % 25;
        Ec.PrintSprite(bruitage, NumSp, 150, 110);
        NumSp = (Horloge / 30) % 25;
        Ec.PrintSprite(music, NumSp, 150, 200);
        NumSp = (Horloge / 50) % 50;
        Ec.PrintSprite(monde, NumSp, 180, 400);

        N = (int)(Pref.Volume * 10 + 1) / SDL_MIX_MAXVOLUME;
        NumSp = (Horloge / 50) % 40 + 120;
        for (i = 0; i < N; i++) {
            if (i == N - 1) {
                Ec.PrintSprite(locomotive, NumSp, (690 - 300) / 10 * i + 300, 110);
            }
            else {
                Ec.PrintSprite(buches, NumSp, (690 - 300) / 10 * i + 300, 110);
            }
        }

        N = (int)(Pref.VolumeM * 10 + 1) / SDL_MIX_MAXVOLUME;
        for (i = 0; i < N; i++) {
            if (i == N - 1) {
                Ec.PrintSprite(locomotive, NumSp, (690 - 300) / 10 * i + 300, 200);
            }
            else {
                Ec.PrintSprite(buches, NumSp, (690 - 300) / 10 * i + 300, 200);
            }
        }

        switch (PyE) {
        case 3:
            Print_Main(180);
            break;
        case 4:
            Print_Main(CentreM);
            break;
        case 5:
            PyE = 0;
            Print_Main();
            PyE = 5;
            break;
        case 6:
            PyE = 0;
            Print_Main();
            PyE = 6;
            break;
        case 7:
            PyE = 1;
            Print_Main();
            PyE = 7;
            break;
        case 8:
            PyE = 1;
            Print_Main();
            PyE = 8;
            break;
        default:
            Print_Main();
        }

        m_mouse.Print();

        // Echange les buffets video
        SDL_RenderPresent(sdlRenderer);

    } while (true);

    return mQuit;
}

/*** SDL Main Menu Choix de la difficulté ***/
/********************************************/
eMenu Menu::SDLMain_Speed()
{
    // Initialisations Divers
    m_mouse.Init(Menu_Py); // Initialise la sourie
    PyE = 1;

    // Prend les evenements
    do {
        Ec.CleanSpriteAndScreen(fmenu);
        SDL_RenderClear(sdlRenderer);
        // Prend l'image du fond et fait l'affichage
        Sprites[fond_menu].Affiche(400, 300, 0, Sprites[fmenu].Image[0]);
        Sprites[menu].Affiche(400, 340, 0, Sprites[fmenu].Image[0]);
        Sprites[titre].Affiche(400, 65, 0, Sprites[fmenu].Image[0]);

        AfficheText(400, 225, T_easy, Sprites[fmenu].Image[0]);
        AddBouton(0, T_easy, 400, 225);
        AfficheText(400, 340, T_normal, Sprites[fmenu].Image[0]);
        AddBouton(1, T_normal, 400, 340);
        AfficheText(400, 455, T_hard, Sprites[fmenu].Image[0]);
        AddBouton(2, T_hard, 400, 455);
        Menu_Py[3].DepX = -1;

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            m_mouse.GetEvent(event, PyE); // Handle mouse
            m_gamepad.GetEvent(event); // Handle gamepad

            switch (event.type) {
            case SDL_WINDOWEVENT:
                if (event.window.event == SDL_WINDOWEVENT_ENTER) {
                    SDL_RenderPresent(sdlRenderer);
                }
                break;
            case SDL_KEYDOWN:
                if (event.key.state == SDL_PRESSED) {
                    m_audio.Play(sClic);
                    switch (event.key.keysym.sym) {
                    case SDLK_ESCAPE:
                    case SDLK_AC_BACK: // Android back button
                        return mMenu;
                    case SDLK_UP:
                        PyE--;
                        if (PyE < 0) {
                            PyE = 2;
                        }
                        break;
                    case SDLK_DOWN:
                        PyE++;
                        if (PyE > 2) {
                            PyE = 0;
                        }
                        break;
                    case SDLK_F12: // Save screenshot
                        if (event.key.repeat == 0) {
                            Utils::doScreenshot(sdlRenderer);
                        }
                        break;
                    case ' ':
                    case SDLK_RETURN:
                    case SDLK_KP_ENTER:
                        switch (PyE) {
                        case 0:
                            Pref.Difficulte = Easy;
                            return Pref.NiveauMax[Pref.Difficulte] > 0 ? mMenuNiveau : mGame;
                        case 1:
                            Pref.Difficulte = Normal;
                            return Pref.NiveauMax[Pref.Difficulte] > 0 ? mMenuNiveau : mGame;
                        case 2:
                            Pref.Difficulte = Hard;
                            return Pref.NiveauMax[Pref.Difficulte] > 0 ? mMenuNiveau : mGame;
                        }
                        break;
                    default:
                        break;
                    }
                }
                break;
            case SDL_QUIT:
                return mQuit;
                break;
            }
        }

        // Gère les variables
        HorlogeAvant = Horloge;
        Horloge = SDL_GetTicks();
        Sleeping();

        // Gère l'Affichage
        Ec.ClearSprite(fmenu);
        Print_Main();
        m_mouse.Print();

        // Echange les buffets video
        SDL_RenderPresent(sdlRenderer);

    } while (true);

    return mQuit;
}

/*** SDL Main Menu Choix du niveau ***/
/*************************************/
eMenu Menu::SDLMain_Level()
{
    // Initialisations Divers
    m_mouse.Init(Menu_Py); // Initialise la sourie
    PyE = 0;
    Niv = Pref.NiveauMax[Pref.Difficulte];
    Pref.Niveau = 0;

    // Prend les evenements
    do {
        // Efface le fond
        SDL_RenderClear(sdlRenderer);
        // Prend l'image du fond et fait l'affichage
        Sprites[fond_menu].Affiche(400, 300, 0, Sprites[fmenu].Image[0]);
        Sprites[menu].Affiche(400, 340, 0, Sprites[fmenu].Image[0]);
        Sprites[titre].Affiche(400, 65, 0, Sprites[fmenu].Image[0]);

        AfficheText(400, 225, T_new_game, Sprites[fmenu].Image[0]);
        AddBouton(0, T_new_game, 400, 225);
        AfficheText(400, 320, T_old_level, Sprites[fmenu].Image[0]);
        AddBouton(1, T_old_level, 400, 320);
        AfficheText(400, 455, T_menu, Sprites[fmenu].Image[0]);
        AddBouton(2, T_menu, 400, 455);

        AddBouton(3, fleches, 330, 380);
        AddBouton(4, fleches, 470, 380);

        Menu_Py[5].DepX = -1;

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            m_mouse.GetEvent(event, PyE); // Handle mouse
            m_gamepad.GetEvent(event); // Handle gamepad

            switch (event.type) {
            case SDL_WINDOWEVENT:
                if (event.window.event == SDL_WINDOWEVENT_ENTER) {
                    SDL_RenderPresent(sdlRenderer);
                }
                break;
            case SDL_KEYDOWN:
                if (event.key.state == SDL_PRESSED) {
                    m_audio.Play(sClic);
                    switch (event.key.keysym.sym) {
                    case SDLK_ESCAPE:
                    case SDLK_AC_BACK: // Android back button
                        return mMenu;
                    case SDLK_UP:
                        PyE--;
                        if (PyE < 0 || PyE > 2) {
                            PyE = 2;
                        }
                        break;
                    case SDLK_DOWN:
                        PyE++;
                        if (PyE > 2) {
                            PyE = 0;
                        }
                        break;
                    case SDLK_LEFT:
                        if (Niv > 0) {
                            Niv--;
                        }
                        PyE = 1;
                        break;
                    case SDLK_RIGHT:
                        if (Niv < Pref.NiveauMax[Pref.Difficulte]) {
                            Niv++;
                        }
                        PyE = 1;
                        break;
                    case SDLK_F12: // Save screenshot
                        if (event.key.repeat == 0) {
                            Utils::doScreenshot(sdlRenderer);
                        }
                        break;
                    case ' ':
                    case SDLK_RETURN:
                    case SDLK_KP_ENTER:
                        switch (PyE) {
                        case 0:
                            return mGame;
                        case 1:
                            Pref.Niveau = Niv;
                            return mGame;
                        case 2:
                            return mMenu;
                        case 3:
                            if (Niv > 0) {
                                Niv--;
                            }
                            break;
                        case 4:
                            if (Niv < Pref.NiveauMax[Pref.Difficulte]) {
                                Niv++;
                            }
                            break;
                        }
                        break;
                    default:
                        break;
                    }
                }
                break;
            case SDL_QUIT:
                return mQuit;
                break;
            }
        }

        // Gère les variables
        HorlogeAvant = Horloge;
        Horloge = SDL_GetTicks();
        Sleeping();

        // Gère l'Affichage
        Ec.ClearSprite(fmenu);

        // Affiche les flèches
        if (Niv > 0) {
            if (PyE == 3) {
                Ec.PrintSprite(fleches, 2, 330, 380);
            }
            else {
                Ec.PrintSprite(fleches, 1, 330, 380);
            }
        }
        else {
            Ec.PrintSprite(fleches, 0, 330, 380);
        }

        if (Niv < Pref.NiveauMax[Pref.Difficulte]) {
            if (PyE == 4) {
                Ec.PrintSprite(fleches, 5, 470, 380);
            }
            else {
                Ec.PrintSprite(fleches, 4, 470, 380);
            }
        }
        else {
            Ec.PrintSprite(fleches, 3, 470, 380);
        }

        AfficheChiffre(400, 380, Niv + 1);

        if (PyE != 3 && PyE != 4) {
            Print_Main();
        }
        m_mouse.Print();

        // Echange les buffets video
        SDL_RenderPresent(sdlRenderer);

    } while (true);

    return mQuit;
}

/*** SDL Main questions sur les droits de l'homme ***/
/****************************************************/
#ifndef DCHILDREN
eMenu Menu::SDLMain_HR()
{
    int Fini = -1;
    int N1, N2, Ordre;
    SDL_Rect Position;

    // InitialisationsDivers
    m_mouse.Init(Menu_Py); // Initialise la sourie
    PyE = rand() % 2;

    // Choix de la question N1=reponse
    N1 = rand() % 30;
    do {
        N2 = rand() % 30;
    } while (N2 == N1);
    Ordre = rand() % 2; // Si 0=bonne ordre, 1=inverse

    // Prend l'image du fond et fait l'affichage
    Position.x = Position.y = 0;
    Position.w = Sprites[fmenu].Dim[0].L;
    Position.h = Sprites[fmenu].Dim[0].H;
    SDL_RenderCopy(sdlRenderer, Sprites[fmenu].Image[0], &Position, &Position);

    Sprites[menu].Affiche(340, 300, 0, Sprites[fmenu].Image[0]);
    Sprites[fond_hr].Affiche(340, 74, 0, Sprites[fmenu].Image[0]);
    AfficheText(338, 70, e_Sprite(T_question), Sprites[fmenu].Image[0]);

    Sprites[locomotive].Affiche(115, 110, rand() % 320, Sprites[fmenu].Image[0]);
    Sprites[deco].Affiche(100, 160 + (rand() % 130), rand() % 18, Sprites[fmenu].Image[0]);
    Sprites[deco].Affiche(100, 470 - (rand() % 130), rand() % 18, Sprites[fmenu].Image[0]);
    Sprites[deco].Affiche(580, 100 + (rand() % 130), rand() % 18, Sprites[fmenu].Image[0]);
    Sprites[deco].Affiche(580, 470 - (rand() % 130), rand() % 18, Sprites[fmenu].Image[0]);

    AfficheText(340, 300, e_Sprite(T_art1 + N1), Sprites[fmenu].Image[0]);

    if (Ordre) {
        AddBouton(0, e_Sprite(T_tart1 + N1), 240, 492);
        AddBouton(1, e_Sprite(T_tart1 + N2), 440, 492);
    }
    else {
        AddBouton(0, e_Sprite(T_tart1 + N1), 440, 492);
        AddBouton(1, e_Sprite(T_tart1 + N2), 240, 492);
    }
    Menu_Py[0].DepY -= 20;
    Menu_Py[0].FinY += 20;
    Menu_Py[1].DepY -= 20;
    Menu_Py[1].FinY += 20;

    Menu_Py[2].DepX = -1;

    std::array<std::pair<int, int>, 4> posDeco = { std::make_pair(rand() % 130, rand() % 18), std::make_pair(rand() % 130, rand() % 18), std::make_pair(rand() % 130, rand() % 18), std::make_pair(rand() % 130, rand() % 18) };
    int const locoPosition = rand() % 320;

    // Prend les evenements
    do {
        // Efface le fond
        SDL_RenderClear(sdlRenderer);

        SDL_RenderCopy(sdlRenderer, Sprites[fmenu].Image[0], &Position, &Position);

        // Sprites[fond].Affiche(400,300,0,Sprites[fjeu].Image[0]);
        Sprites[menu].Affiche(340, 300, 0, Sprites[fmenu].Image[0]);
        Sprites[fond_hr].Affiche(340, 74, 0, Sprites[fmenu].Image[0]);
        AfficheText(338, 70, e_Sprite(T_question), Sprites[fmenu].Image[0]);

        Sprites[locomotive].Affiche(115, 110, locoPosition, Sprites[fmenu].Image[0]);
        Sprites[deco].Affiche(100, 160 + posDeco[0].first, posDeco[0].second, Sprites[fmenu].Image[0]);
        Sprites[deco].Affiche(100, 470 - (posDeco[1].first), posDeco[1].second, Sprites[fmenu].Image[0]);
        Sprites[deco].Affiche(580, 100 + (posDeco[2].first), posDeco[2].second, Sprites[fmenu].Image[0]);
        Sprites[deco].Affiche(580, 470 - (posDeco[3].first), posDeco[3].second, Sprites[fmenu].Image[0]);

        AfficheText(340, 300, e_Sprite(T_art1 + N1), Sprites[fmenu].Image[0]);
        /*
        if(Ordre) {
          AddBouton(0,e_Sprite(T_tart1+N1),240,492);
          AddBouton(1,e_Sprite(T_tart1+N2),440,492);
        }
        else {
          AddBouton(0,e_Sprite(T_tart1+N1),440,492);
          AddBouton(1,e_Sprite(T_tart1+N2),240,492);
        }
        Menu_Py[0].DepY-=20;
        Menu_Py[0].FinY+=20;
        Menu_Py[1].DepY-=20;
        Menu_Py[1].FinY+=20;

        Menu_Py[2].DepX=-1;
      */
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            m_mouse.GetEvent(event, PyE); // Handle mouse
            m_gamepad.GetEvent(event); // Handle gamepad

            switch (event.type) {
            case SDL_WINDOWEVENT:
                if (event.window.event == SDL_WINDOWEVENT_ENTER) {
                }
                break;
            case SDL_KEYDOWN:
                if (Fini == -1 && event.key.state == SDL_PRESSED) {
                    m_audio.Play(sClic);
                    switch (event.key.keysym.sym) {
                    case SDLK_ESCAPE:
                    case SDLK_AC_BACK: // Android back button
                        return mGame;
                    case SDLK_UP:
                        PyE--;
                        if (PyE < 0) {
                            PyE = 1;
                        }
                        break;
                    case SDLK_DOWN:
                        PyE++;
                        if (PyE > 1) {
                            PyE = 0;
                        }
                        break;
                    case SDLK_LEFT:
                        if (Ordre) {
                            PyE = 0;
                        }
                        else {
                            PyE = 1;
                        }
                        break;
                    case SDLK_RIGHT:
                        if (Ordre) {
                            PyE = 1;
                        }
                        else {
                            PyE = 0;
                        }
                        break;
                    case SDLK_F12: // Save screenshot
                        if (event.key.repeat == 0) {
                            Utils::doScreenshot(sdlRenderer);
                        }
                        break;
                    case ' ':
                    case SDLK_RETURN:
                    case SDLK_KP_ENTER:
                        switch (PyE) {
                        case 0:
                            m_audio.Play(sEnd);
                            Pref.Score += 50;
                            Fini = Horloge + 2000;
                            break;
                        case 1:
                            m_audio.Play(sLose);
                            Fini = Horloge + 2000;
                            break;
                        }
                        break;
                    default:
                        break;
                    }
                }
                break;
            case SDL_QUIT:
                return mQuit;
                break;
            }
        }

        // Test si fini
        if (Fini != -1 && Fini < Horloge) {
            return mGame;
        }

        // Gère les variables
        HorlogeAvant = Horloge;
        Horloge = SDL_GetTicks();
        Sleeping();

        // Gère l'Affichage
        Ec.ClearSprite(fmenu);

        if (Ordre) {
            Ec.PrintSprite(fond_hrr, 0, 240, 492);
            AfficheText(240, 492, e_Sprite(T_tart1 + N1));
        }
        else {
            Ec.PrintSprite(fond_hrr, 0, 440, 492);
            AfficheText(440, 492, e_Sprite(T_tart1 + N1));
        }

        if (Fini == -1) {
            if (Ordre) {
                Ec.PrintSprite(fond_hrr, 0, 440, 492);
                AfficheText(440, 492, e_Sprite(T_tart1 + N2));
                if (PyE == 0) {
                    Print_Main(240);
                }
                else {
                    Print_Main(440);
                }
            }
            else {
                Ec.PrintSprite(fond_hrr, 0, 240, 492);
                AfficheText(240, 492, e_Sprite(T_tart1 + N2));
                if (PyE == 1) {
                    Print_Main(240);
                }
                else {
                    Print_Main(440);
                }
            }
            m_mouse.Print();
        }

        // Echange les buffets video
        SDL_RenderPresent(sdlRenderer);

    } while (true);

    return mQuit;
}
#endif

/*** SDL Main dans la partie ***/
/*******************************/
void Menu::Print_InGame()
{
    SDL_Rect Position;

    // InitialisationsDivers
    m_mouse.Init(Menu_Py); // Initialise la sourie
    // PyE=0;

    // Prend l'image du fond et fait l'affichage
    Position.x = Position.y = 0;
    Position.w = Sprites[fmenu].Dim[0].L;
    Position.h = Sprites[fmenu].Dim[0].H;
    SDL_RenderCopy(sdlRenderer, Sprites[fmenu].Image[0], &Position, &Position);

    Sprites[menu].Affiche(340, 300, 0, Sprites[fmenu].Image[0]);

    AfficheText(340, 185, T_continue, Sprites[fmenu].Image[0]);
    AddBouton(0, T_continue, 340, 185);
    AfficheText(340, 300, T_moptions, Sprites[fmenu].Image[0]);
    AddBouton(1, T_moptions, 340, 300);
    AfficheText(340, 415, T_exit_game, Sprites[fmenu].Image[0]);
    AddBouton(2, T_exit_game, 340, 415);
    Menu_Py[3].DepX = -1;
}

/*** Gestion du menu dans le jeu ***/
/***********************************/
eMenu Menu::SDLMain_InGame()
{
    Print_InGame();

    // Prend les evenements
    do {
        SDL_RenderClear(sdlRenderer);
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            m_mouse.GetEvent(event, PyE); // Handle mouse
            m_gamepad.GetEvent(event); // Handle gamepad

            switch (event.type) {
            case SDL_WINDOWEVENT:
                if (event.window.event == SDL_WINDOWEVENT_ENTER) {
                    // SDL_RenderPresent(sdlRenderer);
                    // Ec.Cls(fmenu);
                }
                break;
            case SDL_KEYDOWN:
                if (event.key.state == SDL_PRESSED) {
                    m_audio.Play(sClic);
                    switch (event.key.keysym.sym) {
                    case SDLK_ESCAPE:
                    case SDLK_AC_BACK: // Android back button
                        return mGame;
                    case SDLK_UP:
                        PyE--;
                        if (PyE < 0) {
                            PyE = 2;
                        }
                        break;
                    case SDLK_DOWN:
                        PyE++;
                        if (PyE > 2) {
                            PyE = 0;
                        }
                        break;
                    case SDLK_F12: // Save screenshot
                        if (event.key.repeat == 0) {
                            Utils::doScreenshot(sdlRenderer);
                        }
                        break;
                    case ' ':
                    case SDLK_RETURN:
                    case SDLK_KP_ENTER:
                        switch (PyE) {
                        case 0:
                            return mGame;
                        case 1:
                            SDLMain_Options();
                            PyE = 1;
                            break;
                        case 2:
                            return mScoreEdit;
                        }
                        break;
                    default:
                        break;
                    }
                }
                break;
            case SDL_QUIT:
                return mQuit;
                break;
            }
        }

        // Gère les variables
        HorlogeAvant = Horloge;
        Horloge = SDL_GetTicks();
        Sleeping();

        // Gère l'Affichage
        Ec.ClearSprite(fmenu);
        Print_InGame();
        Print_Main(340);
        m_mouse.Print();

        // Echange les buffets video
        SDL_RenderPresent(sdlRenderer);

    } while (true);

    return mQuit;
}

/*** SDL Main Menu Choix de la difficulté ***/
/********************************************/
eMenu Menu::SDLMain_Score(bool EditScore)
{
    int i;
    int NEdit = -1;
    char Provi[256];
    int PosCur = 0;
    char key;

    // Cherche le numéro du score à remplacer si edition des scores
    if (EditScore) {
        for (i = 7; i >= 0; i--) {
            if (Pref.Sco[i].Score < Pref.Score) {
                NEdit = i;
            }
        }
        if (NEdit == -1) {
            return mMenu;
        }

        if (NEdit < 7) { // Si doit fair un décalage
            for (i = 7; i > NEdit; i--) {
                Pref.Sco[i].Score = Pref.Sco[i - 1].Score;
                strcpy(Pref.Sco[i].Name, Pref.Sco[i - 1].Name);
            }
        }

        // Efface le nouveau nom et met le score
        Pref.Sco[NEdit].Score = Pref.Score;
        Pref.Sco[NEdit].Name[0] = 0;
    }

    // Met la sourie sur tous l'ecran
    m_mouse.Init(Menu_Py); // Initialise la sourie
    Menu_Py[0].DepX = 0;
    Menu_Py[0].DepY = 0;
    Menu_Py[0].FinX = 800;
    Menu_Py[0].FinY = 600;
    Menu_Py[0].Py = 0;
    Menu_Py[0].Valide = true;
    Menu_Py[1].DepX = -1;

    if (EditScore) {
        SDL_StartTextInput();
    }

    // Prend les evenements
    do {
        // Efface le fond
        Ec.CleanSpriteAndScreen(fmenu);
        SDL_RenderClear(sdlRenderer);
        // Prend l'image du fond et fait l'affichage
        Sprites[fond_menu].Affiche(400, 300, 0, Sprites[fmenu].Image[0]);

        // Affiche le titre et les commandes
        AfficheText(400, 50, T_better_scores, Sprites[fmenu].Image[0]);
        AfficheText(400, 550, T_press_any_key, Sprites[fmenu].Image[0]);

        // Affiche les scores
        for (i = 0; i < 8; i++) {
            sprintf(Provi, "%d", i + 1);
            AfficheString(70, 120 + i * (360 / 7), Provi, Sprites[fmenu].Image[0]);

            if (EditScore == false || NEdit != i) {
                if (Pref.Sco[i].Name[0]) {
                    AfficheString(140, 120 + i * (360 / 7), Pref.Sco[i].Name, Sprites[fmenu].Image[0]);
                }
                else {
                    AfficheString(140, 120 + i * (360 / 7), Points, Sprites[fmenu].Image[0]);
                }
            }

            sprintf(Provi, "%i", Pref.Sco[i].Score);
            AfficheString(740 - LongueurString(Provi), 120 + i * (360 / 7), Provi, Sprites[fmenu].Image[0]);
        }

        // Efface le fond
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            m_mouse.GetEvent(event, PyE); // Handle mouse
            m_gamepad.GetEvent(event); // Handle gamepad

            switch (event.type) {
            case SDL_WINDOWEVENT:
                if (event.window.event == SDL_WINDOWEVENT_ENTER) {
                    SDL_RenderPresent(sdlRenderer);
                }
                break;
            case SDL_KEYDOWN: // Prend un touche au clavier
                if (event.key.state == SDL_PRESSED) {
                    m_audio.Play(sClic);
                    if (EditScore == false && event.key.keysym.sym != SDLK_F12) {
                        event.key.keysym.sym = SDLK_RETURN;
                    }
                    switch (event.key.keysym.sym) {
                    case SDLK_F12: // Save screenshot
                        if (event.key.repeat == 0) {
                            Utils::doScreenshot(sdlRenderer);
                        }
                        break;
                    case SDLK_ESCAPE: // Valide l'entrée
                    case SDLK_RETURN:
                    case SDLK_KP_ENTER:
                        if (EditScore) {
                            SDL_StopTextInput();
                        }
                        return mMenu;
                    case SDLK_BACKSPACE: // Fait un retour de chariot
                        if (PosCur) {
                            PosCur--;
                            Pref.Sco[NEdit].Name[PosCur] = 0;
                        }
                        break;
                    default:
                        break;
                    }
                }
                break;
            case SDL_TEXTINPUT:
                /* Add new text onto the end of our text */
                if (LongueurString(Pref.Sco[NEdit].Name) < LSCOREMAX && PosCur < 79 && CharExiste(event.text.text[0])) {
                    PosCur += strlen(event.text.text);
                    strcat(Pref.Sco[NEdit].Name, event.text.text);
                }
                break;
            case SDL_QUIT:
                if (EditScore) {
                    SDL_StopTextInput();
                }
                return mQuit;
            }
        }

        // Gère les variables
        HorlogeAvant = Horloge;
        Horloge = SDL_GetTicks();
        Sleeping();

        if (EditScore) { // Gère l'affiche pour l'édition des scores
            AfficheString(140, 120 + NEdit * (360 / 7), Pref.Sco[NEdit].Name);

            i = (Horloge / 50) % 20; // Affiche les curseurs
            Ec.PrintSprite(fleche_gauche, i, 110, 120 + NEdit * (360 / 7));
            Ec.PrintSprite(fleche_droite, i, 180 + LongueurString(Pref.Sco[NEdit].Name), 120 + NEdit * (360 / 7));
        }

        // Echange les buffets video
        SDL_RenderPresent(sdlRenderer);

    } while (true);

    if (EditScore) {
        SDL_StopTextInput();
    }
    return mQuit;
}

/*** Affiche le menu Principale ***/
/**********************************/
void Menu::Print_Main(int Centre) const
{
    int const NumSp = (Horloge / 50) % 20;
    int const x1 = Menu_Py[PyE].DepX - 25;
    int const x2 = (Centre - x1) + Centre;
    int const y = (Menu_Py[PyE].FinY + Menu_Py[PyE].DepY) / 2;

    Ec.PrintSprite(fleche_gauche, NumSp, x1, y);
    Ec.PrintSprite(fleche_droite, NumSp, x2, y);
}

/*** Centre les flèches sur le boutton ***/
/*****************************************/
void Menu::Affiche_Main_Centre() const
{
    int const NumSp = (Horloge / 50) % 20;
    int const x1 = Menu_Py[PyE].DepX - 5;
    int const x2 = Menu_Py[PyE].FinX + 5;
    int const y = (Menu_Py[PyE].FinY + Menu_Py[PyE].DepY) / 2;

    Ec.PrintSprite(fleche_gauche, NumSp, x1, y);
    Ec.PrintSprite(fleche_droite, NumSp, x2, y);
}
