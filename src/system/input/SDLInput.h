//
// Created by bison on 24-09-2017.
//

#ifndef CRPG_SDLINPUT_H
#define CRPG_SDLINPUT_H


#include <vector>
#include "../../game/input/IInput.h"

class SDLInput : public IInput {
public:
    bool init() override;
    void shutdown() override;
    void update() override;
    void addKeyboardEventListener(IKeyboardEventListener *listener) override;
    void removeKeyboardEventListener(IKeyboardEventListener *listener) override;
    void setQuitEventListener(IQuitEventListener *listener) override;

private:
    std::vector<IKeyboardEventListener*> keyListeners;
    IQuitEventListener *quitEventListener = nullptr;

    void handleKeyboardEvent(const SDL_Event *event);
    void deliverKeyDown(const SDL_Event *event);
    void deliverKeyUp(const SDL_Event *event);
};


#endif //CRPG_SDLINPUT_H
