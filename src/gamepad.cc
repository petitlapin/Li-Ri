//      (_||_/
//      (    )
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

#include <SDL3/SDL_log.h> // for SDL_LogInfo, SDL_LOG_CATEGORY_APPLICATION

#include "gamepad.h"
#include "utils.h"

void Gamepad::Initialize()
{
    char PathFile[512];
    // Controller
    strcpy(PathFile, "gamecontrollerdb.txt");
    Utils::GetPath(PathFile);
    SDL_AddGamepadMappingsFromFile(PathFile);
    // Check if there is already a controller connected
    m_controller = findController();
}

SDL_Gamepad *Gamepad::findController()
{
    int joystickCount = 0;
    SDL_GetJoysticks(&joystickCount);
    for (int i = 0; i < joystickCount; i++) {
        if (SDL_IsGamepad(i)) {
            return SDL_OpenGamepad(i);
        }
    }

    return nullptr;
}

SDL_JoystickID Gamepad::getControllerInstanceID(SDL_Gamepad *controller)
{
    return SDL_GetJoystickID(SDL_GetGamepadJoystick(controller));
}

void Gamepad::OverrideEvent(SDL_Event &event, int key)
{
    event.type = SDL_EVENT_KEY_DOWN;
    event.key.down = true;
    event.key.key = key;
    event.key.repeat = 0;
}

void Gamepad::GetEvent(SDL_Event &event)
{
    constexpr int DEADZONE = 9000;

    switch (event.type) {
    case SDL_EVENT_GAMEPAD_ADDED:
        if (!m_controller) {
            m_controller = SDL_OpenGamepad(event.cdevice.which);
        }
        break;
    case SDL_EVENT_GAMEPAD_REMOVED:
        if (m_controller && event.cdevice.which == getControllerInstanceID(m_controller)) {
            SDL_CloseGamepad(m_controller);
            m_controller = findController();
        }
        break;
    case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
        if (m_controller && event.cdevice.which == getControllerInstanceID(m_controller)) {
            switch (event.gbutton.button) {
            case SDL_GamepadButton::SDL_GAMEPAD_BUTTON_WEST:
            case SDL_GamepadButton::SDL_GAMEPAD_BUTTON_SOUTH:
                OverrideEvent(event, SDLK_RETURN);
                break;
            case SDL_GamepadButton::SDL_GAMEPAD_BUTTON_NORTH:
            case SDL_GamepadButton::SDL_GAMEPAD_BUTTON_EAST:
                OverrideEvent(event, SDLK_ESCAPE);
                break;
            case SDL_GamepadButton::SDL_GAMEPAD_BUTTON_DPAD_RIGHT:
                OverrideEvent(event, SDLK_RIGHT);
                break;
            case SDL_GamepadButton::SDL_GAMEPAD_BUTTON_DPAD_LEFT:
                OverrideEvent(event, SDLK_LEFT);
                break;
            case SDL_GamepadButton::SDL_GAMEPAD_BUTTON_DPAD_UP:
                OverrideEvent(event, SDLK_UP);
                break;
            case SDL_GamepadButton::SDL_GAMEPAD_BUTTON_DPAD_DOWN:
                OverrideEvent(event, SDLK_DOWN);
                break;
            case SDL_GamepadButton::SDL_GAMEPAD_BUTTON_START:
                OverrideEvent(event, SDLK_P);
                break;
            }
        }
        break;
    case SDL_EVENT_GAMEPAD_AXIS_MOTION:
        if (m_controller && event.gaxis.axis == SDL_GAMEPAD_AXIS_LEFTX) {
            // The deadzone ensures we don't trigger the event if the joystick is not press far enough
            // The m_lastValuesPerDirection ensures we don't have multiple repeats of the same event.
            if (event.gaxis.value > DEADZONE && m_lastValuesPerDirection[SDL_GAMEPAD_AXIS_LEFTX] < DEADZONE) {
                OverrideEvent(event, SDLK_RIGHT);
            }
            else if (event.gaxis.value < -DEADZONE && m_lastValuesPerDirection[SDL_GAMEPAD_AXIS_LEFTX] > -DEADZONE) {
                OverrideEvent(event, SDLK_LEFT);
            }
            m_lastValuesPerDirection[SDL_GAMEPAD_AXIS_LEFTX] = event.gaxis.value;
        }
        else if (m_controller && event.gaxis.axis == SDL_GAMEPAD_AXIS_LEFTY) {
            if (event.gaxis.value > DEADZONE && m_lastValuesPerDirection[SDL_GAMEPAD_AXIS_LEFTY] < DEADZONE) {
                OverrideEvent(event, SDLK_DOWN);
            }
            else if (event.gaxis.value < -DEADZONE && m_lastValuesPerDirection[SDL_GAMEPAD_AXIS_LEFTY] > -DEADZONE) {
                OverrideEvent(event, SDLK_UP);
            }
            m_lastValuesPerDirection[SDL_GAMEPAD_AXIS_LEFTY] = event.gaxis.value;
        }
        break;
    }
}
