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
#include <SDL2/SDL_keycode.h> // for SDLK_DELETE, SDLK_DOWN, SDLK_ESCAPE
#include <SDL2/SDL_log.h> // for SDL_LogError, SDL_LOG_CATEGORY_APPLICA...
#include <SDL2/SDL_render.h> // for SDL_RenderPresent, SDL_Renderer
#include <SDL2/SDL_timer.h> // for SDL_GetTicks
#include <SDL2/SDL_video.h> // for SDL_WINDOWEVENT_ENTER
#include <cstdlib>

#include "editor.h"
#include "menu.h"
#include "game.h"
#include "level.h"
#include "gamepad.h"
#include "mouse.h"

extern SDL_Renderer *sdlRenderer;

extern Sprite *Sprites;
extern sNewPreference Pref;

extern int currentTime;
extern int previousTime;

extern Level level;

static int NumRail[] = { 10, 10, 10, 0, 10, 1, 2, 3, 10, 4, 5, 6, 7, 8, 9, 10 };

eMenu Editor::SDLMain(int LevelNumber)
{
    int PyE;
    int cx = 0, cy = 0;
    bool Button = false;
    int TypeB = -1;
    int i, d, dx, dy;

    NumN = LevelNumber;

    Draw(); // Loads level
    SDL_RenderPresent(sdlRenderer);

    currentTime = SDL_GetTicks(); // Get clock time

    Option = rail;

    m_mouse.Init(nullptr);

    // Fetch events
    do {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            m_mouse.GetEvent(event, PyE);
            m_gamepad.GetEvent(event);

            switch (event.type) {
            case SDL_WINDOWEVENT:
                if (event.window.event == SDL_WINDOWEVENT_ENTER) {
                    Draw();
                }
                break;
            case SDL_KEYDOWN:
                if (event.key.state == SDL_PRESSED) {
                    if (event.key.keysym.sym == SDLK_ESCAPE) {
                        return mMenu;
                    }
                    GetKeyPress(event.key.keysym.sym);
                }
                break;
            case SDL_MOUSEBUTTONDOWN:
                if (event.button.state == SDL_PRESSED) {
                    Button = true;
                    TypeB = -1;
                }
                break;
            case SDL_MOUSEBUTTONUP:
                Button = false;
                if (TypeB != -1 && Option == deco && cx >= LT) { // If a decoration must be deleted
                    level.T[NumN].NDeco--;
                }
                TypeB = -1;
                break;
            case SDL_QUIT:
                return mQuit;
                break;
            }
        }

        // Handles mouse click
        cx = m_mouse.Px / D_Case;
        cy = m_mouse.Py / D_Case;

        if (Button && cx < LT) {
            switch (Option) {
            case deco:
                if (TypeB == -1) { // On first time clicking
                    for (i = 0; i < level.T[NumN].NDeco; i++) { // Search if there's a decoration nearby the click
                        dx = level.T[NumN].Deco[i].x - m_mouse.Px;
                        dy = level.T[NumN].Deco[i].y - m_mouse.Py;
                        d = dx * dx + dy * dy;
                        if (d <= (D_Case * 2) * (D_Case * 2)) {
                            TypeB = i;
                        }
                    }
                    if (TypeB == -1) { // Building a new decor
                        level.T[NumN].NDeco++;
                        level.T[NumN].Deco[(level.T[NumN].NDeco - 1)].NumSpr = NumDeco;
                        level.T[NumN].Deco[(level.T[NumN].NDeco - 1)].x = m_mouse.Px;
                        level.T[NumN].Deco[(level.T[NumN].NDeco - 1)].y = m_mouse.Py;
                        TypeB = 1;
                    }
                    else { // Highlight selection
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
                else { // if not the first click, replace
                    level.T[NumN].Deco[(level.T[NumN].NDeco - 1)].NumSpr = NumDeco;
                    level.T[NumN].Deco[(level.T[NumN].NDeco - 1)].x = m_mouse.Px;
                    level.T[NumN].Deco[(level.T[NumN].NDeco - 1)].y = m_mouse.Py;
                }
                break;
            case rail:
                if (TypeB == -1) {
                    if (level.T[NumN].T[cy * LT + cx] != C_Rail) {
                        TypeB = C_Rail;
                    }
                    else {
                        TypeB = C_None;
                    }
                }
                level.T[NumN].T[cy * LT + cx] = TypeB;
                break;
            case car:
                level.T[NumN].T[cy * LT + cx] = C_Car;
                break;
            case expander:
                level.T[NumN].T[cy * LT + cx] = C_Expand;
                break;
            case shrinker:
                level.T[NumN].T[cy * LT + cx] = C_Shrink;
                break;
            case speed:
                level.T[NumN].T[cy * LT + cx] = C_Speed;
                break;
            case life:
                level.T[NumN].T[cy * LT + cx] = C_Life;
                break;
            case (e_Sprite)(locomotive + D_Top):
            case (e_Sprite)(locomotive + D_Bottom):
            case (e_Sprite)(locomotive + D_Left):
            case (e_Sprite)(locomotive + D_Right):
                level.T[NumN].StartX = cx;
                level.T[NumN].StartY = cy;
                level.T[NumN].StartDir = (int)(Option) - (int)(locomotive);
                break;
            default:
                break;
            }
        }

        // Handles time and pause
        previousTime = currentTime;
        currentTime = SDL_GetTicks();
        Sleeping();

        // Handles displaying
        Draw();
        SDL_RenderPresent(sdlRenderer);

    } while (true);

    return mQuit;
}

/*** Loads a map ***/
/*******************/
void Editor::Draw() const
{
    int i, x, y, m, cx, cy;
    unsigned char *T;

    // Address of the level
    T = level.T[NumN].T;

    // Builds the game's background
    Sprites[background].Draw(400, 300, 0);

    // Displays the train track
    for (i = 0; i < LT * HT; i++) {
        if (T[i] >= C_Rail && T[i] < C_Size) {
            y = i / LT * D_Case + D_Case / 2;
            x = i % LT * D_Case + D_Case / 2;

            m = 0;
            cx = i % LT;
            cy = i / LT;
            if (cy > 0 && T[i - LT] >= 1 && T[i - LT] < C_Size) {
                m += 8;
            }
            if (cy < HT - 1 && T[i + LT] >= 1 && T[i + LT] < C_Size) {
                m += 4;
            }
            if (cx > 0 && T[i - 1] >= 1 && T[i - 1] < C_Size) {
                m += 2;
            }
            if (cx < LT - 1 && T[i + 1] >= 1 && T[i + 1] < C_Size) {
                m += 1;
            }

            Sprites[rail].Draw(x, y, NumRail[m]);
        }
    }

    // Displays decorations
    for (i = 0; i < level.T[NumN].NDeco; i++) {
        Sprites[deco].Draw(level.T[NumN].Deco[i].x, level.T[NumN].Deco[i].y, level.T[NumN].Deco[i].NumSpr);
    }

    // Displays the level number
    DrawNumber(740, 130, NumN + 1);

    // Display possible sprites
    for (i = 0; i < LT * HT; i++) {
        switch (T[i]) {
        case C_Car: // Car sprite
            Sprites[car].Draw(i % LT * D_Case + D_Case / 2, i / LT * D_Case + D_Case / 2, 25);
            break;
        case C_Expand: // Expand sprite
            Sprites[expander].Draw(i % LT * D_Case + D_Case / 2, i / LT * D_Case + D_Case / 2, 25);
            break;
        case C_Shrink: // Shrink sprite
            Sprites[shrinker].Draw(i % LT * D_Case + D_Case / 2, i / LT * D_Case + D_Case / 2, 25);
            break;
        case C_Speed: // Speed sprite
            Sprites[speed].Draw(i % LT * D_Case + D_Case / 2, i / LT * D_Case + D_Case / 2, 25);
            break;
        case C_Life: // Life sprite
            Sprites[life].Draw(i % LT * D_Case + D_Case / 2, i / LT * D_Case + D_Case / 2, 25);
            break;
        }
    }

    // Displays the starting point of the locomotive
    switch (level.T[NumN].StartDir) {
    case D_Top:
        Sprites[locomotive].Draw(level.T[NumN].StartX * D_Case + D_Case / 2, level.T[NumN].StartY * D_Case + D_Case / 2, 0);
        break;
    case D_Bottom:
        Sprites[locomotive].Draw(level.T[NumN].StartX * D_Case + D_Case / 2, level.T[NumN].StartY * D_Case + D_Case / 2, 40);
        break;
    case D_Left:
        Sprites[locomotive].Draw(level.T[NumN].StartX * D_Case + D_Case / 2, level.T[NumN].StartY * D_Case + D_Case / 2, 80);
        break;
    case D_Right:
        Sprites[locomotive].Draw(level.T[NumN].StartX * D_Case + D_Case / 2, level.T[NumN].StartY * D_Case + D_Case / 2, 120);
        break;
    }

    // Displays selected option in the menu
    switch (Option) {
    case rail:
    case car:
    case expander:
    case shrinker:
    case speed:
    case life:
        Sprites[Option].Draw(740, 200, 0);
        break;
    case (e_Sprite)(locomotive + D_Top):
        Sprites[locomotive].Draw(740, 200, 0);
        break;
    case (e_Sprite)(locomotive + D_Bottom):
        Sprites[locomotive].Draw(740, 200, 40);
        break;
    case (e_Sprite)(locomotive + D_Left):
        Sprites[locomotive].Draw(740, 200, 80);
        break;
    case (e_Sprite)(locomotive + D_Right):
        Sprites[locomotive].Draw(740, 200, 120);
        break;
    case deco:
        Sprites[deco].Draw(740, 200, NumDeco);
        break;
    default:
        break;
    }

    // Displays the cursor
    if (Option != deco) {
        Sprites[cursor].Draw(m_mouse.Px, m_mouse.Py, 0);
    }
    else {
        Sprites[deco].Draw(m_mouse.Px, m_mouse.Py, NumDeco);
    }
}

/*** Fetches pressed/held keys ***/
/*********************************/
void Editor::GetKeyPress(int Key)
{
    int i, j;

    switch (Key) {
    case SDLK_PAGEUP:
        if (NumN < level.N - 1) {
            NumN++;
        }
        else {
            j = 0;
            for (i = 0; i < LT * HT; i++) {
                j += level.T[NumN].T[i];
            }
            if (j != 0) {
                NumN++;
                level.N++;
                level.Clear(NumN);
            }
        }
        break;
    case SDLK_PAGEDOWN:
        if (NumN) {
            NumN--;
        }
        break;
    case 'p':
        Option = deco;
        NumDeco++;
        if (NumDeco >= Sprites[deco].N) {
            NumDeco = 0;
        }
        break;
    case 'o':
        Option = deco;
        NumDeco--;
        if (NumDeco < 0) {
            NumDeco = Sprites[deco].N - 1;
        }
        break;
    case SDLK_INSERT:
        level.Ins(NumN);
        break;
    case SDLK_DELETE:
        level.Del(NumN);
        break;
    case SDLK_UP:
        Option = (e_Sprite)(locomotive + D_Top);
        break;
    case SDLK_DOWN:
        Option = (e_Sprite)(locomotive + D_Bottom);
        break;
    case SDLK_LEFT:
        Option = (e_Sprite)(locomotive + D_Left);
        break;
    case SDLK_RIGHT:
        Option = (e_Sprite)(locomotive + D_Right);
        break;
    case 'c':
        level.Clear(NumN);
        break;
    case '$':
        // Checks if the last level is empty
        for (j = i = 0; i < LT * HT; i++) {
            j += level.T[level.N - 1].T[i];
        }
        if (j == 0) {
            if (NumN < level.N - 1) {
                level.N--; // Not counted if empty
            }
        }

        // Saving the level
        if (level.Save() == false) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Error while saving levels");
            exit(-1);
        }

        // Testing the level
        Pref.Level = NumN;
        m_game.SDLMain();
        m_mouse.Init(nullptr);
        Draw();
        break;
    case 'a':
        Option = rail;
        break;
    case 'z':
        Option = car;
        break;
    case 'e':
        Option = expander;
        break;
    case 'r':
        Option = shrinker;
        break;
    case 't':
        Option = speed;
        break;
    case 'y':
        Option = life;
        break;
    }
}
