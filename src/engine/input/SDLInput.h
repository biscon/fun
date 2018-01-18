//
// Created by bison on 24-09-2017.
//

#ifndef CRPG_SDLINPUT_H
#define CRPG_SDLINPUT_H


#include <vector>
#include "IInput.h"

class SDLInput : public IInput {
public:
    bool init() override;
    void shutdown() override;
    void update() override;
    void addKeyboardEventListener(IKeyboardEventListener *listener) override;
    void removeKeyboardEventListener(IKeyboardEventListener *listener) override;
    void addMouseEventListener(IMouseEventListener *listener) override;
    void removeMouseEventListener(IMouseEventListener *listener) override;
    void addWindowEventListener(IWindowEventListener *listener) override;
    void removeWindowEventListener(IWindowEventListener *listener) override;
    void setQuitEventListener(IQuitEventListener *listener) override;

private:
    std::vector<IKeyboardEventListener*> keyListeners;
    std::vector<IMouseEventListener*> mouseListeners;
    std::vector<IWindowEventListener*> windowListeners;
    IQuitEventListener *quitEventListener = nullptr;

    void handleKeyboardEvent(const SDL_Event *event);
    void handleMouseEvent(const SDL_Event *event);
    void handleWindowEvent(const SDL_Event *event);
    void deliverWindowEvent(const SDL_Event *event);
    void deliverMouseMotion(const SDL_Event *event);
    void deliverKeyDown(const SDL_Event *event);
    void deliverKeyUp(const SDL_Event *event);
    void deliverTextInput(const SDL_Event *event);
    void deliverTextEditing(const SDL_Event *event);
};


#endif //CRPG_SDLINPUT_H
