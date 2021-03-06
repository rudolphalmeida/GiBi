/*
 * Author: Rudolph Almeida <rudolf1.almeida@gmail.com>
 * */

#include <iostream>
#include <string>

#include <SDL.h>
#include <args.hxx>

#include "GameBoy.h"
#include "options.h"
#include "ppu/ppu.h"

std::vector<byte> readBinaryToVec(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    // Don't eat newlines in binary mode
    file.unsetf(std::ios::skipws);

    file.seekg(0, std::ios::end);
    std::streampos fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<byte> data;
    data.reserve(fileSize);

    // Read the actual data
    data.insert(data.begin(), std::istream_iterator<byte>(file), std::istream_iterator<byte>());

    return data;
}

GameBoy::GameBoy(int argc, const char** argv)
    : pixels(WIDTH * HEIGHT), options{parseCommandLine(argc, argv)} {
    initComponents();
    initRendering();
}

void GameBoy::initComponents() {
    // Read data and create cartridge and insert into bus
    auto romData = readBinaryToVec(options->romPath);
    std::vector<byte> saveData{};

    if (!options->savePath.empty()) {
        saveData = readBinaryToVec(options->savePath);
    }

    Cartridge cart(romData, !options->savePath.empty() ? std::optional(saveData) : std::nullopt);

    inte = std::make_shared<IntE>();
    intf = std::make_shared<IntF>();
    bus = std::make_shared<Bus>(std::move(cart), intf, inte);
    ppu = std::make_shared<PPU>(intf, bus, options);
    bus->connectPPU(ppu);
    cpu = CPU(bus);
}

void GameBoy::initRendering() {
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

int GameBoy::run() {
    gameLoop();

    return 0;
}

void GameBoy::gameLoop() {
    while (!shouldQuit) {
        if (SDL_PollEvent(&event) == 1) {
            if (event.type == SDL_QUIT) {
                shouldQuit = true;
                continue;
            }
        }

        // Run the components for one frame
        update();

        auto pixelBuffer = ppu->buffer();
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

        // Render pixel buffer to texture...
        SDL_UpdateTexture(texture, nullptr, pixels.data(), WIDTH * sizeof(uint));

        // and render texture to screen
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, nullptr, nullptr);
        SDL_RenderPresent(renderer);
    }
}

void GameBoy::update() {
    uint cyclesThisFrame = 0;

    while (cyclesThisFrame < CYCLES_PER_FRAME) {
        cyclesThisFrame += tick();
    }
}

uint GameBoy::tick() {
    uint cpuCycles = cpu.tick();

    bus->tick(cpuCycles);
    ppu->tick(cpuCycles);

    return cpuCycles;
}

GameBoy::~GameBoy() {
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

std::shared_ptr<Options> parseCommandLine(int argc, const char** argv) {
    auto options = std::make_shared<Options>();

    // Command-line argument config
    args::ArgumentParser parser("GiBi is a GameBoy Emulator made for fun and learning");
    args::HelpFlag help(parser, "help", "Display this help menu", {'h', "help"});
    args::Positional<std::string> rom(parser, "rom", "Path to ROM file", args::Options::Required);
    args::Positional<std::string> save(parser, "save", "Optional path to save file");
    args::Flag disableWindow(parser, "disableWindow", "Disable the window layer",
                             {'w', "disable-window"});
    args::Flag disableBackground(parser, "disableBackground", "Disable the background layer",
                                 {'b', "disable-background"});
    args::Flag disableSprites(parser, "disableSprites", "Disable the sprite layer",
                              {'s', "disable-sprites"});
    args::Flag useOriginalColorPalette(parser, "originalColorPalette",
                                       "Use the green color palette", {'c', "green-palette"});
    args::ValueFlag<int> scaleFactor(parser, "scale", "Scale factor for window",
                                     {'x', "scale-factor"});

    // Handle command line errors
    try {
        parser.ParseCLI(argc, argv);
    } catch (args::Help&) {
        std::cout << parser;
        std::exit(-1);
    } catch (args::ParseError& e) {
        std::cerr << e.what() << std::endl;
        std::cerr << parser;
        std::exit(1);
    } catch (args::ValidationError& e) {
        std::cerr << e.what() << std::endl;
        std::cerr << parser;
        std::exit(1);
    }

    if (rom) {
        options->romPath = args::get(rom);
    }

    if (save) {
        options->savePath = args::get(save);
    }

    if (disableBackground) {
        options->disableBackground = true;
    }

    if (disableSprites) {
        options->disableSprites = true;
    }

    if (disableWindow) {
        options->disableWindows = true;
    }

    if (useOriginalColorPalette) {
        options->useOriginalColorPalette = true;
    }

    if (scaleFactor) {
        options->scaleFactor = args::get(scaleFactor);
    }

    return options;
}
