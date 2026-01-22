//      (_||_/
//      (    )       Gamepad class
//     ( o  0 )
//-OOO°--(_)---°OOO---------------------------------------
//                   Copyright (C) 2026 By Johnny Jazeix
// .OOOo      oOOO.  jazeix@gmail.com
//-(   )------(   )---------------------------------------
//  ( (        ) /
//   (_)      (_/

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

#ifndef GAMEPAD_DOM_
#define GAMEPAD_DOM_

#include <SDL3/SDL_events.h>
#include <SDL3/SDL_gamepad.h>

#include <unordered_map>

class Gamepad
{
public:
    ~Gamepad() = default;

    void Initialize();
    void GetEvent(SDL_Event &event);

private:
    std::unordered_map<int, int> m_lastValuesPerDirection;
    SDL_Gamepad *findController();
    SDL_JoystickID getControllerInstanceID(SDL_Gamepad *controller);
    SDL_Gamepad *m_controller = nullptr;
    void OverrideEvent(SDL_Event &event, int key);
};

#endif
