//
// Created by bison on 27-09-2017.
//

#include <SDL_log.h>

#include <utility>
#include <ctime>
#include <engine/util/string_util.h>
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

    /*
    textureAtlas = std::make_shared<TextureAtlas>(2048, 2048, false);
    quadRenderer = std::unique_ptr<QuadRenderer>(new QuadRenderer(textureAtlas));
    cubeRenderer = std::unique_ptr<CubeRenderer>(new CubeRenderer(textureAtlas));
    */

    camera = std::make_shared<Camera>(glm::vec3(0.0f, 65.0f, 0.0f));
    terrain = std::make_shared<Terrain>();
    //lightSceneRenderer = std::unique_ptr<LightSceneRenderer>(new LightSceneRenderer(*game->getSystem(), camera));

    //model = std::make_shared<Model>("nanosuit/nanosuit.obj", "nanosuit/");
    //lightSceneRenderer->testModel = model;
    //game->getAssetLoader()->addLoadTask(model.get());

    //model2 = std::make_shared<Model>("assets/grass_tile.obj", "assets/");
    //game->getAssetLoader()->addLoadTask(model2.get());

    //modelRenderer = std::unique_ptr<ModelRenderer>(new ModelRenderer(*game->getSystem(), model2, camera));

    chunkRenderer = std::unique_ptr<ChunkRenderer>(new ChunkRenderer(*game->getSystem(), camera, terrain));
    game->getAssetLoader()->addLoadTask(chunkRenderer.get());

    game->getAssetLoader()->addLoadTask(this);
    game->getAssetLoader()->load();

    time_t t;
    srand((unsigned) time(&t));


    return true;
}

// this runs on the loader thread when loading async, do not issue gl calls
bool IntroGameMode::load(IGame &game) {
    SDL_Log("IntroGameMode::load");
    terrain->generate();
    return true;
}

// this runs on the mainthread after loading (use it to upload data and set state with gl)
bool IntroGameMode::prepare(IGame &game) {
    SDL_Log("IntroGameMode::prepare");
    return true;
}

void IntroGameMode::shutdown() {
    SDL_Log("IntroGameMode::shutdown");
}

void IntroGameMode::resume() {
    SDL_Log("IntroGameMode::resume");
    SDL_SetRelativeMouseMode(SDL_TRUE);
    game->getInput()->addKeyboardEventListener(this);
    game->getInput()->addMouseEventListener(this);
}

void IntroGameMode::pause() {
    SDL_Log("IntroGameMode::pause");
    SDL_SetRelativeMouseMode(SDL_FALSE);
    game->getInput()->removeKeyboardEventListener(this);
    game->getInput()->removeMouseEventListener(this);
}

static bool fuckme = true;
void IntroGameMode::update() {
    if(!fuckme)
    {
        fuckme = true;
        game->pushMode("MenuGameMode");
    }
    if(movingRight)
        camera->ProcessKeyboard(Camera_Movement::RIGHT, (float) game->getDelta());
    if(movingLeft)
        camera->ProcessKeyboard(Camera_Movement::LEFT, (float) game->getDelta());
    if(movingUp)
        camera->ProcessKeyboard(Camera_Movement::FORWARD, (float) game->getDelta());
    if(movingDown)
        camera->ProcessKeyboard(Camera_Movement::BACKWARD, (float) game->getDelta());

    if(zoomingIn)
        camera->ProcessMouseScroll(50.0f * (float) game->getDelta());
    if(zoomingOut)
        camera->ProcessMouseScroll(50.0f * (float) -game->getDelta());
    //camera->P
    chunkRenderer->update();
}

void IntroGameMode::fixedUpdate() {
    game->getRenderer()->setRealViewport();
    chunkRenderer->render(game->getRenderer()->getRealWidth(), game->getRenderer()->getRealHeight(), game->getTime());

    game->getRenderer()->setLogicalViewport();
    fontRenderer->startFrame();
    fontRenderer->renderText(font2, 50, 50, "Use WASD and mouse look to fly around");
    fontRenderer->renderText(font2, 50, 80, "Mousewheel adjusts FOV, F12 toggles fullscreen");
    fontRenderer->renderText(font2, 50, 1000, stringFormat("pos = %.2f,%.2f,%.2f chunkPosXZ = %d,%d activeChunks: %d visChunks: %d",
                                                           camera->Position.x, camera->Position.y, camera->Position.z,
                                                           chunkRenderer->camChunkPos.x, chunkRenderer->camChunkPos.z,
                                                           chunkRenderer->getActiveChunks(), chunkRenderer->getRenderedChunks()));
    fontRenderer->render(game->getRenderer()->getWidth(), game->getRenderer()->getHeight());
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

void IntroGameMode::onMouseMotion(const SDL_MouseMotionEvent *event) {
    camera->ProcessMouseMovement(event->xrel, -event->yrel, true);
}

void IntroGameMode::onMouseWheel(int32_t yoffset) {
    camera->ProcessMouseScroll(yoffset);
}


