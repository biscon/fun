//
// Created by bison on 24-09-2017.
//

#ifndef CRPG_IINPUT_H
#define CRPG_IINPUT_H

#include "../../defs.h"
#include "IKeyboardEventListener.h"
#include "IQuitEventListener.h"
#include "IMouseEventListener.h"

class IInput
{
public:
    IInput(){}
    virtual ~IInput(){}
    virtual bool init()=0;
    virtual void shutdown()=0;
    virtual void update()=0;
    virtual void addKeyboardEventListener(IKeyboardEventListener *listener)=0;
    virtual void removeKeyboardEventListener(IKeyboardEventListener *listener)=0;
    virtual void addMouseEventListener(IMouseEventListener *listener)=0;
    virtual void removeMouseEventListener(IMouseEventListener *listener)=0;
    virtual void setQuitEventListener(IQuitEventListener *listener)=0;
};


#endif
