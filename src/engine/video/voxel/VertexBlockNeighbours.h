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

// holds the 8 positions to check for each vertex (8 in a cube)
struct VertexBlockNeighbours
{
    i32 AO = 0;
    BlockPos positions[8];

    /*
     * Calculate adjacent voxel positions to check for ambient occlusion
     * x,y,z is the voxel position and vi is the vertex index (0-7) indicating which
     * of the 8 vertices in the block we are checking
     */
    inline void calculate(i8 x, i8 y, i8 z, u8 vi)
    {
        // first pos is always the cube we are in
        positions[0] = {x,y,z};
        switch(vi)
        {
            // Vertex 1: left, top, front
            case LEFT_TOP_FRONT: {
                genBoxOffsets(x - 1, y, z + 1);
                break;
            }
                // Vertex 2: left, bottom, front
            case LEFT_BOTTOM_FRONT: {
                genBoxOffsets(x - 1, y - 1, z + 1);
                break;
            }
                // Vertex 3: right, bottom, front
            case RIGHT_BOTTOM_FRONT: {
                genBoxOffsets(x, y - 1, z + 1);
                break;
            }
                // Vertex 4: right, top, front
            case RIGHT_TOP_FRONT: {
                genBoxOffsets(x, y, z + 1);
                break;
            }

                // Vertex 5: left, top, back
            case LEFT_TOP_BACK: {
                genBoxOffsets(x - 1, y, z);
                break;
            }
                // Vertex 6: left, bottom, back
            case LEFT_BOTTOM_BACK: {
                genBoxOffsets(x - 1, y - 1, z);
                break;
            }
                // Vertex 7: right, bottom, back
            case RIGHT_BOTTOM_BACK: {
                genBoxOffsets(x, y - 1, z);
                break;
            }
                // Vertex 8: right, top, back
            case RIGHT_TOP_BACK: {
                genBoxOffsets(x, y, z);
                break;
            }
        }
    }

    // generates a voxel box, corner is left bottom front
    inline void genBoxOffsets(i32 x, i32 y, i32 z)
    {
        // front
        positions[LEFT_TOP_FRONT] = {x, y+1, z}; // left top front
        positions[LEFT_BOTTOM_FRONT] = {x, y , z};  // left bottom front
        positions[RIGHT_BOTTOM_FRONT] = {x+1, y, z}; // right bottom front
        positions[RIGHT_TOP_FRONT] = {x+1, y+1, z}; // right top front
        // back
        positions[LEFT_TOP_BACK] = {x, y+1, z-1}; // left top back
        positions[LEFT_BOTTOM_BACK] = {x, y , z-1};  // left bottom back
        positions[RIGHT_BOTTOM_BACK] = {x+1, y, z-1}; // right bottom back
        positions[RIGHT_TOP_BACK] = {x+1, y+1, z-1}; // right top back
    }
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

struct AOBlock
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

#endif //GAME_VERTEXBLOCKNEIGHBOURS_H
