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

    camera = std::make_shared<Camera>(glm::vec3(0.0f, 130.0f, 0.0f));
    terrain = std::make_shared<Terrain>();

    setupBlockTypes();

    sceneryManager = std::make_shared<SceneryManager>();

    chunkManager = std::make_shared<AChunkManager>(*game, *blockTypeDict, terrain, sceneryManager);

    chunkRenderer = std::unique_ptr<ChunkRenderer>(new ChunkRenderer(*game, camera, terrain, chunkManager, blockTypeDict, sceneryManager));
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
void IntroGameMode::update(double delta) {
    if(!fuckme)
    {
        fuckme = true;
        game->pushMode("MenuGameMode");
    }
    if(movingRight)
        camera->ProcessKeyboard(Camera_Movement::RIGHT, (float) delta);
    if(movingLeft)
        camera->ProcessKeyboard(Camera_Movement::LEFT, (float) delta);
    if(movingUp)
        camera->ProcessKeyboard(Camera_Movement::FORWARD, (float) delta);
    if(movingDown)
        camera->ProcessKeyboard(Camera_Movement::BACKWARD, (float) delta);

    if(zoomingIn)
        camera->ProcessMouseScroll(50.0f * (float) delta);
    if(zoomingOut)
        camera->ProcessMouseScroll(50.0f * (float) delta);

    chunkRenderer->update();
}

void IntroGameMode::render(double delta) {
    game->getRenderer()->setRealViewport();
    chunkRenderer->render(game->getRenderer()->getRealWidth(), game->getRenderer()->getRealHeight(), delta);

    //game->getRenderer()->setLogicalViewport();
    fontRenderer->startFrame();
    fontRenderer->renderText(font2, 50, 50, "Use WASD and mouse look to fly around");
    fontRenderer->renderText(font2, 50, 80, "Mousewheel adjusts FOV, F12 toggles fullscreen");

    fontRenderer->renderText(font2, 50, game->getRenderer()->getRealHeight() - 50, stringFormat("pos = %.2f,%.2f,%.2f chunkPosXZ = %d,%d activeChunks: %d visChunks: %d voxMeshSize: %d MB visRad: %d mesher: %s",
                                                           camera->Position.x, camera->Position.y, camera->Position.z,
                                                           chunkRenderer->chunkManager->camChunkPos.x, chunkRenderer->chunkManager->camChunkPos.z,chunkRenderer->getActiveChunks(), chunkRenderer->getRenderedChunks(), (chunkRenderer->chunkManager->totalMeshSizeBytes / 1024) / 1024, chunkRenderer->chunkManager->VISIBLE_RADIUS, chunkRenderer->chunkManager->getBuildStageAsString().c_str()));
    std::string chk_info = "INV";
    if(chunkRenderer->chunkManager->camBlockLocalPos.chunk != nullptr)
    {
        chk_info = stringFormat("%d,%d", chunkRenderer->chunkManager->camBlockLocalPos.chunk->chunkPosition.x, chunkRenderer->chunkManager->camBlockLocalPos.chunk->chunkPosition.z);
    }

    fontRenderer->renderText(font2, 50, game->getRenderer()->getRealHeight() - 85, stringFormat("blockPosWorld = %d,%d,%d blockPosLocal = %d,%d,%d (%s) sunlightIntensity = %.2f",
                                                                                                chunkRenderer->chunkManager->camBlockWorldPos.x, chunkRenderer->chunkManager->camBlockWorldPos.y, chunkRenderer->chunkManager->camBlockWorldPos.z,
                                                                                                chunkRenderer->chunkManager->camBlockLocalPos.x, chunkRenderer->chunkManager->camBlockLocalPos.y, chunkRenderer->chunkManager->camBlockLocalPos.z,
                                                                                                chk_info.c_str(), chunkRenderer->chunkManager->sunlightIntensity));

    fontRenderer->render(game->getRenderer()->getRealWidth(), game->getRenderer()->getRealHeight());
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
    if(event->key.keysym.sym == SDLK_SPACE)
    {
        auto& localpos = chunkRenderer->chunkManager->camBlockLocalPos;
        if(localpos.isValid() && (localpos.y >= 0 && localpos.y < CHUNK_HEIGHT))
        {
            Block &block = localpos.chunk->blocks[POS_TO_INDEX(localpos.y, localpos.z, localpos.x)];
            if(block.isFlagSet(BLOCK_FLAG_ACTIVE) && block.type == BLOCK_GOLD)
                chunkRenderer->chunkManager->removeTorchLight(localpos.chunk, localpos.x, localpos.y, localpos.z);
            else
                chunkRenderer->chunkManager->placeTorchLight(localpos.chunk, localpos.x, localpos.y, localpos.z, 15);
        }
        //chunkRenderer->chunkManager->testStuff();
        return;
    }
    if(event->key.keysym.sym == SDLK_HOME)
    {
        chunkRenderer->chunkManager->logChunkMetrics();
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

void IntroGameMode::setupBlockTypes() {
    blockTypeDict = std::make_shared<BlockTypeDictionary>();
    blockTypeDict->createBlockType("stone", "assets/stone.png");

    blockTypeDict->createBlockType("grass", "assets/grass_side.png");
    auto* grass = blockTypeDict->getBlockTypeByName("grass");
    //blockTypeDict->setFaceColor(grass, TOP_FACE, glm::vec4(0.07568f, 0.61424f, 0.07568f, 1.0f));
    blockTypeDict->setFaceTexture(grass, TOP_FACE, "assets/grass_top.png");
    blockTypeDict->setFaceTexture(grass, BOTTOM_FACE, "assets/grass_top.png");

    blockTypeDict->createBlockType("water", "assets/water_diffuse.png");

    blockTypeDict->createBlockType("emerald", glm::vec4(0.07568f, 0.61424f, 0.07568f, 1.0f));

    blockTypeDict->createBlockType("gold", glm::vec4(0.75164f, 0.60648f, 0.22648f, 1.0f));

    blockTypeDict->createBlockType("silver", glm::vec4(0.50754f, 0.50754f, 0.50754f, 1.0f));

    blockTypeDict->createBlockType("bronze", glm::vec4(0.714f, 0.4284f, 0.18144f, 1.0f));

    blockTypeDict->createBlockType("debug", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

    blockTypeDict->createBlockType("planks", "assets/planks_acacia.png");
    blockTypeDict->createBlockType("glass", "assets/glass.png");
    blockTypeDict->createBlockType("brick", "assets/brick.png");

    blockTypeDict->createBlockType("sand", "assets/sand.png");
    blockTypeDict->createBlockType("red_sand", "assets/red_sand.png");
}


