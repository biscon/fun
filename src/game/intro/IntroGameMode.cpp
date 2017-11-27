//
// Created by bison on 27-09-2017.
//

#include <SDL_log.h>

#include <utility>
#include <ctime>
#include "IntroGameMode.h"
#include "../util/string_util.h"

IntroGameMode::IntroGameMode(std::shared_ptr<IGame> game) : IGameMode(std::move(game)) {

}

std::string IntroGameMode::getName() {
    return name;
}

bool IntroGameMode::isInit() {
    return initialized;
}

bool IntroGameMode::init() {
    SDL_Log("IntroGameMode::init");
    initialized = true;

    game->getAssetLoader()->clear();


    game->getAssetLoader()->addLoadTask([](std::shared_ptr<IGame> g){
        g->getAssetLoader();
    }, [](std::shared_ptr<IGame> g) {

    });

    texture1 = std::make_shared<TextureAsset>("test.png");
    game->getAssetLoader()->add(texture1);
    texture2 = std::make_shared<TextureAsset>("test2.png");
    game->getAssetLoader()->add(texture2);
    texture3 = std::make_shared<TextureAsset>("test2.png");
    game->getAssetLoader()->add(texture3);
    texture4 = std::make_shared<TextureAsset>("checker.png");
    game->getAssetLoader()->add(texture4);
    texture5 = std::make_shared<TextureAsset>("font9x14.png");
    game->getAssetLoader()->add(texture5);
    texture6 = std::make_shared<TextureAsset>("bedstead12x20.png");
    game->getAssetLoader()->add(texture6);


    textures.push_back(std::make_shared<TextureAsset>("bacon1.png"));
    game->getAssetLoader()->add(textures.back());
    textures.push_back(std::make_shared<TextureAsset>("bacon2.png"));
    game->getAssetLoader()->add(textures.back());
    textures.push_back(std::make_shared<TextureAsset>("bacon3.png"));
    game->getAssetLoader()->add(textures.back());
    textures.push_back(std::make_shared<TextureAsset>("bacon4.png"));
    game->getAssetLoader()->add(textures.back());

    font1 = std::make_shared<FontAsset>("font9x14.png", 9, 14);
    game->getAssetLoader()->add(font1);

    font2 = std::make_shared<FontAsset>("bedstead12x20.png", 12, 20);
    game->getAssetLoader()->add(font2);


    game->getAssetLoader()->load();

    time_t t;
    srand((unsigned) time(&t));


    return true;
}

void IntroGameMode::shutdown() {
    SDL_Log("IntroGameMode::shutdown");
}

void IntroGameMode::resume() {
    SDL_Log("IntroGameMode::resume");
    game->getInput()->addKeyboardEventListener(this);
}

void IntroGameMode::pause() {
    SDL_Log("IntroGameMode::pause");
    game->getInput()->removeKeyboardEventListener(this);
}

static bool fuckme = true;
void IntroGameMode::update() {
    if(!fuckme)
    {
        fuckme = true;
        game->pushMode("MenuGameMode");
    }
}

void IntroGameMode::fixedUpdate() {

    game->getRenderer()->getQuadRenderer().drawTexturedQuad(texture1->getHandle(), 0, 0, 1280, 360, 0);
    game->getRenderer()->getQuadRenderer().drawTexturedQuad(texture2->getHandle(), 0, 360, 1280, 720, 0);
    game->getRenderer()->getQuadRenderer().drawTexturedQuad(texture4->getHandle(), 100, 100, 300, 240, 1);
    //game->getRenderer()->getQuadRenderer().drawTexturedQuad(texture6->getHandle(), 600, 100, 1280, 540, 1);
    game->getRenderer()->getQuadRenderer().drawTexturedQuad(texture6->getHandle(), 600, 100, 792, 420, 2);

    game->getRenderer()->renderText(font1, 50, 50, 2, "The quick brown fox jumps over the lazy dog");
    game->getRenderer()->renderText(font2, 50, 680, 2, "4 different types of bacon 10000 times each second");

    int index = 0;
    for(int i = 0; i < 10000; i++) {
        int x = (rand() % 1280)+1;
        int y = (rand() % 720)+1;
        int w = (rand() % 300)+1;
        int h = (rand() % 300)+1;

        game->getRenderer()->getQuadRenderer().drawTexturedQuad(textures.at(index)->getHandle(), x, y, x+w, y+h, 0);
        index++;
        if(index > 3)
            index = 0;
    }

}

static double decelerate(double input)
{
    return 1.0 - (1.0 - input) * (1.0 - input);
    //return input;
}

void IntroGameMode::onKeyDown(const SDL_Event *event) {
    if(event->key.keysym.sym == SDLK_d)
    {
        movingRight = true;
        return;
    }
    if(event->key.keysym.sym == SDLK_a)
    {
        movingLeft = true;
        return;
    }
    if(event->key.keysym.sym == SDLK_w)
    {
        movingUp = true;
        return;
    }
    if(event->key.keysym.sym == SDLK_s)
    {
        movingDown = true;
        return;
    }
    if(event->key.keysym.sym == SDLK_q)
    {
        zoomingIn = true;
        return;
    }
    if(event->key.keysym.sym == SDLK_e)
    {
        zoomingOut = true;
        return;
    }

    if(event->key.keysym.sym == SDLK_SPACE)
    {
        return;
    }
}

void IntroGameMode::onKeyUp(const SDL_Event *event) {
    if(event->key.keysym.sym == SDLK_d)
    {
        movingRight = false;
        return;
    }
    if(event->key.keysym.sym == SDLK_a)
    {
        movingLeft = false;
        return;
    }
    if(event->key.keysym.sym == SDLK_w)
    {
        movingUp = false;
        return;
    }
    if(event->key.keysym.sym == SDLK_s)
    {
        movingDown = false;
        return;
    }
    if(event->key.keysym.sym == SDLK_q)
    {
        zoomingIn = false;
        return;
    }
    if(event->key.keysym.sym == SDLK_e)
    {
        zoomingOut = false;
        return;
    }
}

void IntroGameMode::onKeyPressed(const SDL_Event *event) {

}

