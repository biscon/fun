#include <iostream>
#include <SDL.h>
#include <chrono>
#include "Game.h"
#include "Factory.h"

// needed because mingw's c++11 thread implementation isn't complete on windows (gcc on osx and linux are fine ofc)
#ifdef __WIN32__
    #include "mingw.thread.h"
#endif

// workaround for missing WinMain bullshit (SDL encapsulates the program entry point since it differs on platforms)
#ifdef main
# undef main
#endif /* main */

int main(int argc, char *argv[]) {

    u8 ao = 2;
    u8 is_tex = 0;
    u8 face_no = 5;

    SDL_Log("before ao = %d is_tex = %d face_no = %d", ao, is_tex, face_no);

    u8 packed = (face_no << 3) | (ao << 1) | is_tex;
    SDL_Log("Packed value: %d", packed);

    is_tex = packed & 0x1;
    ao = (packed & 0x6) >> 1;
    face_no = (packed & 0x38) >> 3;
    SDL_Log("after ao = %d is_tex = %d face_no = %d", ao, is_tex, face_no);
    //exit(0);

    SDL_Log("Initializing...\n");

    // GDB crashes the program immediately when using named threads on windows
    #ifdef __WIN32__
        SDL_SetHint(SDL_HINT_WINDOWS_DISABLE_THREAD_NAMING, "1");
    #endif
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0 )
    {
        SDL_Log("SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
        return -1;
    }

    auto system = Factory::createSystem();

    auto assetLoader = Factory::createAssetLoader();
    if(assetLoader == nullptr)
    {
        SDL_Log("Could not create asset loader\n");
        SDL_Quit();
        return -1;
    }

    auto input = Factory::createInput();
    if(input == nullptr)
    {
        SDL_Log("could not create input\n");
        SDL_Quit();
        return -1;
    }

    auto renderer = Factory::createRenderer();
    if(renderer == nullptr)
    {
        SDL_Log("Could not create renderer\n");
        SDL_Quit();
        return -1;
    }
    auto game = Factory::createGame(renderer, input, assetLoader, system);
    if(game == nullptr)
    {
        SDL_Log("Could not create game\n");
        SDL_Quit();
        return -1;
    }
    assetLoader->setGame(game);

    // register gamemodes
    auto introGameMode = Factory::createIntroGameMode(game);
    game->registerGameMode(introGameMode);

    auto menuGameMode = Factory::createMenuGameMode(game);
    game->registerGameMode(menuGameMode);

    // run the game
    game->run();

    SDL_Quit();
    SDL_Log("All done, exiting");
    return 0;
}
