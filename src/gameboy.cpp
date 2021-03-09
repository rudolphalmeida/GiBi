/*
 * Author: Rudolph Almeida <rudolf1.almeida@gmail.com>
 * */

#include <iostream>
#include <string>

#include <SDL.h>
#include <args.hxx>

#include "gameboy.h"
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

GameBoy::GameBoy(int argc, const char** argv) : options{parseCommandLine(argc, argv)}, ui(options) {
    initComponents();
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

int GameBoy::run() {
    gameLoop();

    return 0;
}

void GameBoy::gameLoop() {
    while (!shouldQuit) {
        if (ui.checkShouldQuit()) {
            shouldQuit = true;
            continue;
        }

        // Run the components for one frame
        update();

        auto pixelBuffer = ppu->buffer();
        ui.render(pixelBuffer);
//        ppu->clearBuffer();
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
