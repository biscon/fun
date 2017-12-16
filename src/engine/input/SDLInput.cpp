//
// Created by bison on 24-09-2017.
//

#include <SDL.h>
#include "SDLInput.h"

bool SDLInput::init() {
    return true;
}

void SDLInput::shutdown() {

}

void SDLInput::update() {
    //Handle events on queue
    SDL_Event e;
    while( SDL_PollEvent( &e ) != 0 )
    {
        //User requests quit
        if( e.type == SDL_QUIT )
        {
            if(quitEventListener != nullptr)
                quitEventListener->onQuit();
            return;
        }
        handleKeyboardEvent(&e);
        if(e.type == SDL_MOUSEMOTION || e.type == SDL_MOUSEWHEEL || e.type == SDL_MOUSEBUTTONDOWN || e.type == SDL_MOUSEBUTTONUP)
        {
            handleMouseEvent(&e);
        }
        if(e.type == SDL_WINDOWEVENT)
        {
            //SDL_Log("Window event");
            handleWindowEvent(&e);
        }
    }
}

void SDLInput::handleMouseEvent(const SDL_Event *event)
{
    switch( event->type ) {
        case SDL_MOUSEMOTION: {
            // call listeners
            deliverMouseMotion(event);
            break;
        }
    }
}

void SDLInput::handleWindowEvent(const SDL_Event *event)
{
    switch( event->window.event ) {
        case SDL_WINDOWEVENT_SIZE_CHANGED: {
            // call listeners
            deliverWindowEvent(event);
            break;
        }
        case SDL_WINDOWEVENT_ENTER: {
            for(auto &l : windowListeners) {
                l->onMouseEnter();
            }
            break;
        }
        case SDL_WINDOWEVENT_LEAVE: {
            for(auto &l : windowListeners) {
                l->onMouseLeave();
            }
            break;
        }
        case SDL_WINDOWEVENT_FOCUS_GAINED: {
            for(auto &l : windowListeners) {
                l->onFocusGained();
            }
            break;
        }
        case SDL_WINDOWEVENT_FOCUS_LOST: {
            for(auto &l : windowListeners) {
                l->onFocusLost();
            }
            break;
        }
    }
}

void SDLInput::deliverWindowEvent(const SDL_Event *event)
{
    for(auto i = windowListeners.begin(); i != windowListeners.end(); ++i)
    {
        (*i)->onWindowResize(event->window.data1, event->window.data2);
    }
}


void SDLInput::deliverMouseMotion(const SDL_Event *event)
{
    for(auto i = mouseListeners.begin(); i != mouseListeners.end(); ++i)
    {
        (*i)->onMouseMotion(&event->motion);
    }
}


void SDLInput::handleKeyboardEvent(const SDL_Event *event)
{
    switch( event->type ) {
        case SDL_KEYDOWN: {
            if(event->key.repeat == 0) {
                // call listeners
                //kb.keysym = event->key.keysym;
                deliverKeyDown(event);
            }
            break;
        }
        case SDL_KEYUP: {
            //kb.keysym = event->key.keysym;
            deliverKeyUp(event);
            break;
        }
    }
}

void SDLInput::deliverKeyDown(const SDL_Event *event)
{
    for(auto i = keyListeners.begin(); i != keyListeners.end(); ++i)
    {
        (*i)->onKeyDown(event);
    }
}

void SDLInput::deliverKeyUp(const SDL_Event *event)
{
    for(auto i = keyListeners.begin(); i != keyListeners.end(); ++i)
    {
        (*i)->onKeyUp(event);
    }
}

void SDLInput::removeKeyboardEventListener(IKeyboardEventListener *listener) {
    //std::vector<IKeyboardEventListener *>::const_iterator i;
    for(auto i = keyListeners.begin(); i != keyListeners.end(); ++i)
    {
        if(*i == listener)
        {
            i = keyListeners.erase(i);
            return;
        }
    }
}

void SDLInput::addKeyboardEventListener(IKeyboardEventListener *listener) {
    keyListeners.push_back(listener);
}

void SDLInput::setQuitEventListener(IQuitEventListener *listener) {
    quitEventListener = listener;
}

void SDLInput::removeMouseEventListener(IMouseEventListener *listener) {
    for(auto i = mouseListeners.begin(); i != mouseListeners.end(); ++i)
    {
        if(*i == listener)
        {
            i = mouseListeners.erase(i);
            return;
        }
    }
}

void SDLInput::addMouseEventListener(IMouseEventListener *listener) {
    mouseListeners.push_back(listener);
}

void SDLInput::addWindowEventListener(IWindowEventListener *listener) {
    windowListeners.push_back(listener);
}

void SDLInput::removeWindowEventListener(IWindowEventListener *listener) {
    for(auto i = windowListeners.begin(); i != windowListeners.end(); ++i)
    {
        if(*i == listener)
        {
            i = windowListeners.erase(i);
            return;
        }
    }
}
