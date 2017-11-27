//
// Created by bison on 30-09-2017.
//

#ifndef CRPG_IASSET_H
#define CRPG_IASSET_H

//#include "../IGame.h"

enum class AssetStatus { UNLOADED, LOADED, READY};

// forward declaration
class IGame;

class IAsset
{
public:
    IAsset(){}
    virtual ~IAsset(){}
    virtual AssetStatus getStatus()=0;
    virtual void setStatus(AssetStatus as)=0;
    virtual bool load(IGame& game)=0;
    virtual bool prepare(IGame& game)=0;
    virtual std::string getName()=0;
};
#endif //CRPG_IASSET_H
