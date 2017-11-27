#include <iostream>
#include <SDL.h>
#include "../game/Game.h"
#include "Factory.h"

/*
#ifdef __cplusplus
extern "C"
#endif
*/

// workaround for missing WinMain bullshit
#ifdef main
# undef main
#endif /* main */

int main(int argc, char *argv[]) {
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