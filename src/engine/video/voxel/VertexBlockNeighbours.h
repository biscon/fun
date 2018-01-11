//
// Created by bison on 11-01-2018.
//

#ifndef GAME_VERTEXBLOCKNEIGHBOURS_H
#define GAME_VERTEXBLOCKNEIGHBOURS_H

#define LEFT_TOP_FRONT 0
#define LEFT_BOTTOM_FRONT 1
#define RIGHT_BOTTOM_FRONT 2
#define RIGHT_TOP_FRONT 3
#define LEFT_TOP_BACK 4
#define LEFT_BOTTOM_BACK 5
#define RIGHT_BOTTOM_BACK 6
#define RIGHT_TOP_BACK 7
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
        positions[0] = {x, y , z};  // left bottom front
        positions[1] = {x+1, y, z}; // right bottom front
        positions[2] = {x, y+1, z}; // left top front
        positions[3] = {x+1, y+1, z}; // right top front
        // back
        positions[4] = {x, y , z-1};  // left bottom back
        positions[5] = {x+1, y, z-1}; // right bottom back
        positions[6] = {x, y+1, z-1}; // left top back
        positions[7] = {x+1, y+1, z-1}; // right top back
    }
};

#endif //GAME_VERTEXBLOCKNEIGHBOURS_H
