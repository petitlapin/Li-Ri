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

#include <SDL2/SDL_events.h> // for SDL_PollEvent, SDL_PRESSED, SDL_Event
#include <SDL2/SDL_keycode.h> // for SDLK_DOWN, SDLK_LEFT, SDLK_RIGHT, SDLK_UP
#include <SDL2/SDL_render.h> // for SDL_RenderPresent, SDL_RenderClear
#include <SDL2/SDL_timer.h> // for SDL_GetTicks
#include <SDL2/SDL_video.h> // for SDL_WINDOWEVENT_ENTER, SDL_WINDOWEVENT...
#include <cstdlib>

#include "game.h"
#include "gamepad.h"
#include "screen.h"
#include "menu.h"
#include "sprite.h"
#include "level.h"
#include "utils.h"
#include "audio.h"

/*** Global varibles ***/
/**************************/
extern SDL_Renderer *sdlRenderer;

extern Sprite *Sprites;
extern sNewPreference Pref;

extern int Horloge;
extern int HorlogeAvant;

extern Screen Ec;

extern Level level;

static int NumRail[] = { -1, -1, -1, 0, -1, 1, 2, 3, -1, 4, 5, 6, 7, 8, 9, 10 };

int MasqueK; // Mask for the movement keys

/***  Constuctor and destructor  ***/
/***********************************/
Game::Game(Audio &sounds, Gamepad &gamepad) :
    m_sounds(sounds), m_gamepad(gamepad), Lo(m_sounds)
{
    Touche[0] = D_Top;
    Touche[1] = D_Bottom;
    Touche[2] = D_Left;
    Touche[3] = D_Right;
}

/*** SDL Main ***/
/****************/
eMenu Game::SDLMain()
{
    eMenu mRet;
    int NumN = Pref.Niveau;

    Help = true;
    Load(NumN); // Load the table
    SDL_RenderPresent(sdlRenderer);
    Ec.CleanSpriteAndScreen(fjeu);
    Pause = true;

    Horloge = SDL_GetTicks(); // Clock
    DureeJeu = 0;
    Key = 0;

    // Set the player's starting options
    Pref.NVie = N_VIES_DEP;
    Pref.Score = 0;

    // Event listener
    do {
        bool doScreenshot = false;
        SDL_RenderClear(sdlRenderer);
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            m_gamepad.GetEvent(event); // Handle gamepad

            switch (event.type) {
            case SDL_WINDOWEVENT:
                if (event.window.event == SDL_WINDOWEVENT_ENTER) {
                }
                else if (event.window.event == SDL_WINDOWEVENT_LEAVE) { // If you turn off the screen
                    Pause = true; // Enable pause
                }
                break;
            case SDL_MOUSEBUTTONDOWN:
                if (event.button.state == SDL_PRESSED) {
                    // Add a Pause event if we click on the top right loco
                    int const Px = event.button.x;
                    int const Py = event.button.y;
                    if (Px >= 680 && Py <= 90) {
                        mRet = m_menu->SDLMain_InGame();
                        if (mRet == mGame) {
                            DrawLevel(NumN);
                            SDL_RenderPresent(sdlRenderer);
                            Pause = true;
                        }
                        else {
                            return mRet;
                        }
                    }
                    else if (Pause) {
                        Pause = false;
                    }
                    else {
                        TourneFleche();
                    }
                }
                break;
            case SDL_KEYDOWN:
                if (event.key.state == SDL_PRESSED) {
                    if (event.key.keysym.sym == SDLK_ESCAPE || event.key.keysym.sym == SDLK_AC_BACK) {
                        mRet = m_menu->SDLMain_InGame();
                        if (mRet == mGame) {
                            DrawLevel(NumN);
                            SDL_RenderPresent(sdlRenderer);
                            Pause = true;
                        }
                        else {
                            return mRet;
                        }
                    }
                    else {
                        Key = event.key.keysym.sym;

                        if (event.key.repeat == 0) {
                            // The text says: "press any key to start"
                            // so we allow any key press (except Escape)
                            // to start the game
                            if (Pause) {
                                Pause = false;
                                break;
                            }
                            switch (Key) {
                            case SDLK_UP:
                                BufTouche(D_Top);
                                MasqueK |= 1;
                                break;
                            case SDLK_DOWN:
                                BufTouche(D_Bottom);
                                MasqueK |= 2;
                                break;
                            case SDLK_LEFT:
                                BufTouche(D_Left);
                                MasqueK |= 4;
                                break;
                            case SDLK_RIGHT:
                                BufTouche(D_Right);
                                MasqueK |= 8;
                                break;
                            case SDLK_F12: // Save screenshot
                                doScreenshot = true;
                                break;
                            case 'p':
                            case 'P':
                                if (Pause == false) {
                                    Pause = true;
                                }
                                break;
                            case ' ':
                                if (Pause) {
                                    Pause = false;
                                }
                                break;
                            case 'h':
                            case 'H':
                                Help = !Help;
                                break;
                            }
                        }
                    }
                    break;
                case SDL_KEYUP:
                    switch (event.key.keysym.sym) {
                    case SDLK_UP:
                        MasqueK &= 14;
                        break;
                    case SDLK_DOWN:
                        MasqueK &= 13;
                        break;
                    case SDLK_LEFT:
                        MasqueK &= 11;
                        break;
                    case SDLK_RIGHT:
                        MasqueK &= 7;
                        break;
                    default:
                        break;
                    }
                    if (!MasqueK) {
                        Key = 0;
                    }
                    break;
                case SDL_QUIT:
                    return mQuit;
                    break;
                }
            }
        }

        // Manages the clocks and the installation
        HorlogeAvant = Horloge;
        Horloge = SDL_GetTicks();
        Sleeping();
        if (Pause == true || Lo.Mort > Horloge) {
            HorlogeAvant = Horloge;
        }
        DureeJeu += Horloge - HorlogeAvant;

        // Display
        DrawLevel(NumN);
        AfficheEcran();
        SDL_RenderPresent(sdlRenderer);

        if (doScreenshot) {
            Utils::doScreenshot(sdlRenderer);
        }

        // Move the locomotive forward
        if (Lo.Mort == -1 && Pause == false) {
            Lo.Avance(Horloge - HorlogeAvant, DureeJeu, Touche, T);
        }

        // Test the end of a game
        if (Lo.Mort > -1 && Lo.Mort < Horloge) { // If it's dead, test to see if it should continue or quit.
            if (Pref.NVie < 0) {
                return mScoreEdit; // If death ends
            }
            if (Lo.Gagne) {
#ifndef DCHILDREN
                if (Pref.HumanRightsQuiz && m_menu->SDLMain_HR() == mQuit) {
                    return mQuit;
                }
#endif
                NumN++;
                if (level.N == NumN) {
                    Pref.Score += Pref.NVie * 100;
                    return mScoreEdit;
                }
                if (Pref.NiveauMax[Pref.Difficulte] < NumN) {
                    Pref.NiveauMax[Pref.Difficulte] = NumN;
                }
            }
            m_sounds.NextMusic();
            DureeJeu = 0;
            Pause = true;
            Key = MasqueK = 0;
            Load(NumN);
        }

    } while (true);

    return mQuit;
}

/***   Load a table    ***/
/*************************/
bool Game::Load(int NivN)
{
    int i;

    Pref.Niveau = NivN;

    // Copy the table
    for (i = 0; i < LT * HT; i++) {
        T[i] = (int)level.T[NivN].T[i];
    }

    // Leave or erase life depending on the level
    switch (Pref.Difficulte) {
    case Easy:
        i = 5;
        break;
    case Normal:
        i = 9;
        break;
    default:
        i = 15;
    }
    if ((rand() % i) != 0) {
        for (i = 0; i < LT * HT; i++) {
            if (T[i] == C_Live) {
                T[i] = C_Rail;
            }
        }
    }

    // Initialisate locomotive
    Lo.Init(level.T[NivN].DepX + level.T[NivN].DepY * LT, level.T[NivN].DepDir);
    BufTouche(level.T[NivN].DepDir);
    MasqueK = 0;

    // Set the speed according to difficulty
    switch (Pref.Difficulte) {
    case Easy:
        Pref.Vitesse = Pref.VitesseMoy = VITESSE_MIN;
        break;
    case Hard:
        Pref.Vitesse = Pref.VitesseMoy = VITESSE_MAX;
        break;
    default:
        Pref.Vitesse = Pref.VitesseMoy = VITESSE_MOY;
    }

    return DrawLevel(NivN);
}

/***      Draw background of game      ***/
/*****************************************/
bool Game::DrawLevel(int NivN)
{
    int i, x, y, m, cx, cy;

    // Create the game's background
    Sprites[fond].Affiche(400, 300, 0, Sprites[fjeu].Image[0]);

    // Draw the circuit
    for (i = 0; i < LT * HT; i++) {
        if (T[i] >= C_Rail && T[i] < C_Fin) {
            y = i / LT * D_Case + D_Case / 2;
            x = i % LT * D_Case + D_Case / 2;

            m = 0;
            cx = i % LT;
            cy = i / LT;
            if (cy > 0 && T[i - LT] >= C_Rail && T[i - LT] < C_Fin) {
                m += 8;
            }
            if (cy < HT - 1 && T[i + LT] >= C_Rail && T[i + LT] < C_Fin) {
                m += 4;
            }
            if (cx > 0 && T[i - 1] >= C_Rail && T[i - 1] <= C_Fin) {
                m += 2;
            }
            if (cx < LT - 1 && T[i + 1] >= C_Rail && T[i + 1] <= C_Fin) {
                m += 1;
            }

            Sprites[rail].Affiche(x, y, NumRail[m], Sprites[fjeu].Image[0]);
        }
    }

    // Draw decorations
#ifndef DCHILDREN
    for (i = 0; i < level.T[NivN].NDeco; i++) {
        Sprites[deco].Affiche(level.T[NivN].Deco[i].x, level.T[NivN].Deco[i].y, level.T[NivN].Deco[i].NumSpr,
                              Sprites[fjeu].Image[0]);
    }
#endif

    // Displays texts according to language
    AfficheText(740, 110, T_level, Sprites[fjeu].Image[0]);
    AfficheText(740, 180, T_score, Sprites[fjeu].Image[0]);
    AfficheText(740, 260, T_options, Sprites[fjeu].Image[0]);
    AfficheText(740, 340, T_lives, Sprites[fjeu].Image[0]);

    AfficheChiffre(740, 140, Pref.Niveau + 1, Sprites[fjeu].Image[0]);

    return true;
}

/*** Rotates the arrow with a single touch ***/
/*********************************************/
void Game::TourneFleche()
{
    int To = Touche[0];
    bool Cherche = false;
    int const x = Lo.PInter % LT;
    int const y = Lo.PInter / LT;

    do {
        // Makes the direction touch in a clockwise direction
        // and test if the new direction is possible
        switch (To) {
        case D_Top:
            To = D_Right;
            if (Lo.PEntree != D_Right && x + 1 < LT && T[Lo.PInter + 1] != 0) {
                Cherche = true;
            }
            break;
        case D_Right:
            To = D_Bottom;
            if (Lo.PEntree != D_Bottom && y + 1 < HT && T[Lo.PInter + LT] != 0) {
                Cherche = true;
            }
            break;
        case D_Bottom:
            To = D_Left;
            if (Lo.PEntree != D_Left && x > 0 && T[Lo.PInter - 1] != 0) {
                Cherche = true;
            }
            break;
        case D_Left:
            To = D_Top;
            if (Lo.PEntree != D_Top && y > 0 && T[Lo.PInter - LT] != 0) {
                Cherche = true;
            }
            break;
        }
    } while (Cherche == false);

    BufTouche(To); // Remembers the new default key.
}

/***           Stores a key in the keypad           ***/
/******************************************************/
void Game::BufTouche(int Tou)
{
    int n = 0;

    // Promotes the touch
    while (Touche[n] != Tou) {
        n++; // Takes position of the key
    }

    if (n) { // If change is going to cause a shift
        while (n) {
            Touche[n] = Touche[n - 1];
            n--;
        }
        Touche[0] = Tou; // Remember key
    }

    // Looking for its opposite
    switch (Tou) {
    case D_Top:
        Tou = D_Bottom;
        break;
    case D_Bottom:
        Tou = D_Top;
        break;
    case D_Left:
        Tou = D_Right;
        break;
    case D_Right:
        Tou = D_Left;
        break;
    }

    // Disadvantages its opposite.
    n = 3;
    while (Touche[n] != Tou) {
        n--; // Takes position of the key
    }

    if (n < 3) { // If change is going to cause a shift
        while (n < 3) {
            Touche[n] = Touche[n + 1];
            n++;
        }
        Touche[3] = Tou; // Remember key
    }
}

/***   Test the possible directions for the arrows  ***/
/******************************************************/
int Game::TestFleche(int Haut, int Bas, int Gauche, int Droite)
{
    int i;
    int const x = Lo.PInter % LT;
    int const y = Lo.PInter / LT;

    for (i = 0; i < 4; i++) {
        switch (Touche[i]) {
        case D_Top:
            if (y > 0 && Haut != -1 && T[Lo.PInter - LT] != 0) {
                return Haut;
            }
            break;
        case D_Bottom:
            if (y + 1 < HT && Bas != -1 && T[Lo.PInter + LT] != 0) {
                return Bas;
            }
            break;
        case D_Left:
            if (x > 0 && Gauche != -1 && T[Lo.PInter - 1] != 0) {
                return Gauche;
            }
            break;
        case D_Right:
            if (x + 1 < LT && Droite != -1 && T[Lo.PInter + 1] != 0) {
                return Droite;
            }
            break;
        }
    }
    return 0;
}

/*** Render a screenshot of the game ***/
/***************************************/
void Game::AfficheEcran()
{
    int i;
    int ndir = 0;

    // Prepare for new display
    Ec.ClearSprite(fjeu);

    Lo.Display(Ec); // Render locomotive

    if (Lo.PInter != -1 && Help) { // Displays the arrow on the future intersection
        switch (Lo.PEntree) {
        case D_Left:
            ndir = TestFleche(0, 2, -1, 1);
            break;
        case D_Right:
            ndir = TestFleche(8, 6, 7, -1);
            break;
        case D_Top:
            ndir = TestFleche(-1, 4, 5, 3);
            break;
        case D_Bottom:
            ndir = TestFleche(10, -1, 9, 11);
            break;
        }

        Ec.PrintSprite(dir, ndir, (Lo.PInter % LT) * D_Case + D_Case / 2, (Lo.PInter / LT) * D_Case + D_Case / 2);
    }

    // Render options section
    for (i = 0; i < LT * HT; i++) {
        switch (T[i]) {
        case C_Wagon: // If wagon
            Ec.PrintSprite(wagon, (DureeJeu * 40 / 1000 + i * 7) % 50, i % LT * D_Case + D_Case / 2, i / LT * D_Case + D_Case / 2);
            break;
        case C_Allonge: // If long bonus
            Ec.PrintSprite(pluslong, (DureeJeu * 40 / 1000 + i * 7) % 50, i % LT * D_Case + D_Case / 2, i / LT * D_Case + D_Case / 2);
            break;
        case C_Reduit: // If short bonus
            Ec.PrintSprite(pluscourt, (DureeJeu * 40 / 1000 + i * 7) % 50, i % LT * D_Case + D_Case / 2, i / LT * D_Case + D_Case / 2);
            break;
        case C_Speed: // If speed bonus
            Ec.PrintSprite(vitesse, (DureeJeu * 40 / 1000 + i * 7) % 50, i % LT * D_Case + D_Case / 2, i / LT * D_Case + D_Case / 2);
            break;
        case C_Live: // If live bonus
            Ec.PrintSprite(vie, (DureeJeu * 40 / 1000 + i * 7) % 50, i % LT * D_Case + D_Case / 2, i / LT * D_Case + D_Case / 2);
            break;
        }
    }

    if (Pause) {
        Ec.PrintText(T_press_any_key, LT * D_Case / 2, 300);
    }

    // Render bonuses on board
    Ec.PrintOptions(Pref.NVie, Pref.Score);
    if (Pref.EcartWagon < ECARTWAGON_MOY) {
        Ec.PrintSprite(pluscourt, (DureeJeu * 40 / 1000) % 50, 715, 295);
    }
    if (Pref.EcartWagon > ECARTWAGON_MOY) {
        Ec.PrintSprite(pluslong, (DureeJeu * 40 / 1000) % 50, 715, 295);
    }
    if (Pref.VitesseMoy > Pref.Vitesse) {
        Ec.PrintSprite(vitesse, (DureeJeu * 40 / 1000 + 7) % 50, 765, 295);
    }
}
