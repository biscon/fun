//
// Created by bison on 11-01-2018.
//

#ifndef GAME_VERTEXBLOCKNEIGHBOURS_H
#define GAME_VERTEXBLOCKNEIGHBOURS_H

// The 8 vertices of a cube named for readability
#define LEFT_TOP_FRONT 0
#define LEFT_BOTTOM_FRONT 1
#define RIGHT_BOTTOM_FRONT 2
#define RIGHT_TOP_FRONT 3
#define LEFT_TOP_BACK 4
#define LEFT_BOTTOM_BACK 5
#define RIGHT_BOTTOM_BACK 6
#define RIGHT_TOP_BACK 7

// vertex block neighbours named
#define SIDE1 0
#define SIDE2 1
#define CORNER 2

#define BACK_FACE 0
#define FRONT_FACE 1
#define LEFT_FACE 2
#define RIGHT_FACE 3
#define BOTTOM_FACE 4
#define TOP_FACE 5

#define LEFT_TOP 0
#define LEFT_BTM 1
#define RIGHT_BTM 2
#define RIGHT_TOP 3

#include <defs.h>


struct BlockPos
{
    i32 x,y,z;
};

struct AOVertex
{
    i32 AO = 0;
    BlockPos positions[3];
};

struct AOFace
{
    AOVertex vertices[4];
};

struct AmbientOcclusion
{
    AOFace faces[6];
    /*
     * This monster calculates the block positions needed to calculate AO (72)
     */
    inline void calcPositions(i32 x, i32 y, i32 z)
    {
        // iterate faces
        for(i32 i = 0; i < 6; i++)
        {
            switch(i)
            {
                case BACK_FACE:
                {
                    // neighbours
                    faces[i].vertices[LEFT_TOP].positions[SIDE1] = {x+1, y, z-1};
                    faces[i].vertices[LEFT_TOP].positions[SIDE2] = {x, y+1, z-1};
                    faces[i].vertices[LEFT_TOP].positions[CORNER] = {x+1, y+1, z-1};

                    faces[i].vertices[LEFT_BTM].positions[SIDE1] = {x+1, y, z-1};
                    faces[i].vertices[LEFT_BTM].positions[SIDE2] = {x, y-1, z-1};
                    faces[i].vertices[LEFT_BTM].positions[CORNER] = {x+1, y-1, z-1};

                    faces[i].vertices[RIGHT_TOP].positions[SIDE1] = {x, y+1, z-1};
                    faces[i].vertices[RIGHT_TOP].positions[SIDE2] = {x-1, y, z-1};
                    faces[i].vertices[RIGHT_TOP].positions[CORNER] = {x-1, y+1, z-1};

                    faces[i].vertices[RIGHT_BTM].positions[SIDE1] = {x, y-1, z-1};
                    faces[i].vertices[RIGHT_BTM].positions[SIDE2] = {x-1, y, z-1};
                    faces[i].vertices[RIGHT_BTM].positions[CORNER] = {x-1, y-1, z-1};
                    break;
                }
                case FRONT_FACE:
                {
                    // neighbours
                    faces[i].vertices[LEFT_TOP].positions[SIDE1] = {x-1, y, z+1};
                    faces[i].vertices[LEFT_TOP].positions[SIDE2] = {x, y+1, z+1};
                    faces[i].vertices[LEFT_TOP].positions[CORNER] = {x-1, y+1, z+1};

                    faces[i].vertices[LEFT_BTM].positions[SIDE1] = {x-1, y, z+1};
                    faces[i].vertices[LEFT_BTM].positions[SIDE2] = {x, y-1, z+1};
                    faces[i].vertices[LEFT_BTM].positions[CORNER] = {x-1, y-1, z+1};

                    faces[i].vertices[RIGHT_TOP].positions[SIDE1] = {x, y+1, z+1};
                    faces[i].vertices[RIGHT_TOP].positions[SIDE2] = {x+1, y, z+1};
                    faces[i].vertices[RIGHT_TOP].positions[CORNER] = {x+1, y+1, z+1};

                    faces[i].vertices[RIGHT_BTM].positions[SIDE1] = {x, y-1, z+1};
                    faces[i].vertices[RIGHT_BTM].positions[SIDE2] = {x+1, y, z+1};
                    faces[i].vertices[RIGHT_BTM].positions[CORNER] = {x+1, y-1, z+1};
                    break;
                }
                case LEFT_FACE:
                {
                    // neighbours
                    faces[i].vertices[LEFT_TOP].positions[SIDE1] = {x-1, y, z-1};
                    faces[i].vertices[LEFT_TOP].positions[SIDE2] = {x-1, y+1, z};
                    faces[i].vertices[LEFT_TOP].positions[CORNER] = {x-1, y+1, z-1};

                    faces[i].vertices[LEFT_BTM].positions[SIDE1] = {x-1, y, z-1};
                    faces[i].vertices[LEFT_BTM].positions[SIDE2] = {x-1, y-1, z};
                    faces[i].vertices[LEFT_BTM].positions[CORNER] = {x-1, y-1, z-1};

                    faces[i].vertices[RIGHT_TOP].positions[SIDE1] = {x-1, y+1, z};
                    faces[i].vertices[RIGHT_TOP].positions[SIDE2] = {x-1, y, z+1};
                    faces[i].vertices[RIGHT_TOP].positions[CORNER] = {x-1, y+1, z+1};

                    faces[i].vertices[RIGHT_BTM].positions[SIDE1] = {x-1, y, z+1};
                    faces[i].vertices[RIGHT_BTM].positions[SIDE2] = {x-1, y-1, z};
                    faces[i].vertices[RIGHT_BTM].positions[CORNER] = {x-1, y-1, z+1};
                    break;
                }
                case RIGHT_FACE:
                {
                    // neighbours
                    faces[i].vertices[LEFT_TOP].positions[SIDE1] = {x+1, y, z+1};
                    faces[i].vertices[LEFT_TOP].positions[SIDE2] = {x+1, y+1, z};
                    faces[i].vertices[LEFT_TOP].positions[CORNER] = {x+1, y+1, z+1};

                    faces[i].vertices[LEFT_BTM].positions[SIDE1] = {x+1, y, z+1};
                    faces[i].vertices[LEFT_BTM].positions[SIDE2] = {x+1, y-1, z};
                    faces[i].vertices[LEFT_BTM].positions[CORNER] = {x+1, y-1, z+1};

                    faces[i].vertices[RIGHT_TOP].positions[SIDE1] = {x+1, y+1, z};
                    faces[i].vertices[RIGHT_TOP].positions[SIDE2] = {x+1, y, z-1};
                    faces[i].vertices[RIGHT_TOP].positions[CORNER] = {x+1, y+1, z-1};

                    faces[i].vertices[RIGHT_BTM].positions[SIDE1] = {x+1, y, z-1};
                    faces[i].vertices[RIGHT_BTM].positions[SIDE2] = {x+1, y-1, z};
                    faces[i].vertices[RIGHT_BTM].positions[CORNER] = {x+1, y-1, z-1};
                    break;
                }
                case BOTTOM_FACE:
                {
                    // neighbours
                    faces[i].vertices[LEFT_TOP].positions[SIDE1] = {x-1, y-1, z};
                    faces[i].vertices[LEFT_TOP].positions[SIDE2] = {x, y-1, z+1};
                    faces[i].vertices[LEFT_TOP].positions[CORNER] = {x-1, y-1, z+1};

                    faces[i].vertices[LEFT_BTM].positions[SIDE1] = {x-1, y-1, z};
                    faces[i].vertices[LEFT_BTM].positions[SIDE2] = {x, y-1, z-1};
                    faces[i].vertices[LEFT_BTM].positions[CORNER] = {x-1, y-1, z-1};

                    faces[i].vertices[RIGHT_TOP].positions[SIDE1] = {x, y-1, z+1};
                    faces[i].vertices[RIGHT_TOP].positions[SIDE2] = {x+1, y-1, z};
                    faces[i].vertices[RIGHT_TOP].positions[CORNER] = {x+1, y-1, z+1};

                    faces[i].vertices[RIGHT_BTM].positions[SIDE1] = {x+1, y-1, z};
                    faces[i].vertices[RIGHT_BTM].positions[SIDE2] = {x, y-1, z-1};
                    faces[i].vertices[RIGHT_BTM].positions[CORNER] = {x+1, y-1, z-1};
                    break;
                }
                case TOP_FACE:
                {
                    // neighbours
                    faces[i].vertices[LEFT_TOP].positions[SIDE1] = {x-1, y+1, z};
                    faces[i].vertices[LEFT_TOP].positions[SIDE2] = {x, y+1, z-1};
                    faces[i].vertices[LEFT_TOP].positions[CORNER] = {x-1, y+1, z-1};

                    faces[i].vertices[LEFT_BTM].positions[SIDE1] = {x-1, y+1, z};
                    faces[i].vertices[LEFT_BTM].positions[SIDE2] = {x, y+1, z+1};
                    faces[i].vertices[LEFT_BTM].positions[CORNER] = {x-1, y+1, z+1};

                    faces[i].vertices[RIGHT_TOP].positions[SIDE1] = {x, y+1, z-1};
                    faces[i].vertices[RIGHT_TOP].positions[SIDE2] = {x+1, y+1, z};
                    faces[i].vertices[RIGHT_TOP].positions[CORNER] = {x+1, y+1, z-1};

                    faces[i].vertices[RIGHT_BTM].positions[SIDE1] = {x+1, y+1, z};
                    faces[i].vertices[RIGHT_BTM].positions[SIDE2] = {x, y+1, z+1};
                    faces[i].vertices[RIGHT_BTM].positions[CORNER] = {x+1, y+1, z+1};
                    break;
                }
            }
        }
    }
};

struct FaceLight
{
    u8 v1,v2,v3,v4;
};

struct BlockLight
{
    FaceLight torchLight[6];
    FaceLight sunLight[6];
};
#endif //GAME_VERTEXBLOCKNEIGHBOURS_H
