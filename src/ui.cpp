#include <iostream>

#include "ui.h"

UI::UI(std::shared_ptr<Options> ops) : options{std::move(ops)}, pixels(WIDTH * HEIGHT) {
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

bool UI::checkShouldQuit() {
    if (SDL_PollEvent(&event) == 1) {
        if (event.type == SDL_QUIT) {
            return true;
        }
    }

    return false;
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