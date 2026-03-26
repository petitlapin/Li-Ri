//      (_||_/
//      (    )       Mouse class
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

#ifndef MOUSE_DOM_
#define MOUSE_DOM_

#include <SDL2/SDL_events.h>

class Audio;
class Screen;

/*** Structure for the positions in the Py menu ***/
/**************************************************/
struct mPy
{ // DepX==-1 if last entry
    int StartX, StartY;
    int EndX, EndY;
    int Py;
    bool Valid; // Trigger enter key on mouse click
};

struct mButton
{ // DepX=-1 if last entry
    int StartX, StartY;
    int EndX, EndY;
    int Value;
    int *Adr;
};

class Mouse
{
public:
    explicit Mouse(Audio &audio, Screen &screen) :
        m_audio(audio), m_screen(screen) { }
    ~Mouse() = default;

    void InitStart(); // Inits the mouse coordinates
    void Init(struct mPy *TablePy, struct mButton *B = nullptr); // Mouse init
    void GetEvent(SDL_Event &event, int &Py); // Fetches events
    void Print() const; // Displays cursor

    int Px { 400 }, Py { 300 }; // Mouse screen position

private:
    Audio &m_audio;
    Screen &m_screen;
    struct mPy *tPy { nullptr }; // Pointer to py coordinates
    struct mButton *Bo { nullptr }; // Pointer to Button coordinates
};

#endif
