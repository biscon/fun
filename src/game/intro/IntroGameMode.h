//
// Created by bison on 27-09-2017.
//

#ifndef CRPG_INTROGAMEMODE_H
#define CRPG_INTROGAMEMODE_H

#include <list>
#include <vector>
#include <engine/IGameMode.h>
#include <engine/asset/FontAsset.h>
#include <engine/video/ui/QuadRenderer.h>
#include <engine/video/font/FontRenderer.h>
#include <engine/video/other/CubeRenderer.h>
#include <engine/video/other/LightSceneRenderer.h>
#include <engine/video/model/Model.h>
#include <engine/video/model/ModelRenderer.h>
#include <engine/video/voxel/ChunkRenderer.h>
#include <engine/video/voxel/Terrain.h>

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

    // keyboard events
    void onKeyDown(const SDL_Event *event) override;
    void onKeyUp(const SDL_Event *event) override;
    void onKeyPressed(const SDL_Event *event) override;

    // mouse events
    void onMouseMotion(const SDL_MouseMotionEvent *event) override;

    void onMouseWheel(int32_t yoffset) override;

    bool load(IGame &game) override;

    bool prepare(IGame &game) override;

private:
    bool initialized = false;
    std::string name = "IntroGameMode";
    int32_t font1;
    int32_t font2;

    std::vector<int32_t> textures;

    std::unique_ptr<FontRenderer> fontRenderer;
    //std::unique_ptr<QuadRenderer> quadRenderer;
    //std::unique_ptr<CubeRenderer> cubeRenderer;
    //std::shared_ptr<TextureAtlas> textureAtlas;

    //std::shared_ptr<Model> model;
    //std::shared_ptr<Model> model2;

    std::unique_ptr<LightSceneRenderer> lightSceneRenderer;
    //std::unique_ptr<ModelRenderer> modelRenderer;
    std::unique_ptr<ChunkRenderer> chunkRenderer;
    std::shared_ptr<Camera> camera;
    std::shared_ptr<Terrain> terrain;

    bool movingUp = false, movingDown = false, movingRight = false, movingLeft = false, zoomingIn = false, zoomingOut = false;
};


#endif //CRPG_INTROGAMEMODE_H
