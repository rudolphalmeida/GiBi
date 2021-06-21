/*
 * Handles the user interface - rendering, user input and windowing
 *
 * Author: Rudolph Almeida <rudolf1.almeida@gmail.com>
 * */

#ifndef GIBI_UI_H
#define GIBI_UI_H

#include <memory>
#include <vector>

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

#include "mmu/bus.h"
#include "options.h"
#include "ppu/ppu.h"

class UI {
   private:
    std::shared_ptr<Options> options;
    std::shared_ptr<Bus> bus;

    const int WIDTH = PPU::LCD_WIDTH;
    const int HEIGHT = PPU::LCD_HEIGHT;

    SDL_Window* window{};
    SDL_Renderer* renderer{};
    SDL_Texture* texture{};

    bool usingJoystick{false};
    SDL_GameController* gameController{};

    std::vector<uint> pixels;

    bool shouldQuit{};
    SDL_Event event{};

    enum class ColorShade : uint {
        Black = 0x000000FF,
        DarkGray = 0x555555FF,
        LightGray = 0xAAAAAAFF,
        White = 0xFFFFFFFF,
        GreenBlack = 0x0F380FFF,
        GreenDark = 0x306230FF,
        GreenLight = 0x8BAC0FFF,
        GreenWhite = 0xFFBC0FFF
    };

    // Map the vector of GameBoy colors to on-screen colors
    // FIXME: The background whites are mapped to a yellowish-green color
    void toActualColorPixels(const std::vector<DisplayColor>& pixelBuffer);

    [[nodiscard]] uint displayColorToColorShade(const DisplayColor& color) const;

   public:
    explicit UI(std::shared_ptr<Options> ops, std::shared_ptr<Bus> bus = nullptr);

    void handleEvents();
    void keyboardButtonDown(SDL_Keycode);
    void keyboardButtonUp(SDL_Keycode);
    void joystickButtonDown(Uint8);
    void joystickButtonUp(Uint8);

    void render(const std::vector<DisplayColor>& pixelBuffer);

    [[nodiscard]] bool quit() const { return shouldQuit; }

    void connectBus(std::shared_ptr<Bus> b) { this->bus = std::move(b); }

    ~UI();
};

#endif  // GIBI_UI_H
