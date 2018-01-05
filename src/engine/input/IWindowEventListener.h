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
    virtual void onWindowResize(i32 width, i32 height)=0;
    virtual void onMouseEnter()=0;
    virtual void onMouseLeave()=0;
    virtual void onFocusGained()=0;
    virtual void onFocusLost()=0;
};


#endif
