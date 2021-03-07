/*
 * Handles the user interface - rendering, user input and windowing
 *
 * Author: Rudolph Almeida <rudolf1.almeida@gmail.com>
 * */

#ifndef GIBI_UI_H
#define GIBI_UI_H

#include <memory>
#include <vector>

#include <SDL.h>

#include "options.h"
#include "ppu/ppu.h"

class UI {
   private:
    std::shared_ptr<Options> options;

    const int WIDTH = PPU::LCD_WIDTH;
    const int HEIGHT = PPU::LCD_HEIGHT;

    SDL_Window* window{};
    SDL_Renderer* renderer{};
    SDL_Texture* texture{};

    std::vector<uint> pixels;

    SDL_Event event{};

    // Map the vector of GameBoy colors to on-screen colors
    // FIXME: The background whites are mapped to a yellowish-green color
    void toActualColor(const std::vector<DisplayColor>& pixelBuffer);

   public:
    explicit UI(std::shared_ptr<Options> ops);

    bool checkShouldQuit();

    void render(const std::vector<DisplayColor>& pixelBuffer);

    ~UI();
};

#endif  // GIBI_UI_H
