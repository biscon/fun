//
// Created by bison on 15-10-2017.
//

#include <SDL_log.h>
#include "math_util.h"
#include "defs.h"

double remapDouble(double oMin, double oMax, double nMin, double nMax, double x){
//range check
    if( oMin == oMax) {
        SDL_Log("Warning: Zero input range");
        return -1;    }

    if( nMin == nMax){
        SDL_Log("Warning: Zero output range");
        return -1;        }

//check reversed input range
    bool reverseInput = false;
    double oldMin = MIN(oMin, oMax );
    double oldMax = MAX(oMin, oMax );
    if (oldMin == oMin)
        reverseInput = true;

//check reversed output range
    bool reverseOutput = false;
    double newMin = MIN(nMin, nMax );
    double newMax = MAX(nMin, nMax );
    if (newMin == nMin)
        reverseOutput = true;

    double portion = (x-oldMin)*(newMax-newMin)/(oldMax-oldMin);
    if (reverseInput)
        portion = (oldMax-x)*(newMax-newMin)/(oldMax-oldMin);

    double result = portion + newMin;
    if (reverseOutput)
        result = newMax - portion;

    return result;
}

float remapFloat(float oMin, float oMax, float nMin, float nMax, float x){
//range check
    if( oMin == oMax) {
        SDL_Log("Warning: Zero input range");
        return -1;    }

    if( nMin == nMax){
        SDL_Log("Warning: Zero output range");
        return -1;        }

//check reversed input range
    bool reverseInput = false;
    float oldMin = MIN(oMin, oMax );
    float oldMax = MAX(oMin, oMax );
    if (oldMin == oMin)
        reverseInput = true;

//check reversed output range
    bool reverseOutput = false;
    float newMin = MIN(nMin, nMax );
    float newMax = MAX(nMin, nMax );
    if (newMin == nMin)
        reverseOutput = true;

    float portion = (x-oldMin)*(newMax-newMin)/(oldMax-oldMin);
    if (reverseInput)
        portion = (oldMax-x)*(newMax-newMin)/(oldMax-oldMin);

    float result = portion + newMin;
    if (reverseOutput)
        result = newMax - portion;

    return result;
}

// *Really* minimal PCG32 code / (c) 2014 M.E. O'Neill / pcg-random.org
// Licensed under Apache License 2.0 (NO WARRANTY, etc. see website)

typedef struct { uint64_t state;  uint64_t inc; } pcg32_random_t;

static inline u32 pcg32_random_r(pcg32_random_t* rng)
{
    uint64_t oldstate = rng->state;
    // Advance internal state
    rng->state = oldstate * 6364136223846793005ULL + (rng->inc|1);
    // Calculate output function (XSH RR), uses old state for max ILP
    u32 xorshifted = ((oldstate >> 18u) ^ oldstate) >> 27u;
    u32 rot = oldstate >> 59u;
    return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
}
