/*
 * Author: Rudolph Almeida <rudolf1.almeida@gmail.com>
 * */

#include "GameBoy.h"

std::vector<byte> readBinaryToVec(const char* filename) {
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

GameBoy::GameBoy(int argc, char** argv) {}
