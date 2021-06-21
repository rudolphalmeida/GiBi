#include <algorithm>
#include <iostream>
#include <utility>

#include <SDL_gamecontroller.h>

#include "ui.h"

UI::UI(std::shared_ptr<Options> ops, std::shared_ptr<Bus> bus)
    : options{std::move(ops)}, bus{std::move(bus)}, pixels(WIDTH * HEIGHT) {
    // Graphics init
    SDL_SetMainReady();
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) != 0) {
        std::cerr << "SDL_Init error: " << SDL_GetError() << "\n";
        std::exit(1);
    }

    window = SDL_CreateWindow("GiBi - GameBoy Emulator", 100, 100, WIDTH * options->scaleFactor,
                              HEIGHT * options->scaleFactor, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "SDL_CreateWindow error: " << SDL_GetError() << "\n";
        SDL_Quit();
        std::exit(1);
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        SDL_DestroyWindow(window);
        std::cerr << "SDL_CreateRenderer error: " << SDL_GetError() << "\n";
        SDL_Quit();
        std::exit(1);
    }

    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STATIC, WIDTH,
                                HEIGHT);

    // Check for any connected joysticks
    if (SDL_NumJoysticks() < 1) {
        std::cerr << "Warning: No joysticks connected...\n";
    } else {
        usingJoystick = true;
        gameController = SDL_GameControllerOpen(0);  // Open the first connected joystick

        if (SDL_GameControllerGetAttached(gameController) == SDL_FALSE) {
            SDL_GameControllerClose(gameController);
            std::cerr << "Error: Failed to open joystick! SDL Error: " << SDL_GetError() << "\n";
            gameController = nullptr;
            usingJoystick = false;
        } else {
            std::cerr << "Connect to joystick...\n";
        }
    }
}

void UI::handleEvents() {
    if (SDL_PollEvent(&event) == 1) {
        switch (event.type) {
            case SDL_QUIT:
                shouldQuit = true;
                break;
            case SDL_KEYDOWN:
                keyboardButtonDown(event.key.keysym.sym);
                break;
            case SDL_KEYUP:
                keyboardButtonUp(event.key.keysym.sym);
                break;
            case SDL_CONTROLLERBUTTONDOWN:
                // Check if we are using a joystick and the press came from the connected one
                if (usingJoystick &&
                    event.cbutton.which ==
                        SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(gameController))) {
                    joystickButtonDown(event.cbutton.button);
                }
                break;
            case SDL_CONTROLLERBUTTONUP:
                std::cerr << "Here!\n";
                if (usingJoystick &&
                    event.cbutton.which ==
                        SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(gameController))) {
                    joystickButtonUp(event.cbutton.button);
                }
                break;
            case SDL_CONTROLLERDEVICEREMOVED:
                std::cerr << "Controller removed...\n";
                SDL_GameControllerClose(gameController);
                gameController = nullptr;
                usingJoystick = false;
                break;
        }
    }
}

void UI::keyboardButtonDown(SDL_Keycode sym) {
    switch (sym) {
        case SDLK_LEFT:
            bus->getJoyPad().keydown(JoypadKeys::Left);
            break;
        case SDLK_RIGHT:
            bus->getJoyPad().keydown(JoypadKeys::Right);
            break;
        case SDLK_UP:
            bus->getJoyPad().keydown(JoypadKeys::Up);
            break;
        case SDLK_DOWN:
            bus->getJoyPad().keydown(JoypadKeys::Down);
            break;
        case SDLK_z:
            bus->getJoyPad().keydown(JoypadKeys::B);
            break;
        case SDLK_x:
            bus->getJoyPad().keydown(JoypadKeys::A);
            break;
        case SDLK_n:
            bus->getJoyPad().keydown(JoypadKeys::Select);
            break;
        case SDLK_m:
            bus->getJoyPad().keydown(JoypadKeys::Start);
            break;
        default:
            break;
    }
}

void UI::joystickButtonDown(Uint8 button) {
    switch (button) {
        case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
            bus->getJoyPad().keydown(JoypadKeys::Left);
            break;
        case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
            bus->getJoyPad().keydown(JoypadKeys::Right);
            break;
        case SDL_CONTROLLER_BUTTON_DPAD_UP:
            bus->getJoyPad().keydown(JoypadKeys::Up);
            break;
        case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
            bus->getJoyPad().keydown(JoypadKeys::Down);
            break;
        case SDL_CONTROLLER_BUTTON_B:
            bus->getJoyPad().keydown(JoypadKeys::B);
            break;
        case SDL_CONTROLLER_BUTTON_A:
            bus->getJoyPad().keydown(JoypadKeys::A);
            break;
        case SDL_CONTROLLER_BUTTON_BACK:
            bus->getJoyPad().keydown(JoypadKeys::Select);
            break;
        case SDL_CONTROLLER_BUTTON_START:
            bus->getJoyPad().keydown(JoypadKeys::Start);
            break;
        default:
            break;
    }
}

void UI::keyboardButtonUp(SDL_Keycode sym) {
    switch (sym) {
        case SDLK_LEFT:
            bus->getJoyPad().keyup(JoypadKeys::Left);
            break;
        case SDLK_RIGHT:
            bus->getJoyPad().keyup(JoypadKeys::Right);
            break;
        case SDLK_UP:
            bus->getJoyPad().keyup(JoypadKeys::Up);
            break;
        case SDLK_DOWN:
            bus->getJoyPad().keyup(JoypadKeys::Down);
            break;
        case SDLK_z:
            bus->getJoyPad().keyup(JoypadKeys::B);
            break;
        case SDLK_a:
            bus->getJoyPad().keyup(JoypadKeys::A);
            break;
        case SDLK_n:
            bus->getJoyPad().keyup(JoypadKeys::Select);
            break;
        case SDLK_m:
            bus->getJoyPad().keyup(JoypadKeys::Start);
            break;
        default:
            break;
    }
}

void UI::joystickButtonUp(Uint8 button) {
    switch (button) {
        case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
            bus->getJoyPad().keyup(JoypadKeys::Left);
            break;
        case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
            bus->getJoyPad().keyup(JoypadKeys::Right);
            break;
        case SDL_CONTROLLER_BUTTON_DPAD_UP:
            bus->getJoyPad().keyup(JoypadKeys::Up);
            break;
        case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
            bus->getJoyPad().keyup(JoypadKeys::Down);
            break;
        case SDL_CONTROLLER_BUTTON_B:
            bus->getJoyPad().keyup(JoypadKeys::B);
            break;
        case SDL_CONTROLLER_BUTTON_A:
            bus->getJoyPad().keyup(JoypadKeys::A);
            break;
        case SDL_CONTROLLER_BUTTON_BACK:
            bus->getJoyPad().keyup(JoypadKeys::Select);
            break;
        case SDL_CONTROLLER_BUTTON_START:
            bus->getJoyPad().keyup(JoypadKeys::Start);
            break;
        default:
            break;
    }
}

void UI::toActualColorPixels(const std::vector<DisplayColor>& pixelBuffer) {
    std::transform(pixelBuffer.cbegin(), pixelBuffer.cend(), pixels.begin(),
                   [this](DisplayColor color) { return displayColorToColorShade(color); });
}

uint UI::displayColorToColorShade(const DisplayColor& color) const {
    switch (color) {
        case DisplayColor::White: {
            if (options->useOriginalColorPalette) {
                return static_cast<uint>(ColorShade::GreenWhite);
            } else {
                return static_cast<uint>(ColorShade::White);
            }
        }
        case DisplayColor::LightGray: {
            if (options->useOriginalColorPalette) {
                return static_cast<uint>(ColorShade::GreenLight);
            } else {
                return static_cast<uint>(ColorShade::LightGray);
            }
        }
        case DisplayColor::DarkGray: {
            if (options->useOriginalColorPalette) {
                return static_cast<uint>(ColorShade::GreenDark);
            } else {
                return static_cast<uint>(ColorShade::DarkGray);
            }
        }
        case DisplayColor::Black: {
            if (options->useOriginalColorPalette) {
                return static_cast<uint>(ColorShade::GreenBlack);
            } else {
                return static_cast<uint>(ColorShade::Black);
            }
        }
    }

    return static_cast<uint>(ColorShade::White);  // Not needed
}

void UI::render(const std::vector<DisplayColor>& pixelBuffer) {
    toActualColorPixels(pixelBuffer);

    // Render pixel buffer to texture...
    SDL_UpdateTexture(texture, nullptr, pixels.data(), WIDTH * sizeof(uint));

    // and render texture to screen
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);
}

UI::~UI() {
    // Close connected controllers
    SDL_GameControllerClose(gameController);
    gameController = nullptr;

    // Close graphics subsystem
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
