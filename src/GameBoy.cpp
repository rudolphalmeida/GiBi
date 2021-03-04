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

GameBoy::GameBoy(int argc, char** argv)
    : pixels(WIDTH * HEIGHT), options{std::make_shared<Options>()} {
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
        romPath = args::get(rom);
    }

    if (save) {
        savePath = args::get(save);
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

    initComponents();
    initRendering();
}

void GameBoy::initComponents() {
    // Read data and create cartridge and insert into bus
    auto romData = readBinaryToVec(romPath);
    std::vector<byte> saveData{};

    if (!savePath.empty()) {
        saveData = readBinaryToVec(savePath);
    }

    Cartridge cart(romData, !savePath.empty() ? std::optional(saveData) : std::nullopt);

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

    window = SDL_CreateWindow("GiBi - GameBoy Emulator", 100, 100, WIDTH * SCALE_FACTOR,
                              HEIGHT * SCALE_FACTOR, SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI);
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
                       [](DisplayColor color) {
                           byte greyValue;

                           // TODO: Make this mapping user configurable
                           switch (color) {
                               case DisplayColor::White: {
                                   greyValue = 255;
                                   break;
                               }
                               case DisplayColor::LightGray: {
                                   greyValue = 170;
                                   break;
                               }
                               case DisplayColor::DarkGray: {
                                   greyValue = 85;
                                   break;
                               }
                               case DisplayColor::Black: {
                                   greyValue = 0;
                                   break;
                               }
                           }

                           return (greyValue << 24) | (greyValue << 16) | (greyValue << 8) | 0xFF;
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
