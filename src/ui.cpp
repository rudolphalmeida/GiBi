#include <iostream>
#include <utility>

#include "ui.h"

UI::UI(std::shared_ptr<Options> ops, std::shared_ptr<Bus> bus)
    : options{std::move(ops)}, bus{std::move(bus)}, pixels(WIDTH * HEIGHT) {
    // Graphics init
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
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
}

void UI::handleEvents() {
    if (SDL_PollEvent(&event) == 1) {
        switch (event.type) {
            case SDL_QUIT:
                shouldQuit = true;
                break;
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
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
                    case SDLK_a:
                        bus->getJoyPad().keydown(JoypadKeys::A);
                        break;
                    case SDLK_n:
                        bus->getJoyPad().keydown(JoypadKeys::Select);
                        break;
                    case SDLK_m:
                        bus->getJoyPad().keydown(JoypadKeys::Start);
                        break;
                }
                break;
            case SDL_KEYUP:
                switch (event.key.keysym.sym) {
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
                }
                break;
        }
    }
}

void UI::toActualColor(const std::vector<DisplayColor>& pixelBuffer) {
    std::transform(pixelBuffer.cbegin(), pixelBuffer.cend(), pixels.begin(),
                   [this](DisplayColor color) {
                       byte r, g, b;

                       switch (color) {
                           case DisplayColor::White: {
                               if (options->useOriginalColorPalette) {
                                   r = 255;
                                   g = 188;
                                   b = 15;
                               } else {
                                   r = g = b = 255;
                               }
                               break;
                           }
                           case DisplayColor::LightGray: {
                               if (options->useOriginalColorPalette) {
                                   r = 139;
                                   g = 172;
                                   b = 15;
                               } else {
                                   r = g = b = 170;
                               }
                               break;
                           }
                           case DisplayColor::DarkGray: {
                               if (options->useOriginalColorPalette) {
                                   r = 48;
                                   g = 98;
                                   b = 48;
                               } else {
                                   r = g = b = 85;
                               }
                               break;
                           }
                           case DisplayColor::Black: {
                               if (options->useOriginalColorPalette) {
                                   r = 15;
                                   g = 56;
                                   b = 15;
                               } else {
                                   r = g = b = 0;
                               }
                               break;
                           }
                       }

                       return (uint(r) << 24) | (uint(g) << 16) | (uint(b) << 8) | 0xFF;
                   });
}

void UI::render(const std::vector<DisplayColor>& pixelBuffer) {
    toActualColor(pixelBuffer);

    // Render pixel buffer to texture...
    SDL_UpdateTexture(texture, nullptr, pixels.data(), WIDTH * sizeof(uint));

    // and render texture to screen
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);
}

UI::~UI() {
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
