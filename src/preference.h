//      (_||_/
//      (    )       Preferences data
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

#ifndef PREFERENCE_DOM_
#define PREFERENCE_DOM_

#include <SDL2/SDL_audio.h> // for SDL_MIX_MAXVOLUME
#include <string>

#define SPEED_MAX 180.0
#define SPEED_AVERAGE 120.0
#define SPEED_MIN 80.0
#define SPEED_DURATION (10 * 1000)

#define PAUSE_DURATION (3 * 1000)

#define REDUCTION_DURATION (10 * 1000)
#define EXTENSION_DURATION (10 * 1000)
#define WAGON_GAP_MAX 27.0
#define WAGON_GAP_AVERAGE 13.0
#define WAGON_GAP_MIN 4.0

#define N_LIVES_COUNT 3

#define RAY_HIT (32 * 32)

#define LT 17
#define HT 15
#define MAX_N_LEVEL_IN_MEMORY 256

#define D_Left 0
#define D_Right 1
#define D_Top 2
#define D_Bottom 3

#define D_Case 40
#define D_CaseR 31.416

/*** Possible pieces on the level ***/
#define C_None 0
#define C_Rail 1
#define C_Wagon 2
#define C_Expand 3
#define C_Shrink 4
#define C_Speed 5
#define C_Life 6
#define C_Size 7

enum e_Difficulty {
    Easy,
    Normal,
    Hard
};

/*** Remember score ***/
#define LSCOREMAX 460

struct sScore
{
    int Score;
    std::string Name;
};

/*** Old Preferences structures, UNUSED ***/
struct sOldPreference
{
    e_Difficulty Difficulte; // Difficulty (Speed)
    int Level; // Player Level
    int Lives; // Player Life count
    int Score; // Player Score
    double Vitesse; // Speed according to level
    double VitesseMoy; // Current locomotive speed
    float EcartWagon; // gap in pixels between wagons
    int NiveauMax;
    int FullScreen;
    int Langue; // Language displayed
    int NLangues; // Available languages
    float Volume; // Sound effects volume
    float VolumeM; // Music volume
    struct sScore Sco[8]; // Store scores
};

enum eAudioTheme {
    mMaf = 0, // original Ri-Li soundtrack
    mZabiden = 1 // updated soundtrack
};

struct sNewPreference
{
    e_Difficulty Difficulty { Normal }; // current game difficulty
    int Level; // current level
    int Lives { N_LIVES_COUNT }; // current number of life
    int Score; // current score
    double Speed { SPEED_AVERAGE }; // speed ​​depending on level
    double SpeedAverage { SPEED_AVERAGE }; // current speed of the locomotive
    float WagonGap { WAGON_GAP_AVERAGE }; // size in pixels between wagons
    int NLanguages; // number of languages ​​available
    int LevelMax[3] { 0, 0, 0 }; // One per difficulty
    int FullScreen { true };
    int Language { -1 }; // locale index
    float Volume { (float)SDL_MIX_MAXVOLUME }; // audio volume
    float VolumeM { (float)SDL_MIX_MAXVOLUME }; // music volume
    struct sScore Sco[8]; // store scores
    eAudioTheme AudioTheme { mZabiden }; // which audio theme to use
    int HumanRightsQuiz { 1 }; // enable the human rights questions at the end of a level
};

enum eMenu {
    mMenu,
    mMenuGame,
    mMenuSpeed,
    mMenuLevel,
    mLanguage,
    mGame,
    mScore,
    mScoreEdit,
    mOption,
    mQuit,
    mEdit,
};

#endif
