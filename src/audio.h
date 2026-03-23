//      (_||_/
//      (    )       Audio class
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

#ifndef AUDIO_DOM_
#define AUDIO_DOM_

#include <SDL2/SDL_mixer.h>

/*** Sound effects enum ***/
/**************************/
enum eSound {
    sClick = 0,
    sSpeed,
    sCrash,
    sEnd,
    sLose,
    sExpand,
    sCar,
    sShrink,
    sLive,
    sSize
};

class Audio
{
public:
    Audio() = default;
    ~Audio();

    bool Init(); // Initializes and loads audio files
    void LoadMusic(int Num); // Loads a music track, 0 = menu music 1,2,3,4=game music tracks
    void NextMusic(); // Switch to next game track

    void Play(eSound);
    void PlayMusic() const;

    void PauseMusic(bool IsMusicPlaying) const; // Pauses/Resumes music

    void DoVolume() const; // Handles sound volumes
    Mix_Music *Music { nullptr }; // Pointer to music tracks

private:
    int N { 0 }; // Number/Amount of sound effects
    int NMus { 0 }; // Number of the current music
    int MemorizedTime { 0 }; // Memorizes time for clicks
    Mix_Chunk **Sound { nullptr }; // Pointer to sound effects
};

#endif
