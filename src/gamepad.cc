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

#include <SDL2/SDL_log.h> // for SDL_LogInfo, SDL_LOG_CATEGORY_APPLICATION

#include "gamepad.h"
#include "utils.h"

void Gamepad::Initialize()
{
    char PathFile[512];
    // Controller
    strcpy(PathFile, "gamecontrollerdb.txt");
    Utils::GetPath(PathFile);
    SDL_GameControllerAddMappingsFromFile(PathFile);
    // Check if there is already a controller connected
    m_controller = findController();
}

SDL_GameController *Gamepad::findController()
{
    for (int i = 0; i < SDL_NumJoysticks(); i++) {
        if (SDL_IsGameController(i)) {
            return SDL_GameControllerOpen(i);
        }
    }

    return nullptr;
}

SDL_JoystickID Gamepad::getControllerInstanceID(SDL_GameController *controller)
{
    return SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(controller));
}

void Gamepad::OverrideEvent(SDL_Event &event, int key)
{
    event.type = SDL_KEYDOWN;
    event.key.state = SDL_PRESSED;
    event.key.keysym.sym = key;
    event.key.repeat = 0;
}

void Gamepad::GetEvent(SDL_Event &event)
{
    constexpr int DEADZONE = 9000;

    switch (event.type) {
    case SDL_CONTROLLERDEVICEADDED:
        if (!m_controller) {
            m_controller = SDL_GameControllerOpen(event.cdevice.which);
        }
        break;
    case SDL_CONTROLLERDEVICEREMOVED:
        if (m_controller && event.cdevice.which == getControllerInstanceID(m_controller)) {
            SDL_GameControllerClose(m_controller);
            m_controller = findController();
        }
        break;
    case SDL_CONTROLLERBUTTONDOWN:
        if (m_controller && event.cdevice.which == getControllerInstanceID(m_controller)) {
            switch (event.cbutton.button) {
            case SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_X:
            case SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_A:
                OverrideEvent(event, SDLK_RETURN);
                break;
            case SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_Y:
            case SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_B:
                OverrideEvent(event, SDLK_ESCAPE);
                break;
            case SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
                OverrideEvent(event, SDLK_RIGHT);
                break;
            case SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_DPAD_LEFT:
                OverrideEvent(event, SDLK_LEFT);
                break;
            case SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_DPAD_UP:
                OverrideEvent(event, SDLK_UP);
                break;
            case SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_DPAD_DOWN:
                OverrideEvent(event, SDLK_DOWN);
                break;
            case SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_START:
                OverrideEvent(event, SDLK_p);
                break;
            }
        }
        break;
    case SDL_CONTROLLERAXISMOTION:
        if (m_controller && event.caxis.axis == SDL_CONTROLLER_AXIS_LEFTX) {
            // The deadzone ensures we don't trigger the event if the joystick is not press far enough
            // The m_lastValuesPerDirection ensures we don't have multiple repeats of the same event.
            if (event.caxis.value > DEADZONE && m_lastValuesPerDirection[SDL_CONTROLLER_AXIS_LEFTX] < DEADZONE) {
                OverrideEvent(event, SDLK_RIGHT);
            }
            else if (event.caxis.value < -DEADZONE && m_lastValuesPerDirection[SDL_CONTROLLER_AXIS_LEFTX] > -DEADZONE) {
                OverrideEvent(event, SDLK_LEFT);
            }
            m_lastValuesPerDirection[SDL_CONTROLLER_AXIS_LEFTX] = event.caxis.value;
        }
        else if (m_controller && event.caxis.axis == SDL_CONTROLLER_AXIS_LEFTY) {
            if (event.caxis.value > DEADZONE && m_lastValuesPerDirection[SDL_CONTROLLER_AXIS_LEFTY] < DEADZONE) {
                OverrideEvent(event, SDLK_DOWN);
            }
            else if (event.caxis.value < -DEADZONE && m_lastValuesPerDirection[SDL_CONTROLLER_AXIS_LEFTY] > -DEADZONE) {
                OverrideEvent(event, SDLK_UP);
            }
            m_lastValuesPerDirection[SDL_CONTROLLER_AXIS_LEFTY] = event.caxis.value;
        }
        break;
    }
}
