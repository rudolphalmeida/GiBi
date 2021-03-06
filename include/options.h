#ifndef GIBI_OPTIONS_H
#define GIBI_OPTIONS_H

#include <string>

struct Options {
    std::string romPath{};
    std::string savePath{};

    bool disableWindows = false;
    bool disableBackground = false;
    bool disableSprites = false;

    // If set we use the greenish color shades that were seen on the original GameBoy
    bool useOriginalColorPalette = false;

    int scaleFactor = 1;
};

#endif  // GIBI_OPTIONS_H
