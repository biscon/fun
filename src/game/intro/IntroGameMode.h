//
// Created by bison on 27-09-2017.
//

#ifndef CRPG_INTROGAMEMODE_H
#define CRPG_INTROGAMEMODE_H

#include <list>
#include "../IGameMode.h"
#include "../asset/TextureAsset.h"
#include "../asset/FontAsset.h"

class IntroGameMode : public IGameMode, public IKeyboardEventListener, public ILoadTask {
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

    bool load(IGame &game) override;

    bool prepare(IGame &game) override;

private:
    bool initialized = false;
    std::string name = "IntroGameMode";
    std::shared_ptr<TextureAsset> texture1;
    std::shared_ptr<TextureAsset> texture2;
    std::shared_ptr<TextureAsset> texture3;
    std::shared_ptr<TextureAsset> texture4;
    std::shared_ptr<TextureAsset> texture5;
    std::shared_ptr<TextureAsset> texture6;
    std::shared_ptr<FontAsset> font1;
    std::shared_ptr<FontAsset> font2;

    std::vector<std::shared_ptr<TextureAsset>> textures;

    bool movingUp = false, movingDown = false, movingRight = false, movingLeft = false, zoomingIn = false, zoomingOut = false;
};


#endif //CRPG_INTROGAMEMODE_H
