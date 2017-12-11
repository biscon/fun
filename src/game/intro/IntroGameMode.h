//
// Created by bison on 27-09-2017.
//

#ifndef CRPG_INTROGAMEMODE_H
#define CRPG_INTROGAMEMODE_H

#include <list>
#include <vector>
#include <engine/IGameMode.h>
#include <engine/asset/FontAsset.h>
#include <engine/video/QuadRenderer.h>
#include <engine/video/FontRenderer.h>
#include <engine/video/CubeRenderer.h>
#include <engine/video/LightSceneRenderer.h>

class IntroGameMode : public IGameMode, public IKeyboardEventListener, public IMouseEventListener, public ILoadTask {
public:
    explicit IntroGameMode(std::shared_ptr<IGame> game);

    std::string getName() override;
    bool isInit() override;
    bool init() override;
    void shutdown() override;
    void resume() override;
    void pause() override;
    void update() override;
    void fixedUpdate() override;

    void onKeyDown(const SDL_Event *event) override;
    void onKeyUp(const SDL_Event *event) override;
    void onKeyPressed(const SDL_Event *event) override;

    void onMouseMotion(const SDL_MouseMotionEvent *event) override;

    bool load(IGame &game) override;

    bool prepare(IGame &game) override;

private:
    bool initialized = false;
    std::string name = "IntroGameMode";
    int32_t font1;
    int32_t font2;

    std::vector<int32_t> textures;

    std::unique_ptr<FontRenderer> fontRenderer;
    std::unique_ptr<QuadRenderer> quadRenderer;
    std::unique_ptr<CubeRenderer> cubeRenderer;
    std::shared_ptr<TextureAtlas> textureAtlas;

    std::unique_ptr<LightSceneRenderer> lightSceneRenderer;
    std::shared_ptr<Camera> camera;

    bool movingUp = false, movingDown = false, movingRight = false, movingLeft = false, zoomingIn = false, zoomingOut = false;
};


#endif //CRPG_INTROGAMEMODE_H
