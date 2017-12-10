//
// Created by bison on 24-09-2017.
//

#ifndef CRPG_IKEYBOARDEVENTLISTENER_H
#define CRPG_IKEYBOARDEVENTLISTENER_H

#include <SDL.h>

class IKeyboardEventListener
{
public:
    IKeyboardEventListener(){}
    virtual ~IKeyboardEventListener(){}
    virtual void onKeyDown(const SDL_Event *event)=0;
    virtual void onKeyUp(const SDL_Event *event)=0;
    virtual void onKeyPressed(const SDL_Event *event)=0;
};

#endif //CRPG_IKEYBOARDEVENTLISTENER_H
