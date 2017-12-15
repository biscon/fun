//
// Created by bison on 15-10-2017.
//

#include <SDL_timer.h>
#include <vector>
#include <ios>
#include <fstream>
#include <array>
#include <SDL_log.h>
#include "SDLSystem.h"

uint64 SDLSystem::getPerformanceFreq() {
    return SDL_GetPerformanceFrequency();
}

uint64 SDLSystem::getPerformanceCounter() {
    return SDL_GetPerformanceCounter();
}

std::shared_ptr<std::vector<uint8>> SDLSystem::readFile(std::string path) {
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);
    auto buffer = std::make_shared<std::vector<uint8>>(size);
    if (file.read(reinterpret_cast<char *>(buffer->data()), size))
    {
        return buffer;
    }
    return nullptr;
}

std::string SDLSystem::readTextFile(std::string path) {
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);
    auto buffer = std::vector<char>(size+1);
    if (file.read(buffer.data(), size))
    {
        //SDL_Log("Shader source = %s", buffer.data());
        return std::string(buffer.data());
    }
    return std::string();
}
