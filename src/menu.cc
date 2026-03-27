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

#define DCHILDREN
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

extern int currentTime;
extern int previousTime;
extern SDL_Window *sdlWindow;
extern SDL_Renderer *sdlRenderer;
extern Sprite *Sprites;
extern sNewPreference Pref;

static char Points[] = ". . . . . . . . . . . . . .";
static struct mPy Menu_Py[20];

/*** Sleeping for 50fps ***/
/**************************/
void Sleeping()
{
    int delay;

    if ((currentTime - previousTime) < (1000 / 60)) {
        delay = 1000 / 60 - (currentTime - previousTime);
        SDL_Delay(delay);
        currentTime = SDL_GetTicks();
    }
}

/*** Add button with text to the menu array ***/
/**********************************************/
void AddTextButton(int Num, std::string Text, int X, int Y, Screen& ScreenObj)
{
    ScreenObj.ChangeFontSize(45);

    int textWidth = ScreenObj.TextLength(Text);

    ScreenObj.PrintText(Text, X - textWidth / 2, Y - 22);

    Menu_Py[Num].StartX = X - textWidth / 2;
    Menu_Py[Num].StartY = Y - 22;
    Menu_Py[Num].EndX = X + textWidth / 2;
    Menu_Py[Num].EndY = Y + 22;
    Menu_Py[Num].Py = Num;
    Menu_Py[Num].Valid = true;
}

/*** Add an button to the menu array ***/
/***************************************/
void AddButton(int Num, e_Sprite NumSp, int X, int Y)
{
    int const NumS = (int)NumSp;

    Menu_Py[Num].StartX = X - Sprites[NumS].Dim[0].L / 2;
    Menu_Py[Num].StartY = Y - Sprites[NumS].Dim[0].H / 2;
    Menu_Py[Num].EndX = X + Sprites[NumS].Dim[0].L / 2;
    Menu_Py[Num].EndY = Y + Sprites[NumS].Dim[0].H / 2;
    Menu_Py[Num].Py = Num;
    Menu_Py[Num].Valid = true;
}

/*** Change Windowed/Fullscreen ***/
/**********************************/
void ChangeVideo()
{
    Uint32 flag = SDL_WINDOW_RESIZABLE;
    if (Pref.FullScreen) {
        flag = SDL_WINDOW_FULLSCREEN_DESKTOP;
    }
    SDL_SetWindowFullscreen(sdlWindow, flag);
    SDL_ShowCursor(0); // Hide cursor
}

eMenu Menu::SDLMain()
{
    int i;
    char MCode[5] = { 0, 0, 0, 0, 0 };
    char key;

    // Miscellaneous inits
    m_mouse.Init(Menu_Py);
    PyE = 0;

    // Fetching events
    do {
        m_screen.CleanSpriteAndScreen(fmenu);
        SDL_RenderClear(sdlRenderer);
        // Get background image and build menu
        Sprites[background_menu].Draw(400, 300, 0, Sprites[fmenu].Image[0]);
        Sprites[menu].Draw(400, 340, 0, Sprites[fmenu].Image[0]);
        Sprites[title].Draw(400, 65, 0, Sprites[fmenu].Image[0]);

        m_screen.ChangeFontSize(14);
        // Automatic year change
        m_screen.PrintText(CopyrightText, 400-m_screen.TextLength(CopyrightText)/2, 575);
        m_screen.ChangeFontSize(22);

        AddTextButton(0, "Play", 400, 229, m_screen);
        AddTextButton(1, "Leaderboard", 400, 306, m_screen);
        AddTextButton(2, "Settings", 400, 384, m_screen);
        AddTextButton(3, "Exit", 400, 461, m_screen);
        Menu_Py[4].StartX = -1;
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            m_mouse.GetEvent(event, PyE);
            m_gamepad.GetEvent(event);
            switch (event.type) {
            case SDL_KEYDOWN:
                if (event.key.state == SDL_PRESSED) {
                    m_audio.Play(sClick);
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
                        key = event.key.keysym.sym & 0x7F; // Gets the character corresponding to the key
                            for (i = 2; i >= 0; i--) {
                                MCode[i + 1] = MCode[i]; // shift code
                            }
                            MCode[0] = key;
                            if (strcmp(MCode, "tide") == 0 || strcmp(MCode, "TIDE") == 0) {
                                return mEdit; // If level editor selected
                            }
                    }
                }
                break;
            case SDL_QUIT:
                return mQuit;
                break;
            }
        }

        // Handle variables
        previousTime = currentTime;
        currentTime = SDL_GetTicks();
        Sleeping();

        // Handle display
        Print_Main();
        m_mouse.Print();

        // Update render
        SDL_RenderPresent(sdlRenderer);

    } while (true);

    return mQuit;
}

/*** SDL Main Menu Language choice ***/
/*************************************/
/*eMenu Menu::SDLMain_Language()
{
    int NCol = 1;
    int NL;
    int Gap;
    int i;
    int x, y;
    int const OldLanguage = Pref.Language;

    // Miscellaneous inits
    m_mouse.Init(Menu_Py);
    PyE = Pref.Language;
    if (PyE == -1) {
        PyE = 1;
    }

    SDL_RenderClear(sdlRenderer);
    // Set background image and build display
    Sprites[background_menu].Draw(400, 300, 0, Sprites[fmenu].Image[0]);

    // Draw available languages
    NCol = 3;
    if (Pref.NLanguages % NCol == 0) {
        NL = Pref.NLanguages / NCol;
    }
    else {
        NL = Pref.NLanguages / NCol + 1;
    }
    Gap = 600 / (NL + 1);

    for (i = 0; i < Pref.NLanguages; i++) {
        x = (i / NL) * (800 / 3) + (800 / 6);
        y = (i % NL) * Gap + Gap;

        Sprites[T_Language + i].Draw(x, y, 0, Sprites[fmenu].Image[0]);
        AddButton(i, (e_Sprite)(T_Language + i), x, y);
    }

    Menu_Py[Pref.NLanguages].StartX = -1;

    // Erase background
    SDL_RenderPresent(sdlRenderer);

    // Fetch events
    do {
        SDL_RenderClear(sdlRenderer);
        Sprites[background_menu].Draw(400, 300, 0, Sprites[fmenu].Image[0]);
        for (i = 0; i < Pref.NLanguages; i++) {
            x = (i / NL) * (800 / 3) + (800 / 6);
            y = (i % NL) * Gap + Gap;

            Sprites[T_Language + i].Draw(x, y, 0, Sprites[fmenu].Image[0]);
            AddButton(i, (e_Sprite)(T_Language + i), x, y);
        }

        SDL_Event event;
        while (SDL_PollEvent(&event)) {

            m_mouse.GetEvent(event, PyE);
            m_gamepad.GetEvent(event);
            switch (event.type) {
            case SDL_WINDOWEVENT:
                if (event.window.event == SDL_WINDOWEVENT_ENTER) {
                    SDL_RenderPresent(sdlRenderer);
                }
                break;
            case SDL_KEYDOWN:
                if (event.key.state == SDL_PRESSED) {
                    m_audio.Play(sClick);
                    switch (event.key.keysym.sym) {
                    case SDLK_ESCAPE:
                    case SDLK_AC_BACK: // Android back button
                        if (Pref.Language == -1) {
                            Pref.Language = PyE;
                        }
                        return mMenu;
                    case SDLK_UP:
                        PyE--;
                        if (PyE < 0) {
                            PyE = Pref.NLanguages - 1;
                        }
                        break;
                    case SDLK_DOWN:
                        PyE++;
                        if (PyE >= Pref.NLanguages) {
                            PyE = 0;
                        }
                        break;
                    case SDLK_LEFT:
                        if (PyE - NL >= 0) {
                            PyE -= NL;
                        }
                        break;
                    case SDLK_RIGHT:
                        if (PyE + NL < Pref.NLanguages) {
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
                        Pref.Language = PyE;
                        if (Pref.Language != OldLanguage) {
                            LoadLanguage();
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

        // Handle variables
        previousTime = currentTime;
        currentTime = SDL_GetTicks();
        Sleeping();

        // Handle display
        Center_Arrows();

        m_mouse.Print();

        // Update render
        SDL_RenderPresent(sdlRenderer);

    } while (true);

    return mQuit;
}*/

/*** SDL Main Menu Settings ***/
/******************************/
void Menu::InitMain_Options()
{
    // Miscellaneous inits
    m_mouse.Init(Menu_Py);

    // Set background image and build display
    Sprites[background_menu].Draw(400, 300, 0, Sprites[fmenu].Image[0]);
    Sprites[gmenu].Draw(400, 300, 0, Sprites[fmenu].Image[0]);
    Sprites[keys].Draw(610, 455, 0, Sprites[fmenu].Image[0]);

    AddButton(0, sound, 140, 110);
    AddButton(1, music, 160, 200);

    AddButton(2, fscreen, 190, 300);
    Menu_Py[2].StartX -= 40;
    Menu_Py[2].EndX = 625 + 40 + Sprites[fscreen].Dim[0].L / 2;
    Sprites[fscreen].Draw(185, 300, 0, Sprites[fmenu].Image[0]);
    Sprites[window].Draw(625, 300, 0, Sprites[fmenu].Image[0]);

    AddButton(3, earth, 180, 400);

    // Center text left
    AddTextButton(4, "Menu", 100, 490, m_screen);

    // Sound buttons
    Sprites[arrows].Draw(250, 110, 1, Sprites[fmenu].Image[0]);
    Sprites[arrows].Draw(700, 110, 4, Sprites[fmenu].Image[0]);
    Menu_Py[5].StartX = 230;
    Menu_Py[5].StartY = 70;
    Menu_Py[5].EndX = 475;
    Menu_Py[5].EndY = 145;
    Menu_Py[5].Py = 5;
    Menu_Py[5].Valid = true;

    Menu_Py[6].StartX = 476;
    Menu_Py[6].StartY = 70;
    Menu_Py[6].EndX = 720;
    Menu_Py[6].EndY = 145;
    Menu_Py[6].Py = 6;
    Menu_Py[6].Valid = true;

    // Music buttons
    Sprites[arrows].Draw(250, 200, 1, Sprites[fmenu].Image[0]);
    Sprites[arrows].Draw(700, 200, 4, Sprites[fmenu].Image[0]);
    Menu_Py[7].StartX = 230;
    Menu_Py[7].StartY = 155;
    Menu_Py[7].EndX = 475;
    Menu_Py[7].EndY = 245;
    Menu_Py[7].Py = 7;
    Menu_Py[7].Valid = true;

    Menu_Py[8].StartX = 476;
    Menu_Py[8].StartY = 155;
    Menu_Py[8].EndX = 720;
    Menu_Py[8].EndY = 245;
    Menu_Py[8].Py = 8;
    Menu_Py[8].Valid = true;

    Menu_Py[9].StartX = -1;
}

/*** Options menu management ***/
/*******************************/
eMenu Menu::SDLMain_Options()
{
    int i, N;
    int NumSp;

    PyE = 4;
    // Fetch events
    do {
        m_screen.CleanSpriteAndScreen(fmenu);
        SDL_RenderClear(sdlRenderer);
        InitMain_Options(); // Prepare menu
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            m_mouse.GetEvent(event, PyE);
            m_gamepad.GetEvent(event);

            switch (event.type) {
            case SDL_WINDOWEVENT:
                if (event.window.event == SDL_WINDOWEVENT_ENTER) {
                    SDL_RenderPresent(sdlRenderer);
                }
                break;
            case SDL_KEYDOWN:
                if (event.key.state == SDL_PRESSED) {
                    m_audio.Play(sClick);
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
                        case 5: // Lowers sound effects volume
                        case 6:
                            Pref.Volume -= SDL_MIX_MAXVOLUME / 10.0;
                            if (Pref.Volume < 0) {
                                Pref.Volume = 0;
                            }
                            m_audio.DoVolume();
                            m_audio.Play(sLive);
                            break;
                        case 1:
                        case 7: // Lowers music volume
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
                        case 2: // Display type
                            Pref.FullScreen = (Pref.FullScreen + 1) & 1;
                            ChangeVideo();
                            PyE = 2;
                            break;
                        case 3: // Language choice
                            //SDLMain_Language();
                            PyE = 3;
                            break;
                        case 5: // Lower sound effects volume
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
                        case 7: // Lower music volume
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

        //  Handle variables
        previousTime = currentTime;
        currentTime = SDL_GetTicks();
        Sleeping();

        if (Pref.FullScreen) {
            m_screen.PrintSprite(arrows, 1, 350, 300);
            m_screen.PrintSprite(arrows, 3, 450, 300);
        }
        else {
            m_screen.PrintSprite(arrows, 0, 350, 300);
            m_screen.PrintSprite(arrows, 4, 450, 300);
        }

        NumSp = (currentTime / 30) % 25;
        m_screen.PrintSprite(sound, NumSp, 150, 110);
        NumSp = (currentTime / 30) % 25;
        m_screen.PrintSprite(music, NumSp, 150, 200);
        NumSp = (currentTime / 50) % 50;
        m_screen.PrintSprite(earth, NumSp, 180, 400);

        N = (int)(Pref.Volume * 10 + 1) / SDL_MIX_MAXVOLUME;
        NumSp = (currentTime / 50) % 40 + 120;
        for (i = 0; i < N; i++) {
            if (i == N - 1) {
                m_screen.PrintSprite(locomotive, NumSp, (690 - 300) / 10 * i + 300, 110);
            }
            else {
                m_screen.PrintSprite(logs_wagon, NumSp, (690 - 300) / 10 * i + 300, 110);
            }
        }

        N = (int)(Pref.VolumeM * 10 + 1) / SDL_MIX_MAXVOLUME;
        for (i = 0; i < N; i++) {
            if (i == N - 1) {
                m_screen.PrintSprite(locomotive, NumSp, (690 - 300) / 10 * i + 300, 200);
            }
            else {
                m_screen.PrintSprite(logs_wagon, NumSp, (690 - 300) / 10 * i + 300, 200);
            }
        }

        switch (PyE) {
        case 3:
            Print_Main(180);
            break;
        case 4:
            Print_Main(CenterM);
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

        // Update render
        SDL_RenderPresent(sdlRenderer);

    } while (true);

    return mQuit;
}

/*** SDL Main Menu difficulty choice ***/
/***************************************/
eMenu Menu::SDLMain_Speed()
{
    // Miscellaneous inits
    m_mouse.Init(Menu_Py);
    PyE = 1;

    // Fetch events
    do {
        m_screen.CleanSpriteAndScreen(fmenu);
        SDL_RenderClear(sdlRenderer);
        // Set background image and build display
        Sprites[background_menu].Draw(400, 300, 0, Sprites[fmenu].Image[0]);
        Sprites[menu].Draw(400, 340, 0, Sprites[fmenu].Image[0]);
        Sprites[title].Draw(400, 65, 0, Sprites[fmenu].Image[0]);

        AddTextButton(0, "Easy", 400, 225, m_screen);
        AddTextButton(1, "Normal", 400, 340, m_screen);
        AddTextButton(2, "Hard", 400, 455, m_screen);
        Menu_Py[3].StartX = -1;

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            m_mouse.GetEvent(event, PyE);
            m_gamepad.GetEvent(event);

            switch (event.type) {
            case SDL_WINDOWEVENT:
                if (event.window.event == SDL_WINDOWEVENT_ENTER) {
                    SDL_RenderPresent(sdlRenderer);
                }
                break;
            case SDL_KEYDOWN:
                if (event.key.state == SDL_PRESSED) {
                    m_audio.Play(sClick);
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
                            Pref.Difficulty = Easy;
                            return Pref.LevelMax[Pref.Difficulty] > 0 ? mMenuLevel : mGame;
                        case 1:
                            Pref.Difficulty = Normal;
                            return Pref.LevelMax[Pref.Difficulty] > 0 ? mMenuLevel : mGame;
                        case 2:
                            Pref.Difficulty = Hard;
                            return Pref.LevelMax[Pref.Difficulty] > 0 ? mMenuLevel : mGame;
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

        // Handle variables
        previousTime = currentTime;
        currentTime = SDL_GetTicks();
        Sleeping();

        // Handle display
        Print_Main();
        m_mouse.Print();

        // Update render
        SDL_RenderPresent(sdlRenderer);

    } while (true);

    return mQuit;
}

/*** SDL Main Menu Level choice ***/
/**********************************/
eMenu Menu::SDLMain_Level()
{
    // Miscellaneous inits
    m_mouse.Init(Menu_Py);
    PyE = 0;
    Level = Pref.LevelMax[Pref.Difficulty];
    Pref.Level = 0;

    // Fetch events
    do {
        // Erase background
        m_screen.CleanSpriteAndScreen(fmenu);
        SDL_RenderClear(sdlRenderer);
        // Set background image and build display
        Sprites[background_menu].Draw(400, 300, 0, Sprites[fmenu].Image[0]);
        Sprites[menu].Draw(400, 340, 0, Sprites[fmenu].Image[0]);
        Sprites[title].Draw(400, 65, 0, Sprites[fmenu].Image[0]);

        AddTextButton(0, "New game", 400, 225, m_screen);
        AddTextButton(1, "Old level", 400, 340, m_screen);
        AddTextButton(2, "Menu", 400, 445, m_screen);

        AddButton(3, arrows, 330, 380);
        AddButton(4, arrows, 470, 380);

        Menu_Py[5].StartX = -1;

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            m_mouse.GetEvent(event, PyE);
            m_gamepad.GetEvent(event);

            switch (event.type) {
            case SDL_WINDOWEVENT:
                if (event.window.event == SDL_WINDOWEVENT_ENTER) {
                    SDL_RenderPresent(sdlRenderer);
                }
                break;
            case SDL_KEYDOWN:
                if (event.key.state == SDL_PRESSED) {
                    m_audio.Play(sClick);
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
                        if (Level > 0) {
                            Level--;
                        }
                        PyE = 1;
                        break;
                    case SDLK_RIGHT:
                        if (Level < Pref.LevelMax[Pref.Difficulty]) {
                            Level++;
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
                            Pref.Level = Level;
                            return mGame;
                        case 2:
                            return mMenu;
                        case 3:
                            if (Level > 0) {
                                Level--;
                            }
                            break;
                        case 4:
                            if (Level < Pref.LevelMax[Pref.Difficulty]) {
                                Level++;
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

        // Handle variables
        previousTime = currentTime;
        currentTime = SDL_GetTicks();
        Sleeping();

        // Draw arrows
        if (Level > 0) {
            if (PyE == 3) {
                m_screen.PrintSprite(arrows, 2, 330, 380);
            }
            else {
                m_screen.PrintSprite(arrows, 1, 330, 380);
            }
        }
        else {
            m_screen.PrintSprite(arrows, 0, 330, 380);
        }

        if (Level < Pref.LevelMax[Pref.Difficulty]) {
            if (PyE == 4) {
                m_screen.PrintSprite(arrows, 5, 470, 380);
            }
            else {
                m_screen.PrintSprite(arrows, 4, 470, 380);
            }
        }
        else {
            m_screen.PrintSprite(arrows, 3, 470, 380);
        }

        m_screen.PrintText(std::to_string(Level + 1), 400, 380);

        if (PyE != 3 && PyE != 4) {
            Print_Main();
        }
        m_mouse.Print();

        // Update render
        SDL_RenderPresent(sdlRenderer);

    } while (true);

    return mQuit;
}

/*** SDL Main questions on human rights ***/
/******************************************/
#ifndef DCHILDREN
eMenu Menu::SDLMain_HR()
{
    int Done = -1;
    int N1, N2, Order;
    SDL_Rect Position;

    // Miscellaneous inits
    m_mouse.Init(Menu_Py);
    PyE = rand() % 2;

    // Question choice, N1=answer
    N1 = rand() % 30;
    do {
        N2 = rand() % 30;
    } while (N2 == N1);
    Order = rand() % 2; // if 0=right order, 1=inverted

    // Set background image and build display
    Position.x = Position.y = 0;
    Position.w = Sprites[fmenu].Dim[0].L;
    Position.h = Sprites[fmenu].Dim[0].H;
    SDL_RenderCopy(sdlRenderer, Sprites[fmenu].Image[0], &Position, &Position);

    Sprites[menu].Draw(340, 300, 0, Sprites[fmenu].Image[0]);
    Sprites[background_hr].Draw(340, 74, 0, Sprites[fmenu].Image[0]);
    DrawText(338, 70, e_Sprite(T_question), Sprites[fmenu].Image[0]);

    Sprites[locomotive].Draw(115, 110, rand() % 320, Sprites[fmenu].Image[0]);
    Sprites[deco].Draw(100, 160 + (rand() % 130), rand() % 18, Sprites[fmenu].Image[0]);
    Sprites[deco].Draw(100, 470 - (rand() % 130), rand() % 18, Sprites[fmenu].Image[0]);
    Sprites[deco].Draw(580, 100 + (rand() % 130), rand() % 18, Sprites[fmenu].Image[0]);
    Sprites[deco].Draw(580, 470 - (rand() % 130), rand() % 18, Sprites[fmenu].Image[0]);

    DrawText(340, 300, e_Sprite(T_art1 + N1), Sprites[fmenu].Image[0]);

    if (Order) {
        AddButton(0, e_Sprite(T_tart1 + N1), 240, 492);
        AddButton(1, e_Sprite(T_tart1 + N2), 440, 492);
    }
    else {
        AddButton(0, e_Sprite(T_tart1 + N1), 440, 492);
        AddButton(1, e_Sprite(T_tart1 + N2), 240, 492);
    }
    Menu_Py[0].StartY -= 20;
    Menu_Py[0].EndY += 20;
    Menu_Py[1].StartY -= 20;
    Menu_Py[1].EndY += 20;

    Menu_Py[2].StartX = -1;

    std::array<std::pair<int, int>, 4> posDeco = { std::make_pair(rand() % 130, rand() % 18), std::make_pair(rand() % 130, rand() % 18), std::make_pair(rand() % 130, rand() % 18), std::make_pair(rand() % 130, rand() % 18) };
    int const locoPosition = rand() % 320;

    // Fetch events
    do {
        // Erase background
        SDL_RenderClear(sdlRenderer);

        SDL_RenderCopy(sdlRenderer, Sprites[fmenu].Image[0], &Position, &Position);

        Sprites[menu].Draw(340, 300, 0, Sprites[fmenu].Image[0]);
        Sprites[background_hr].Draw(340, 74, 0, Sprites[fmenu].Image[0]);
        DrawText(338, 70, e_Sprite(T_question), Sprites[fmenu].Image[0]);

        Sprites[locomotive].Draw(115, 110, locoPosition, Sprites[fmenu].Image[0]);
        Sprites[deco].Draw(100, 160 + posDeco[0].first, posDeco[0].second, Sprites[fmenu].Image[0]);
        Sprites[deco].Draw(100, 470 - (posDeco[1].first), posDeco[1].second, Sprites[fmenu].Image[0]);
        Sprites[deco].Draw(580, 100 + (posDeco[2].first), posDeco[2].second, Sprites[fmenu].Image[0]);
        Sprites[deco].Draw(580, 470 - (posDeco[3].first), posDeco[3].second, Sprites[fmenu].Image[0]);

        DrawText(340, 300, e_Sprite(T_art1 + N1), Sprites[fmenu].Image[0]);
        /*
        if(Order) {
          AddButton(0,e_Sprite(T_tart1+N1),240,492);
          AddButton(1,e_Sprite(T_tart1+N2),440,492);
        }
        else {
          AddButton(0,e_Sprite(T_tart1+N1),440,492);
          AddButton(1,e_Sprite(T_tart1+N2),240,492);
        }
        Menu_Py[0].StartX-=20;
        Menu_Py[0].EndX+=20;
        Menu_Py[1].StartY-=20;
        Menu_Py[1].EndY+=20;

        Menu_Py[2].StartX=-1;
      */
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            m_mouse.GetEvent(event, PyE);
            m_gamepad.GetEvent(event);

            switch (event.type) {
            case SDL_WINDOWEVENT:
                if (event.window.event == SDL_WINDOWEVENT_ENTER) {
                }
                break;
            case SDL_KEYDOWN:
                if (Done == -1 && event.key.state == SDL_PRESSED) {
                    m_audio.Play(sClick);
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
                        if (Order) {
                            PyE = 0;
                        }
                        else {
                            PyE = 1;
                        }
                        break;
                    case SDLK_RIGHT:
                        if (Order) {
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
                            Done = currentTime + 2000;
                            break;
                        case 1:
                            m_audio.Play(sLose);
                            Done = currentTime + 2000;
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

        // Test if Endished
        if (Done != -1 && Done < currentTime) {
            return mGame;
        }

        // Handle variables
        previousTime = currentTime;
        currentTime = SDL_GetTicks();
        Sleeping();

        if (Order) {
            m_screen.PrintSprite(background_hrr, 0, 240, 492);
            DrawText(240, 492, e_Sprite(T_tart1 + N1));
        }
        else {
            m_screen.PrintSprite(background_hrr, 0, 440, 492);
            DrawText(440, 492, e_Sprite(T_tart1 + N1));
        }

        if (Done == -1) {
            if (Order) {
                m_screen.PrintSprite(background_hrr, 0, 440, 492);
                DrawText(440, 492, e_Sprite(T_tart1 + N2));
                if (PyE == 0) {
                    Print_Main(240);
                }
                else {
                    Print_Main(440);
                }
            }
            else {
                m_screen.PrintSprite(background_hrr, 0, 240, 492);
                DrawText(240, 492, e_Sprite(T_tart1 + N2));
                if (PyE == 1) {
                    Print_Main(240);
                }
                else {
                    Print_Main(440);
                }
            }
            m_mouse.Print();
        }

        // Update render
        SDL_RenderPresent(sdlRenderer);

    } while (true);

    return mQuit;
}
#endif

/*** SDL Main InGame ***/
/***********************/
void Menu::Print_InGame()
{
    SDL_Rect Position;

    // Miscellaneous inits
    m_mouse.Init(Menu_Py);

    // Set background image and build display
    Position.x = Position.y = 0;
    Position.w = Sprites[fmenu].Dim[0].L;
    Position.h = Sprites[fmenu].Dim[0].H;
    SDL_RenderCopy(sdlRenderer, Sprites[fmenu].Image[0], &Position, &Position);

    Sprites[menu].Draw(340, 300, 0, Sprites[fmenu].Image[0]);

    AddTextButton(0, "Continue", 340, 185, m_screen);
    AddTextButton(1, "Settings", 340, 300, m_screen);
    AddTextButton(2, "Exit from game", 340, 415, m_screen);
    Menu_Py[3].StartX = -1;
}

/*** Managing menu InGame ***/
/****************************/
eMenu Menu::SDLMain_InGame()
{
    Print_InGame();

    // Fetch events
    do {
        m_screen.CleanSpriteAndScreen(fmenu);
        SDL_RenderClear(sdlRenderer);
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            m_mouse.GetEvent(event, PyE);
            m_gamepad.GetEvent(event);

            switch (event.type) {
            case SDL_WINDOWEVENT:
                if (event.window.event == SDL_WINDOWEVENT_ENTER) {
                    // SDL_RenderPresent(sdlRenderer);
                    // Ec.Cls(fmenu);
                }
                break;
            case SDL_KEYDOWN:
                if (event.key.state == SDL_PRESSED) {
                    m_audio.Play(sClick);
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

        // Handle variables
        previousTime = currentTime;
        currentTime = SDL_GetTicks();
        Sleeping();

        // Handle display
        Print_InGame();
        Print_Main(340);
        m_mouse.Print();

        // Update render
        SDL_RenderPresent(sdlRenderer);

    } while (true);

    return mQuit;
}

/*** SDL Main Menu difficulty choice ***/
/***************************************/
eMenu Menu::SDLMain_Score(bool EditScore)
{
    int i;
    int NEdit = -1;
    char Provi[256];
    char key;

    // Cherche le numéro du score à remplacer si edition des scores
    if (EditScore) {
        for (i = 0; i < 8; i++) {
            if (Pref.Sco[i].Score < Pref.Score) {
                NEdit = i;
                break;
            }
        }
        if (NEdit == -1) {
            return mMenu;
        }

        if (NEdit < 7) { // Si doit fair un décalage
            for (i = 7; i > NEdit; i--) {
                Pref.Sco[i].Score = Pref.Sco[i - 1].Score;
                Pref.Sco[i].Name = Pref.Sco[i - 1].Name;
            }
        }

        // Efface le nouveau nom et met le score
        Pref.Sco[NEdit].Score = Pref.Score;
        Pref.Sco[NEdit].Name.clear();
    }

    // Met la sourie sur tous l'ecran
    m_mouse.Init(Menu_Py); // Initialise la sourie
    Menu_Py[0].StartX = 0;
    Menu_Py[0].StartY = 0;
    Menu_Py[0].EndX = 800;
    Menu_Py[0].EndY = 600;
    Menu_Py[0].Py = 0;
    Menu_Py[0].Valid = true;
    Menu_Py[1].StartX = -1;

    if (EditScore) {
        SDL_StartTextInput();
    }

    // Prend les evenements
    do {
        // Efface le background
        m_screen.CleanSpriteAndScreen(fmenu);
        SDL_RenderClear(sdlRenderer);
        // Prend l'image du background et fait l'affichage
        Sprites[background_menu].Draw(400, 300, 0, Sprites[fmenu].Image[0]);

        // Draw title and commands
        m_screen.ChangeFontColor(255, 255, 0);
        m_screen.PrintText("Leaderboard", 400-m_screen.TextLength("Leaderboard")/2, 50);
        #ifndef ANDROID
        m_screen.PrintText("Press enter to continue", 400-m_screen.TextLength("Press enter to continue")/2, 550);
        #else
        m_screen.PrintText("Tap to continue", 400-m_screen.TextLength("Tap to continue")/2, 550);
        #endif
        m_screen.ChangeFontColor(255, 255, 255);

        // Draw scores
        for (i = 0; i < 8; i++) {
            sprintf(Provi, "%d", i + 1);
            m_screen.PrintText(Provi, 70, 120 + i * (360 / 7));

            if (EditScore == false || NEdit != i) {
                if (!Pref.Sco[i].Name.empty()) {
                    m_screen.PrintText(Pref.Sco[i].Name, 140, 120 + i * (360 / 7));
                }
                else {
                    m_screen.PrintText(Points, 140, 120 + i * (360 / 7));
                }
            }

            sprintf(Provi, "%i", Pref.Sco[i].Score);
            m_screen.PrintText(Provi, 740 - m_screen.TextLength(std::string(Provi)), 120 + i * (360 / 7));
        }

        // Efface le background
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
                    if (NEdit >= 0 && event.key.keysym.sym == SDLK_BACKSPACE && !Pref.Sco[NEdit].Name.empty()) {
                        Pref.Sco[NEdit].Name.pop_back();
                    }
                    m_audio.Play(sClick);
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
                    }
                }
                break;
            case SDL_TEXTINPUT:
                if (NEdit >= 0) {
                    Pref.Sco[NEdit].Name += event.text.text;
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
        previousTime = currentTime;
        currentTime = SDL_GetTicks();
        Sleeping();

        if (EditScore && NEdit >= 0 && NEdit < 8) {  // Handle the scores edition drawing
            if (!Pref.Sco[NEdit].Name.empty()) {
                m_screen.PrintText(Pref.Sco[NEdit].Name, 140, 120 + NEdit * (360 / 7));
            }

            i = (currentTime / 50) % 20; // Draw cursors
            m_screen.PrintSprite(arrow_left, i, 110, 120 + NEdit * (360 / 7));

            int textLen = Pref.Sco[NEdit].Name.empty() ? 0 : m_screen.TextLength(Pref.Sco[NEdit].Name);
            m_screen.PrintSprite(arrow_right, i, 180 + textLen, 120 + NEdit * (360 / 7));
        }

        // Echange les buffets video
        SDL_RenderPresent(sdlRenderer);

    } while (true);

    if (EditScore) {
        SDL_StopTextInput();
    }
    return mQuit;
}

/*** Draw main menu ***/
/**********************/
void Menu::Print_Main(int Center) const
{
    int const NumSp = (currentTime / 50) % 20;
    int const x1 = Menu_Py[PyE].StartX - 25;
    int const x2 = (Center - x1) + Center;
    int const y = (Menu_Py[PyE].EndY + Menu_Py[PyE].StartY) / 2;

    m_screen.PrintSprite(arrow_left, NumSp, x1, y);
    m_screen.PrintSprite(arrow_right, NumSp, x2, y);
}

/*** Center arrows on the menu ***/
/*********************************/
void Menu::Center_Arrows() const
{
    int const NumSp = (currentTime / 50) % 20;
    int const x1 = Menu_Py[PyE].StartX - 5;
    int const x2 = Menu_Py[PyE].EndX + 5;
    int const y = (Menu_Py[PyE].EndY + Menu_Py[PyE].StartY) / 2;

    m_screen.PrintSprite(arrow_left, NumSp, x1, y);
    m_screen.PrintSprite(arrow_right, NumSp, x2, y);
}
