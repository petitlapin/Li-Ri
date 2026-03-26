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

extern SDL_Renderer *sdlRenderer;

extern Sprite *Sprites;
extern sNewPreference Pref;

extern int currentTime;
extern int previousTime;

static int NumRail[] = { -1, -1, -1, 0, -1, 1, 2, 3, -1, 4, 5, 6, 7, 8, 9, 10 };

int MaskK; // Mask for movement keys

Game::Game(Audio &sounds, Screen &screen, Level &level, Gamepad &gamepad) :
    m_sounds(sounds), m_screen(screen), m_level(level), m_gamepad(gamepad), Lo(m_sounds)
{
    KeyPress[0] = D_Top;
    KeyPress[1] = D_Bottom;
    KeyPress[2] = D_Left;
    KeyPress[3] = D_Right;
}

eMenu Game::SDLMain()
{
    eMenu mRet;
    int NumN = Pref.Level;

    Help = true;
    Load(NumN); // Loads map/level
    SDL_RenderPresent(sdlRenderer);
    m_screen.CleanSpriteAndScreen(fgame);
    Pause = true;

    currentTime = SDL_GetTicks(); // get Clock time
    GameClock = 0;
    Key = 0;

    // Set initial options for the player
    Pref.Lives = N_LIVES_COUNT;
    Pref.Score = 0;

    // Event Handling
    do {
        bool doScreenshot = false;
        SDL_RenderClear(sdlRenderer);
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            m_gamepad.GetEvent(event);

            switch (event.type) {
            case SDL_WINDOWEVENT:
                if (event.window.event == SDL_WINDOWEVENT_ENTER) {
                }
                else if (event.window.event == SDL_WINDOWEVENT_LEAVE) { // If window is inactive
                    Pause = true; // Pause game
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
                        TurnArrow();
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
                                BufKeyPress(D_Top);
                                MaskK |= 1;
                                break;
                            case SDLK_DOWN:
                                BufKeyPress(D_Bottom);
                                MaskK |= 2;
                                break;
                            case SDLK_LEFT:
                                BufKeyPress(D_Left);
                                MaskK |= 4;
                                break;
                            case SDLK_RIGHT:
                                BufKeyPress(D_Right);
                                MaskK |= 8;
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
                        MaskK &= 14;
                        break;
                    case SDLK_DOWN:
                        MaskK &= 13;
                        break;
                    case SDLK_LEFT:
                        MaskK &= 11;
                        break;
                    case SDLK_RIGHT:
                        MaskK &= 7;
                        break;
                    default:
                        break;
                    }
                    if (!MaskK) {
                        Key = 0;
                    }
                    break;
                case SDL_QUIT:
                    return mQuit;
                    break;
                }
            }
        }

        // Handling pause and clocks
        previousTime = currentTime;
        currentTime = SDL_GetTicks();
        Sleeping();
        if (Pause == true || Lo.Dead > currentTime) {
            previousTime = currentTime;
        }
        GameClock += currentTime - previousTime;

        // Handles display
        DrawLevel(NumN);
        DisplayScreen();
        SDL_RenderPresent(sdlRenderer);

        if (doScreenshot) {
            Utils::doScreenshot(sdlRenderer);
        }

        // Locomotive moves forward
        if (Lo.Dead == -1 && Pause == false) {
            Lo.MoveForward(currentTime - previousTime, GameClock, KeyPress, T);
        }

        // Checks if the game ended (Death or timeout)
        if (Lo.Dead > -1 && Lo.Dead < currentTime) { // If dead, check for remaining lives
            if (Pref.Lives < 0) {
                return mScoreEdit; // Game ends
            }
            if (Lo.Win) {
#ifndef DCHILDREN
                if (Pref.HumanRightsQuiz && m_menu->SDLMain_HR() == mQuit) {
                    return mQuit;
                }
#endif
                NumN++;
                if (m_level.N == NumN) {
                    Pref.Score += Pref.Lives * 100;
                    return mScoreEdit;
                }
                if (Pref.LevelMax[Pref.Difficulty] < NumN) {
                    Pref.LevelMax[Pref.Difficulty] = NumN;
                }
            }
            m_sounds.NextMusic();
            GameClock = 0;
            Pause = true;
            Key = MaskK = 0;
            Load(NumN);
        }

    } while (true);

    return mQuit;
}

/*** Loads a level/map ***/
/*************************/
bool Game::Load(int LevelN)
{
    int i;

    Pref.Level = LevelN;

    // Copy wanted level to current
    for (i = 0; i < LT * HT; i++) {
        T[i] = (int)m_level.T[LevelN].T[i];
    }

    // Change lives count with difficulty settings
    switch (Pref.Difficulty) {
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
            if (T[i] == C_Life) {
                T[i] = C_Rail;
            }
        }
    }

    // Initialize locomotive
    Lo.Init(m_level.T[LevelN].StartX + m_level.T[LevelN].StartY * LT, m_level.T[LevelN].StartDir);
    BufKeyPress(m_level.T[LevelN].StartDir);
    MaskK = 0;

    // Adapt speed with difficulty
    switch (Pref.Difficulty) {
    case Easy:
        Pref.Speed = Pref.SpeedAverage = SPEED_MIN;
        break;
    case Hard:
        Pref.Speed = Pref.SpeedAverage = SPEED_MAX;
        break;
    default:
        Pref.Speed = Pref.SpeedAverage = SPEED_AVERAGE;
    }

    return DrawLevel(LevelN);
}

/*** Draws the background of the game ***/
/****************************************/
bool Game::DrawLevel(int LevelN)
{
    int i, x, y, m, cx, cy;

    // Builds the background of the game
    Sprites[background].Draw(400, 300, 0, Sprites[fgame].Image[0]);

    // Displays the train track
    for (i = 0; i < LT * HT; i++) {
        if (T[i] >= C_Rail && T[i] < C_Size) {
            y = i / LT * D_Case + D_Case / 2;
            x = i % LT * D_Case + D_Case / 2;

            m = 0;
            cx = i % LT;
            cy = i / LT;
            if (cy > 0 && T[i - LT] >= C_Rail && T[i - LT] < C_Size) {
                m += 8;
            }
            if (cy < HT - 1 && T[i + LT] >= C_Rail && T[i + LT] < C_Size) {
                m += 4;
            }
            if (cx > 0 && T[i - 1] >= C_Rail && T[i - 1] <= C_Size) {
                m += 2;
            }
            if (cx < LT - 1 && T[i + 1] >= C_Rail && T[i + 1] <= C_Size) {
                m += 1;
            }

            Sprites[rail].Draw(x, y, NumRail[m], Sprites[fgame].Image[0]);
        }
    }

    // Display decorations
#ifndef DCHILDREN
    for (i = 0; i < m_level.T[LevelN].NDeco; i++) {
        Sprites[deco].Draw(m_level.T[LevelN].Deco[i].x, m_level.T[LevelN].Deco[i].y, m_level.T[LevelN].Deco[i].NumSpr,
                           Sprites[fgame].Image[0]);
    }
#endif

    // Displays texts for selected language
    DrawText(740, 110, T_level, Sprites[fgame].Image[0]);
    DrawText(740, 180, T_score, Sprites[fgame].Image[0]);
    DrawText(740, 260, T_options, Sprites[fgame].Image[0]);
    DrawText(740, 340, T_lives, Sprites[fgame].Image[0]);

    DrawNumber(740, 140, Pref.Level + 1, Sprites[fgame].Image[0]);

    return true;
}

/*** Turn arrow with a keypress ***/
/**********************************/
void Game::TurnArrow()
{
    int Key = KeyPress[0];
    bool Search = false;
    int const x = Lo.IntersectPos % LT;
    int const y = Lo.IntersectPos / LT;

    do {
        // Key turns clockwise and searches if the new wanted direction is possible
        switch (Key) {
        case D_Top:
            Key = D_Right;
            if (Lo.EntryPos != D_Right && x + 1 < LT && T[Lo.IntersectPos + 1] != 0) {
                Search = true;
            }
            break;
        case D_Right:
            Key = D_Bottom;
            if (Lo.EntryPos != D_Bottom && y + 1 < HT && T[Lo.IntersectPos + LT] != 0) {
                Search = true;
            }
            break;
        case D_Bottom:
            Key = D_Left;
            if (Lo.EntryPos != D_Left && x > 0 && T[Lo.IntersectPos - 1] != 0) {
                Search = true;
            }
            break;
        case D_Left:
            Key = D_Top;
            if (Lo.EntryPos != D_Top && y > 0 && T[Lo.IntersectPos - LT] != 0) {
                Search = true;
            }
            break;
        }
    } while (Search == false);

    BufKeyPress(Key); // Stores the new default key
}

/*** Stores key presses in an array ***/
/**************************************/
void Game::BufKeyPress(int Key)
{
    int n = 0;

    // Prioritize new key
    while (KeyPress[n] != Key) {
        n++; // Stores the position of the key
    }

    if (n) { // On change, shift indices
        while (n) {
            KeyPress[n] = KeyPress[n - 1];
            n--;
        }
        KeyPress[0] = Key; // Store key
    }

    // Search the key's opposite
    switch (Key) {
    case D_Top:
        Key = D_Bottom;
        break;
    case D_Bottom:
        Key = D_Top;
        break;
    case D_Left:
        Key = D_Right;
        break;
    case D_Right:
        Key = D_Left;
        break;
    }

    // Disadvantage its opposite
    n = 3;
    while (KeyPress[n] != Key) {
        n--; // Stores the position of the key
    }

    if (n < 3) { // On change, shift indices
        while (n < 3) {
            KeyPress[n] = KeyPress[n + 1];
            n++;
        }
        KeyPress[3] = Key; // Store key
    }
}

/*** Attempts possible directions for the arrows ***/
/***************************************************/
int Game::TestArrows(int Up, int Down, int Left, int Right)
{
    int i;
    int const x = Lo.IntersectPos % LT;
    int const y = Lo.IntersectPos / LT;

    for (i = 0; i < 4; i++) {
        switch (KeyPress[i]) {
        case D_Top:
            if (y > 0 && Up != -1 && T[Lo.IntersectPos - LT] != 0) {
                return Up;
            }
            break;
        case D_Bottom:
            if (y + 1 < HT && Down != -1 && T[Lo.IntersectPos + LT] != 0) {
                return Down;
            }
            break;
        case D_Left:
            if (x > 0 && Left != -1 && T[Lo.IntersectPos - 1] != 0) {
                return Left;
            }
            break;
        case D_Right:
            if (x + 1 < LT && Right != -1 && T[Lo.IntersectPos + 1] != 0) {
                return Right;
            }
            break;
        }
    }
    return 0;
}

/*** Displays game screen ***/
/****************************/
void Game::DisplayScreen()
{
    int i;
    int ndir = 0;

    // New Display
    Lo.Display(m_screen); // Display locomotive

    if (Lo.IntersectPos != -1 && Help) { // Display arrow on future intersection
        switch (Lo.EntryPos) {
        case D_Left:
            ndir = TestArrows(0, 2, -1, 1);
            break;
        case D_Right:
            ndir = TestArrows(8, 6, 7, -1);
            break;
        case D_Top:
            ndir = TestArrows(-1, 4, 5, 3);
            break;
        case D_Bottom:
            ndir = TestArrows(10, -1, 9, 11);
            break;
        }

        m_screen.PrintSprite(dir, ndir, (Lo.IntersectPos % LT) * D_Case + D_Case / 2, (Lo.IntersectPos / LT) * D_Case + D_Case / 2);
    }

    // Display options
    for (i = 0; i < LT * HT; i++) {
        switch (T[i]) {
        case C_Car: // if car sprite
            m_screen.PrintSprite(car, (GameClock * 40 / 1000 + i * 7) % 50, i % LT * D_Case + D_Case / 2, i / LT * D_Case + D_Case / 2);
            break;
        case C_Expand: // if expander sprite
            m_screen.PrintSprite(expander, (GameClock * 40 / 1000 + i * 7) % 50, i % LT * D_Case + D_Case / 2, i / LT * D_Case + D_Case / 2);
            break;
        case C_Shrink: // if shrinker sprite
            m_screen.PrintSprite(shrinker, (GameClock * 40 / 1000 + i * 7) % 50, i % LT * D_Case + D_Case / 2, i / LT * D_Case + D_Case / 2);
            break;
        case C_Speed: // if speed sprite
            m_screen.PrintSprite(speed, (GameClock * 40 / 1000 + i * 7) % 50, i % LT * D_Case + D_Case / 2, i / LT * D_Case + D_Case / 2);
            break;
        case C_Life: // if life sprite
            m_screen.PrintSprite(life, (GameClock * 40 / 1000 + i * 7) % 50, i % LT * D_Case + D_Case / 2, i / LT * D_Case + D_Case / 2);
            break;
        }
    }

    // When paused, asks for a key press
    if (Pause) {
        m_screen.PrintText(T_press_any_key, LT * D_Case / 2, 300);
    }

    // Prints a dashboard
    m_screen.PrintOptions(Pref.Lives, Pref.Score);
    if (Pref.WagonGap < WAGON_GAP_MIN) {
        m_screen.PrintSprite(shrinker, (GameClock * 40 / 1000) % 50, 715, 295);
    }
    if (Pref.WagonGap > WAGON_GAP_AVERAGE) {
        m_screen.PrintSprite(expander, (GameClock * 40 / 1000) % 50, 715, 295);
    }
    if (Pref.SpeedAverage > Pref.Speed) {
        m_screen.PrintSprite(speed, (GameClock * 40 / 1000 + 7) % 50, 765, 295);
    }
}
