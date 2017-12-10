//
// Created by bison on 27-09-2017.
//

#include <SDL_log.h>

#include <utility>
#include <ctime>
#include "IntroGameMode.h"

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
    /*
    game->getAssetLoader()->addLoadTask([this](std::shared_ptr<IGame> g){

    }, [this](std::shared_ptr<IGame> g) {

    });
     */
    fontRenderer = std::unique_ptr<FontRenderer>(new FontRenderer());
    font1 = fontRenderer->addFont("font9x14.png", 9, 14);
    font2 = fontRenderer->addFont("bedstead12x20.png", 12, 20);
    game->getAssetLoader()->addLoadTask(fontRenderer.get());

    textureAtlas = std::make_shared<TextureAtlas>(2048, 2048);
    quadRenderer = std::unique_ptr<QuadRenderer>(new QuadRenderer(textureAtlas));

    cubeRenderer = std::unique_ptr<CubeRenderer>(new CubeRenderer(textureAtlas));

    game->getAssetLoader()->addLoadTask(this);
    game->getAssetLoader()->load();

    time_t t;
    srand((unsigned) time(&t));


    return true;
}

bool IntroGameMode::load(IGame &game) {
    SDL_Log("IntroGameMode::load");
    textures.push_back(textureAtlas->addBuffer(std::make_shared<PixelBuffer>("bacon1.png")));
    textures.push_back(textureAtlas->addBuffer(std::make_shared<PixelBuffer>("bacon2.png")));
    textures.push_back(textureAtlas->addBuffer(std::make_shared<PixelBuffer>("bacon3.png")));
    textures.push_back(textureAtlas->addBuffer(std::make_shared<PixelBuffer>("bacon4.png")));
    textureAtlas->build();
    return true;
}

bool IntroGameMode::prepare(IGame &game) {
    SDL_Log("IntroGameMode::prepare");
    textureAtlas->upload();
    quadRenderer->buildBuffers();
    cubeRenderer->buildBuffers();
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
    quadRenderer->startFrame();
    int index = 0;
    for(int i = 0; i < 10000; i++) {
        int x = (rand() % 1280)+1;
        int y = (rand() % 720)+1;
        int w = (rand() % 300)+1;
        int h = (rand() % 300)+1;

        quadRenderer->drawTexturedQuad(textures.at(index), x, y, x+w, y+h, 0);
        index++;
        if(index > 3)
            index = 0;
    }
    quadRenderer->render(1280, 720);

    fontRenderer->startFrame();
    fontRenderer->renderText(font1, 50, 50, 2, "The quick brown fox jumps over the lazy dog");
    fontRenderer->renderText(font2, 50, 680, 2, "4 different types of bacon 10000 times each second");
    fontRenderer->render(1280, 720);
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


