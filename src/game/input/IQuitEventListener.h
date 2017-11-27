//
// Created by bison on 24-09-2017.
//

#ifndef CRPG_IQUITEVENTLISTENER_H
#define CRPG_IQUITEVENTLISTENER_H


class IQuitEventListener {
public:
    IQuitEventListener(){}
    virtual ~IQuitEventListener(){}
    virtual void onQuit()=0;
};


#endif //CRPG_IQUITEVENTLISTENER_H
