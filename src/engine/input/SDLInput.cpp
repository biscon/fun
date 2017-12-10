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
