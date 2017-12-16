//
// Created by bison on 11-12-2017.
//

#ifndef GAME_IWINDOWEVENTLISTENER_H
#define GAME_IWINDOWEVENTLISTENER_H

#include <SDL.h>

class IWindowEventListener {
public:
    IWindowEventListener(){}
    virtual ~IWindowEventListener(){}
    virtual void onWindowResize(int32_t width, int32_t height)=0;
};


#endif
