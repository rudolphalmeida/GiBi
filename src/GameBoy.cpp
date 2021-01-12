/*
 * Author: Rudolph Almeida <rudolf1.almeida@gmail.com>
 * */

#include <iostream>
#include <string>

#include <args.hxx>

#include "GameBoy.h"

std::vector<byte> readBinaryToVec(std::string filename) {
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

GameBoy::GameBoy(int argc, char** argv) {
    // Command-line argument config
    args::ArgumentParser parser("GiBi is a GameBoy Emulator made for fun and learning");
    args::HelpFlag help(parser, "help", "Display this help menu", {'h', "help"});
    args::Positional<std::string> rom(parser, "rom", "Path to ROM file");
    args::Positional<std::string> save(parser, "save", "Path to save file");

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

    // ROM file is required
    if (rom) {
        romPath = args::get(rom);
    } else {
        std::cout << parser;
        std::exit(-1);
    }

    // Save file path is optional
    if (save) {
        savePath = args::get(save);
    }

    initComponents();
}

void GameBoy::initComponents() {
    // Read data and create cartridge and insert into bus
    auto romData = readBinaryToVec(romPath);
    std::vector<byte> saveData{};

    if (!savePath.empty()) {
        saveData = readBinaryToVec(savePath);
    }

    Cartridge cart(romData, !savePath.empty() ? std::optional(saveData) : std::nullopt);
    bus = std::make_shared<Bus>(std::move(cart));
    cpu = CPU(bus);
}

int GameBoy::run(){
return 0;
}
