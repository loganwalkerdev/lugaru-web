/*
Copyright (C) 2003, 2010 - Wolfire Games
Copyright (C) 2010-2017 - Lugaru contributors (see AUTHORS file)

This file is part of Lugaru, maintained as part of the Loupgarenne fork.
See README and AUTHORS for project details.

Lugaru is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

Lugaru is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Lugaru.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "Utils/Input.hpp"

bool keyDown[SDL_NUM_SCANCODES + 6];
bool keyPressed[SDL_NUM_SCANCODES + 6];

namespace {
SDL_GameController* controller = nullptr;
SDL_JoystickID controllerInstance = -1;
bool controllerSubsystemReady = false;
Uint32 nextControllerScan = 0;
Sint16 controllerAxes[SDL_CONTROLLER_AXIS_MAX] = {};
bool controllerButtons[SDL_CONTROLLER_BUTTON_MAX] = {};
int loggedAxes[SDL_CONTROLLER_AXIS_MAX] = {};
int controllerMoveX = 0;
int controllerMoveY = 0;
// Combined action history belongs to the input tick, not controller lifecycle:
// disconnecting must not invent a new press while keyboard/mouse is still held.
bool previousAttackDown = false;
bool attackPressed = false;

int digitalMovementAxis(Sint16 value, int previous)
{
    // Independent axis hysteresis permits diagonals without proportional speed.
    constexpr int activate = 10000;
    constexpr int release = 7000;
    if (value <= -activate) {
        return -1;
    }
    if (value >= activate) {
        return 1;
    }
    if (previous < 0 && value < -release) {
        return -1;
    }
    if (previous > 0 && value > release) {
        return 1;
    }
    return 0;
}

void clearControllerState()
{
    controllerMoveX = 0;
    controllerMoveY = 0;
    for (int i = 0; i < SDL_CONTROLLER_AXIS_MAX; ++i) {
        controllerAxes[i] = 0;
        loggedAxes[i] = 0;
    }
    for (int i = 0; i < SDL_CONTROLLER_BUTTON_MAX; ++i) {
        controllerButtons[i] = false;
    }
}
}

void Input::InitController()
{
    if (controllerSubsystemReady) {
        return;
    }
    if (SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER) < 0) {
        SDL_Log("Controller: initialization failed: %s", SDL_GetError());
        return;
    }
    controllerSubsystemReady = true;
    nextControllerScan = SDL_GetTicks();
    UpdateController();
}

void Input::ShutdownController()
{
    if (controller) {
        SDL_GameControllerClose(controller);
        controller = nullptr;
    }
    controllerInstance = -1;
    clearControllerState();
    if (controllerSubsystemReady) {
        SDL_QuitSubSystem(SDL_INIT_GAMECONTROLLER);
        controllerSubsystemReady = false;
    }
}

void Input::UpdateController()
{
    if (!controllerSubsystemReady) {
        return;
    }
    // Poll independently of event consumers (including native key capture).
    SDL_GameControllerUpdate();
    if (controller && !SDL_GameControllerGetAttached(controller)) {
        SDL_Log("Controller: disconnected instance %d", int(controllerInstance));
        SDL_GameControllerClose(controller);
        controller = nullptr;
        controllerInstance = -1;
        clearControllerState();
        nextControllerScan = SDL_GetTicks();
    }
    if (!controller) {
        const Uint32 now = SDL_GetTicks();
        if (!SDL_TICKS_PASSED(now, nextControllerScan)) {
            return;
        }
        // Avoid repeated enumeration/open failures every frame while absent.
        nextControllerScan = now + 1000;
        for (int device = 0; device < SDL_NumJoysticks(); ++device) {
            if (!SDL_IsGameController(device)) {
                continue;
            }
            const char* name = SDL_GameControllerNameForIndex(device);
            SDL_Log("Controller: detected device %d (%s)", device, name ? name : "unknown");
            controller = SDL_GameControllerOpen(device);
            if (!controller) {
                SDL_Log("Controller: open failed: %s", SDL_GetError());
                continue;
            }
            controllerInstance = SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(controller));
            SDL_Log("Controller: opened instance %d (%s)", int(controllerInstance),
                    SDL_GameControllerName(controller) ? SDL_GameControllerName(controller) : "unknown");
            break;
        }
    }
    if (!controller) {
        return;
    }
    for (int i = 0; i < SDL_CONTROLLER_AXIS_MAX; ++i) {
        controllerAxes[i] = SDL_GameControllerGetAxis(controller, SDL_GameControllerAxis(i));
        // Coarse diagnostics only: retain full raw precision in controllerAxes.
        if (SDL_abs(int(controllerAxes[i]) - loggedAxes[i]) >= 8192) {
            loggedAxes[i] = controllerAxes[i];
            SDL_Log("Controller: axis %s = %d", SDL_GameControllerGetStringForAxis(SDL_GameControllerAxis(i)),
                    int(controllerAxes[i]));
        }
    }
    controllerMoveX = digitalMovementAxis(controllerAxes[SDL_CONTROLLER_AXIS_LEFTX], controllerMoveX);
    controllerMoveY = digitalMovementAxis(controllerAxes[SDL_CONTROLLER_AXIS_LEFTY], controllerMoveY);
    for (int i = 0; i < SDL_CONTROLLER_BUTTON_MAX; ++i) {
        const bool down = SDL_GameControllerGetButton(controller, SDL_GameControllerButton(i)) != 0;
        if (down != controllerButtons[i]) {
            SDL_Log("Controller: button %s %s", SDL_GameControllerGetStringForButton(SDL_GameControllerButton(i)),
                    down ? "down" : "up");
        }
        controllerButtons[i] = down;
    }
}

bool Input::hasController()
{
    return controller && SDL_GameControllerGetAttached(controller);
}

Sint16 Input::controllerAxis(SDL_GameControllerAxis axis)
{
    return hasController() && axis >= 0 && axis < SDL_CONTROLLER_AXIS_MAX ? controllerAxes[axis] : 0;
}

bool Input::controllerButtonDown(SDL_GameControllerButton button)
{
    return hasController() && button >= 0 && button < SDL_CONTROLLER_BUTTON_MAX && controllerButtons[button];
}

Input::ControllerMovement Input::controllerMovement()
{
    if (!hasController()) {
        return {};
    }
    return {controllerMoveY < 0, controllerMoveY > 0, controllerMoveX < 0, controllerMoveX > 0};
}

bool Input::isJumpDown()
{
    return isKeyDown(Game::jumpkey) || controllerButtonDown(SDL_CONTROLLER_BUTTON_A);
}

bool Input::isCrouchDown()
{
    return isKeyDown(Game::crouchkey) || controllerButtonDown(SDL_CONTROLLER_BUTTON_B);
}

bool Input::isDrawDown()
{
    return isKeyDown(Game::drawkey) || controllerButtonDown(SDL_CONTROLLER_BUTTON_X);
}

bool Input::isThrowDown()
{
    return isKeyDown(Game::throwkey) || controllerButtonDown(SDL_CONTROLLER_BUTTON_LEFTSHOULDER);
}

bool Input::isAttackDown()
{
    return isKeyDown(Game::attackkey) || controllerButtonDown(SDL_CONTROLLER_BUTTON_RIGHTSHOULDER);
}

bool Input::isAttackPressed()
{
    return attackPressed && isAttackDown();
}

void Input::Tick()
{
    SDL_PumpEvents();
    int numkeys;
    const Uint8* keyState = SDL_GetKeyboardState(&numkeys);
    for (int i = 0; i < numkeys; i++) {
        keyPressed[i] = !keyDown[i] && keyState[i];
        keyDown[i] = keyState[i];
    }
    Uint8 mb = SDL_GetMouseState(NULL, NULL);
    for (int i = 1; i < 6; i++) {
        keyPressed[SDL_NUM_SCANCODES + i] = !keyDown[SDL_NUM_SCANCODES + i] && (mb & SDL_BUTTON(i));
        keyDown[SDL_NUM_SCANCODES + i] = (mb & SDL_BUTTON(i));
    }
    // Like keyboard presses, expose the edge for one input/physics tick.
    const bool attackDown = isAttackDown();
    attackPressed = attackDown && !previousAttackDown;
    previousAttackDown = attackDown;
}

bool Input::isKeyDown(int k)
{
    if (k >= SDL_NUM_SCANCODES + 6) {
        return false;
    }
    return keyDown[k];
}

bool Input::isKeyPressed(int k)
{
    if (k >= SDL_NUM_SCANCODES + 6) {
        return false;
    }
    return keyPressed[k];
}

const char* Input::keyToChar(unsigned short i)
{
    if (i < SDL_NUM_SCANCODES) {
        return SDL_GetKeyName(SDL_GetKeyFromScancode(SDL_Scancode(i)));
    } else if (i == MOUSEBUTTON_LEFT) {
        return "mouse left button";
    } else if (i == MOUSEBUTTON_RIGHT) {
        return "mouse right button";
    } else if (i == MOUSEBUTTON_MIDDLE) {
        return "mouse middle button";
    } else if (i == MOUSEBUTTON_X1) {
        return "mouse button 4";
    } else if (i == MOUSEBUTTON_X2) {
        return "mouse button 5";
    } else {
        return "unknown";
    }
}

bool Input::MouseClicked()
{
    return isKeyPressed(MOUSEBUTTON_LEFT);
}
