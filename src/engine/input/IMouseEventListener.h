//
// Created by bison on 11-12-2017.
//

#ifndef GAME_IMOUSEEVENTLISTENER_H
#define GAME_IMOUSEEVENTLISTENER_H

#include <SDL.h>

class IMouseEventListener {
public:
    IMouseEventListener(){}
    virtual ~IMouseEventListener(){}
    virtual void onMouseMotion(const SDL_MouseMotionEvent *event)=0;
    virtual void onMouseWheel(int32_t yoffset)=0;
};


#endif //GAME_IMOUSEEVENTLISTENER_H
