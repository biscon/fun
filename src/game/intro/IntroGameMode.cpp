//
// Created by bison on 27-09-2017.
//

#include <SDL_log.h>

#include <utility>
#include <ctime>
#include <engine/util/string_util.h>
#include "IntroGameMode.h"
#include "engine/video/shader/shaders.h"

GLuint vao, vbo;
Shader *fontShader;
Font font;

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

    fontShader = new Shader(fontVertex, fontFragment, NULL);
    GLfloat w, h;
    w = static_cast<GLfloat>(game->getRenderer()->getWidth());
    h = static_cast<GLfloat>(game->getRenderer()->getHeight());
    fontShader->use();
    fontShader->setMat4("proj", glm::ortho(0.0f, w, 0.0f, h));

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    fontRenderer = std::unique_ptr<FontRenderer>(new FontRenderer());
    font1 = fontRenderer->addFont("font9x14.png", 9, 14);
    font2 = fontRenderer->addFont("bedstead12x20.png", 12, 20);

    // TTF
    b32 error = InitFontLoader();
    if (error) {
        SDL_Log("Failed to init TTF loader");
    }

    error = MakeFont(&font, "UnDotum.ttf", 255);
    if (error) {
        SDL_Log("Failed to load font 'UnDotum.ttf'");
    }

    game->getAssetLoader()->addLoadTask(fontRenderer.get());

    /*
    textureAtlas = std::make_shared<TextureAtlas>(2048, 2048, false);
    quadRenderer = std::unique_ptr<QuadRenderer>(new QuadRenderer(textureAtlas));
    cubeRenderer = std::unique_ptr<CubeRenderer>(new CubeRenderer(textureAtlas));
    */

    camera = std::make_shared<Camera>(glm::vec3(0.0f, 130.0f, 0.0f));
    terrain = std::make_shared<Terrain>();
    //lightSceneRenderer = std::unique_ptr<LightSceneRenderer>(new LightSceneRenderer(*game->getSystem(), camera));

    //model = std::make_shared<Model>("nanosuit/nanosuit.obj", "nanosuit/");
    //lightSceneRenderer->testModel = model;
    //game->getAssetLoader()->addLoadTask(model.get());

    //model2 = std::make_shared<Model>("assets/grass_tile.obj", "assets/");
    //game->getAssetLoader()->addLoadTask(model2.get());

    //modelRenderer = std::unique_ptr<ModelRenderer>(new ModelRenderer(*game->getSystem(), model2, camera));

    chunkRenderer = std::unique_ptr<ChunkRenderer>(new ChunkRenderer(*game, camera, terrain));
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

u32 FIRST_LEN[] = {
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 
    4, 4, 4, 4, 4, 4, 4, 4, 1, 1, 1, 1, 1, 1, 1, 1
};

u8 MASK[] = {
    0xFF, 0xFF, 0x1F, 0xF, 0x7
};

#define InvalidCodePoint 0xFFFD

// WARNING: this function cannot handle a buffer where `len = 0`
u32 DecodeCodePoint(int *cpLen, u8 *buffer) {
    u8 b0 = buffer[0];
    int l = FIRST_LEN[b0];
    int val = (int)(b0 & MASK[l]);

    for (int i=1; i < l; i += 1) {
        val = (val << 6) | (int)(buffer[i] & 0x3f);
    }

    if (cpLen)
        *cpLen = l;
    return val;
}

// WARNING: this function assumes that buffer is >= 4 bytes
u32 EncodeCodePoint(u8 * const buffer, const u32 cp) {
    if (cp <= 0x7F) {
        buffer[0] = cp;
        return 1;
    }

    if (cp <= 0x7FF) {
        buffer[0] = 0xC0 | (cp >> 6);
        buffer[1] = 0x80 | (cp & 0x3F);
        return 2;
    }

    if (cp <= 0xFFFF) {
        buffer[0] = 0xE0 | (cp >> 12);
        buffer[1] = 0x80 | ((cp >> 6) & 0x3F);
        buffer[2] = 0x80 | (cp & 0x3F);
        return 3;
    }

    if (cp <= 0x10FFFF) {
        buffer[0] = 0xF0 | (cp >> 18);
        buffer[1] = 0x80 | ((cp >> 12) & 0x3F);
        buffer[2] = 0x80 | ((cp >> 6) & 0x3F);
        buffer[3] = 0x80 | (cp & 0x3F);
        return 4;
    }

    return 0;
}

void drawText(Font *font, GLfloat x, GLfloat y, const char *text) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    fontShader->use();
    fontShader->setVec3("textColor", 1, 0, 1);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(vao);

    size_t len = strlen(text);
    for (int i=0; i < len;) {
        // TODO(Brett): if the character is null, attempt to fetch it from the font
        int cpLen;
        u32 cp = DecodeCodePoint(&cpLen, (u8 *)&text[i]);
        Character c = font->characterMap[cp];
        i += cpLen;

        GLfloat xp, yp, h, w;
        xp = x + c.bearing.x;
        yp = y - (c.size.y - c.bearing.y);
        w = c.size.x;
        h = c.size.y;

        GLfloat verts[6][4] = {
            {xp,     yp + h, 0, 0},
            {xp,     yp,     0, 1},
            {xp + w, yp,     1, 1},

            {xp,     yp + h, 0, 0},
            {xp + w, yp,     1, 1},
            {xp + w, yp + h, 1, 0}
        };

        glBindTexture(GL_TEXTURE_2D, c.textureId);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(verts), verts);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glDrawArrays(GL_TRIANGLES, 0, 6);
        x += c.advance;
    }

    glDisable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void IntroGameMode::render(double delta) {
    game->getRenderer()->setRealViewport();
    chunkRenderer->render(game->getRenderer()->getRealWidth(), game->getRenderer()->getRealHeight(), delta);

    //game->getRenderer()->setLogicalViewport();
    fontRenderer->startFrame();
    fontRenderer->renderText(font2, 50, 50, "Use WASD and mouse look to fly around");
    fontRenderer->renderText(font2, 50, 80, "Mousewheel adjusts FOV, F12 toggles fullscreen");
    fontRenderer->renderText(font2, 50, game->getRenderer()->getRealHeight() - 50, stringFormat("pos = %.2f,%.2f,%.2f chunkPosXZ = %d,%d activeChunks: %d visChunks: %d voxMeshSize: %d MB visRad: %d buildStage: %s",
                                                           camera->Position.x, camera->Position.y, camera->Position.z,
                                                           chunkRenderer->chunkManager->camChunkPos.x, chunkRenderer->chunkManager->camChunkPos.z,chunkRenderer->getActiveChunks(), chunkRenderer->getRenderedChunks(), (chunkRenderer->chunkManager->totalMeshSizeBytes / 1024) / 1024, chunkRenderer->chunkManager->VISIBLE_RADIUS, chunkRenderer->chunkManager->getBuildStageAsString().c_str()));
    std::string chk_info = "INV";
    if(chunkRenderer->chunkManager->camBlockLocalPos.chunk != nullptr)
    {
        chk_info = stringFormat("%d,%d", chunkRenderer->chunkManager->camBlockLocalPos.chunk->chunkPosition.x, chunkRenderer->chunkManager->camBlockLocalPos.chunk->chunkPosition.z);
    }

    fontRenderer->renderText(font2, 50, game->getRenderer()->getRealHeight() - 85, stringFormat("blockPosWorld = %d,%d,%d blockPosLocal = %d,%d,%d (%s)",
                                                                                                chunkRenderer->chunkManager->camBlockWorldPos.x, chunkRenderer->chunkManager->camBlockWorldPos.y, chunkRenderer->chunkManager->camBlockWorldPos.z,
                                                                                                chunkRenderer->chunkManager->camBlockLocalPos.x, chunkRenderer->chunkManager->camBlockLocalPos.y, chunkRenderer->chunkManager->camBlockLocalPos.z,
                                                                                                chk_info.c_str()));
    fontRenderer->render(game->getRenderer()->getRealWidth(), game->getRenderer()->getRealHeight());


    drawText(&font, 450, 450, "한글");
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
        // TODO prevent this from being called twice lol
        if(localpos.isValid() && (localpos.y >= 0 && localpos.y < CHUNK_HEIGHT))
        {
            chunkRenderer->chunkManager->placeTorchLight(localpos.chunk, localpos.x, localpos.y, localpos.z, 31);
        }
        //chunkRenderer->chunkManager->testStuff();
        return;
    }
}

void IntroGameMode::onTextInput(const SDL_Event *event) {

}

void IntroGameMode::onTextEditing(const SDL_Event *event) {

}

void IntroGameMode::onKeyPressed(const SDL_Event *event) {

}

void IntroGameMode::onMouseMotion(const SDL_MouseMotionEvent *event) {
    camera->ProcessMouseMovement(event->xrel, -event->yrel, true);
}

void IntroGameMode::onMouseWheel(int32_t yoffset) {
    camera->ProcessMouseScroll(yoffset);
}


