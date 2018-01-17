//
// Created by bison on 26-12-2017.
//

#include "Chunk.h"
#include "AOBlock.h"

#define GLEW_STATIC

#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

// cache friendly order y,z,x
Chunk::Chunk(BlockTypeDictionary &blockTypeDict, IChunkManager *chunkManager) : blockTypeDict(blockTypeDict), chunkManager(chunkManager) {
    blocks = (Block*) malloc(CHUNK_HEIGHT*CHUNK_SIZE*CHUNK_SIZE*sizeof(Block));
    lightMapSize = CHUNK_HEIGHT*CHUNK_SIZE*CHUNK_SIZE*sizeof(u8);
    lightMap = (u8*) malloc(lightMapSize);
    //memset(blocks, 0, sizeof(blocks));
}

Chunk::~Chunk() {
    // Delete the blocks
    free(blocks);
    free(lightMap);
}

void Chunk::setupDebugChunk()
{
    clearLightMap();
    u8 temple_block = BLOCK_STONE;

    for(int y = 0; y < CHUNK_HEIGHT; y++)
    {
        for(int z = 0; z < CHUNK_SIZE; z++)
        {
            for(int x = 0; x < CHUNK_SIZE; x++)
            {
                // clear
                blocks[POS_TO_INDEX(y,z,x)].setFlag(BLOCK_FLAG_ACTIVE, false);
                blocks[POS_TO_INDEX(y,z,x)].setFlag(BLOCK_FLAG_SHOULD_MESH, false);
                blocks[POS_TO_INDEX(y,z,x)].setFlag(BLOCK_FLAG_TRANSPARENT, true);

                if(y < 100)
                {
                    blocks[POS_TO_INDEX(y,z,x)].setFlag(BLOCK_FLAG_ACTIVE, true);
                    blocks[POS_TO_INDEX(y,z,x)].setFlag(BLOCK_FLAG_TRANSPARENT, false);
                    blocks[POS_TO_INDEX(y,z,x)].type = BLOCK_GRASS;
                }

                if(y == 100)
                {
                    blocks[POS_TO_INDEX(y,z,x)].setFlag(BLOCK_FLAG_ACTIVE, true);
                    blocks[POS_TO_INDEX(y,z,x)].setFlag(BLOCK_FLAG_TRANSPARENT, false);
                    blocks[POS_TO_INDEX(y,z,x)].type = temple_block;
                }

                if(y == 106)
                {
                    blocks[POS_TO_INDEX(y,z,x)].setFlag(BLOCK_FLAG_ACTIVE, true);
                    blocks[POS_TO_INDEX(y,z,x)].setFlag(BLOCK_FLAG_TRANSPARENT, false);
                    blocks[POS_TO_INDEX(y,z,x)].type = temple_block;
                }

                // punch a few holes
                if(y == 106 && x == 4 && z == 4)
                {
                    blocks[POS_TO_INDEX(y,z,x)].setFlag(BLOCK_FLAG_ACTIVE, false);
                    blocks[POS_TO_INDEX(y,z,x)].setFlag(BLOCK_FLAG_TRANSPARENT, true);
                }

                // wall
                if(x >= 6 && x <= 12 && z == 8 && y >= 100 && y <= 105)
                {
                    blocks[POS_TO_INDEX(y,z,x)].setFlag(BLOCK_FLAG_ACTIVE, true);
                    blocks[POS_TO_INDEX(y,z,x)].setFlag(BLOCK_FLAG_TRANSPARENT, false);
                    blocks[POS_TO_INDEX(y,z,x)].type = temple_block;
                }

                // hole
                if(x == 8 && z == 8 && y == 103)
                {
                    blocks[POS_TO_INDEX(y,z,x)].setFlag(BLOCK_FLAG_ACTIVE, true);
                    blocks[POS_TO_INDEX(y,z,x)].setFlag(BLOCK_FLAG_TRANSPARENT, true);
                    blocks[POS_TO_INDEX(y,z,x)].type = BLOCK_WATER;
                }
                // wall 2
                if(z >= 6 && z <= 12 && x == 5 && y >= 100 && y <= 105)
                {
                    blocks[POS_TO_INDEX(y,z,x)].setFlag(BLOCK_FLAG_ACTIVE, true);
                    blocks[POS_TO_INDEX(y,z,x)].setFlag(BLOCK_FLAG_TRANSPARENT, false);
                    blocks[POS_TO_INDEX(y,z,x)].type = temple_block;
                }

                // wall 2
                if(z >= 6 && z <= 12 && x == 12 && y >= 100 && y <= 105)
                {
                    blocks[POS_TO_INDEX(y,z,x)].setFlag(BLOCK_FLAG_ACTIVE, true);
                    blocks[POS_TO_INDEX(y,z,x)].setFlag(BLOCK_FLAG_TRANSPARENT, false);
                    blocks[POS_TO_INDEX(y,z,x)].type = temple_block;
                }

                if(x == 14 && z == 1 && y >= 100 && y <= 110
                   || x == 14 && z == 14 && y >= 100 && y <= 110
                   || x == 1 && z == 14 && y >= 100 && y <= 110
                   || x == 1 && z == 1 && y >= 100 && y <= 110)
                {
                    blocks[POS_TO_INDEX(y,z,x)].setFlag(BLOCK_FLAG_ACTIVE, true);
                    blocks[POS_TO_INDEX(y,z,x)].setFlag(BLOCK_FLAG_TRANSPARENT, false);
                    blocks[POS_TO_INDEX(y,z,x)].type = temple_block;
                }
                /*
                if(x == 8 && z == 11 && y == 103)
                {
                    blocks[POS_TO_INDEX(y,z,x)].setFlag(BLOCK_FLAG_ACTIVE, true);
                    blocks[POS_TO_INDEX(y,z,x)].setFlag(BLOCK_FLAG_TRANSPARENT, false);
                    blocks[POS_TO_INDEX(y,z,x)].type = BLOCK_GOLD;
                }
                 */
            }
        }
    }
    //placeTorchLight(7, 102, 7, 14);
    //auto ll = getTorchlight(7, 105, 7);
    //SDL_Log("light level read %d", ll);
}

// cache friendly order y,z,x
void Chunk::setupFromTerrain(const ChunkPos& position, const std::shared_ptr<Terrain>& terrain)
{
    clearLightMap();
    int half_terrain_size = terrain->getHeightMap()->getWidth()/2;
    int half_chunk_size = CHUNK_SIZE/2;
    float fchunk_height = (float) CHUNK_HEIGHT;

    // to map coords
    auto chk_map_x = (position.x * CHUNK_SIZE) + half_terrain_size;
    auto chk_map_z = (position.z * CHUNK_SIZE) + half_terrain_size;
    //SDL_Log("Chunkpos %d,%d mapped to chunk map pos %d,%d", position.x, position.z, chk_map_x, chk_map_z);

    for(int y = 0; y < CHUNK_HEIGHT; y++)
    {
        for(int z = 0; z < CHUNK_SIZE; z++)
        {
            //SDL_Log("Sampled height %d at map pos %d,%d", height, map_x, map_z);
            for(int x = 0; x < CHUNK_SIZE; x++)
            {
                auto map_x = chk_map_x + x - half_chunk_size;
                auto map_z = chk_map_z + z - half_chunk_size;
                auto height = (int) ceil(terrain->getHeightMap()->getSampleWrap(map_x, map_z) * fchunk_height);

                //SDL_Log("(%d,%d,%d) Index = %d", y, z, x, POS_TO_INDEX(y,z,x));
                // set initial state of blocks (clearing)
                blocks[POS_TO_INDEX(y,z,x)].setFlag(BLOCK_FLAG_ACTIVE, false);
                blocks[POS_TO_INDEX(y,z,x)].setFlag(BLOCK_FLAG_TRANSPARENT, true);
                blocks[POS_TO_INDEX(y,z,x)].setFlag(BLOCK_FLAG_SHOULD_MESH, false);
                blocks[POS_TO_INDEX(y,z,x)].type = BLOCK_GRASS;
                if(y <= height)
                {
                    blocks[POS_TO_INDEX(y,z,x)].setFlag(BLOCK_FLAG_ACTIVE, true);
                    blocks[POS_TO_INDEX(y,z,x)].setFlag(BLOCK_FLAG_TRANSPARENT, false);
                    blocks[POS_TO_INDEX(y,z,x)].type = BLOCK_GRASS;
                    if(y >= 0 && y <= 5)
                    {
                        blocks[POS_TO_INDEX(y,z,x)].setFlag(BLOCK_FLAG_TRANSPARENT, false);
                        blocks[POS_TO_INDEX(y,z,x)].type = BLOCK_STONE;
                    }
                    if(y >= 6 && y <= 10) {
                        auto off = rand() % 4;
                        blocks[POS_TO_INDEX(y,z,x)].setFlag(BLOCK_FLAG_TRANSPARENT, false);
                        blocks[POS_TO_INDEX(y, z, x)].type = BLOCK_EMERALD + off;
                    }

                }
                if(y > height && y <= 2)
                {
                    blocks[POS_TO_INDEX(y,z,x)].setFlag(BLOCK_FLAG_ACTIVE, true);
                    blocks[POS_TO_INDEX(y,z,x)].setFlag(BLOCK_FLAG_TRANSPARENT, true);
                    blocks[POS_TO_INDEX(y,z,x)].type = BLOCK_WATER;
                }
            }
        }
    }
}

// cache friendly order y,z,x
void Chunk::rebuild(const ChunkPos& position) {
    bool first_build = false;
    if(mesh == nullptr) {
        first_build = true;
        mesh = std::unique_ptr<BlockMesh>(new BlockMesh());
    }
    if(!first_build)
        mesh->clear();

    materialBatchMap.clear();

    // place torchlights
    /*
    if(position.x == 0 && position.z == 0)
    {
        chunkManager->placeTorchLight(this, 7, 107, 7, 14);
        chunkManager->placeTorchLight(this, 8, 103, 11, 15);
    }
    */

    auto m = glm::mat4();
    auto origin = glm::vec4(0,0,0,1);
    m = glm::translate(m, glm::vec3(-0.5*CHUNK_SIZE, 0.0f, -0.5*CHUNK_SIZE));
    for(int y = 0; y < CHUNK_HEIGHT; y++)
    {
        glm::mat4 old = m;
        for(int z = 0; z < CHUNK_SIZE; z++)
        {
            m = glm::translate(m, glm::vec3(0.0f, 0.0f, 1.0f));
            auto z_m = m;
            for(int x = 0; x < CHUNK_SIZE; x++)
            {
                Block &block = blocks[POS_TO_INDEX(y,z,x)];
                if(block.isFlagSet(BLOCK_FLAG_ACTIVE)) {
                    auto pos = z_m * origin;
                    auto should_mesh = false;
                    if (!isBlockActiveAt(x, y, z - 1)) should_mesh = true; // back
                    if (!isBlockActiveAt(x, y, z + 1)) should_mesh = true; // front
                    if (!isBlockActiveAt(x - 1, y, z)) should_mesh = true; // left
                    if (!isBlockActiveAt(x + 1, y, z)) should_mesh = true; // right
                    if (!isBlockActiveAt(x, y - 1, z)) should_mesh = true; // bottom
                    if (!isBlockActiveAt(x, y + 1, z)) should_mesh = true; // top

                    if (should_mesh) {
                        block.setFlag(BLOCK_FLAG_SHOULD_MESH, true);
                        auto batch = materialBatchMap.find(block.type);
                        if(batch == materialBatchMap.end()) // batch does not exist, create
                        {
                            materialBatchMap[block.type] = std::unique_ptr<MaterialBatch>(new MaterialBatch());
                            materialBatchMap[block.type]->blockType = block.type;
                            materialBatchMap[block.type]->blocks.emplace_back(x,y,z, getTorchlight(x,y,z));
                            //SDL_Log("Generated cube at %d,%d,%d", x, y, z);
                        }
                        else {
                            batch->second->blocks.emplace_back(x, y, z, getTorchlight(x,y,z));
                            //SDL_Log("Generated cube at %d,%d,%d", x, y, z);
                        }
                    }
                }
                z_m = glm::translate(z_m, glm::vec3(1.0f, 0.0f, 0.0f));
            }
        }
        m = old;
        m = glm::translate(m, glm::vec3(0.0f, 1.0f, 0.0f));
    }

    BlockLight block_light;
    AOBlock ao_block;

    for(auto const& kv : materialBatchMap)
    {
        kv.second->start = static_cast<u32>(mesh->vertices.size());
        for(auto& mb : kv.second->blocks)
        {
            Block &block = blocks[POS_TO_INDEX(mb.y,mb.z,mb.x)];
            if(block.isFlagSet(BLOCK_FLAG_SHOULD_MESH) && block.isFlagSet(BLOCK_FLAG_ACTIVE))
            {
                mb.faces.disableAll();
                // back
                if (!shouldBlockMeshAt(mb.x, mb.y, mb.z - 1)) {
                    mb.faces.back = true; // back
                    //face_light.back = getTorchLightAt(mb.x, mb.y, mb.z - 1);
                }
                // front
                if (!shouldBlockMeshAt(mb.x, mb.y, mb.z + 1)) {
                    mb.faces.front = true;
                    //face_light.front = getTorchLightAt(mb.x, mb.y, mb.z + 1);
                }
                // left
                if (!shouldBlockMeshAt(mb.x - 1, mb.y, mb.z)) {
                    mb.faces.left = true;
                    //face_light.left = getTorchLightAt(mb.x - 1, mb.y, mb.z);
                }
                // right
                if (!shouldBlockMeshAt(mb.x + 1, mb.y, mb.z)) {
                    mb.faces.right = true;
                    //face_light.right = getTorchLightAt(mb.x + 1, mb.y, mb.z);
                }
                // bottom
                if (!shouldBlockMeshAt(mb.x, mb.y - 1, mb.z)) {
                    mb.faces.bottom = true;
                    //face_light.bottom = getTorchLightAt(mb.x, mb.y - 1, mb.z);
                }
                // top
                if (!shouldBlockMeshAt(mb.x, mb.y + 1, mb.z)) {
                    mb.faces.top = true;
                    //face_light.top = getTorchLightAt(mb.x, mb.y + 1, mb.z);

                }
            }

            //SDL_Log("Meshing cube at %d,%d,%d", mb.x, mb.y, mb.z);
            if(mb.faces.anyFacesEnabled()) {
                calculateAO(ao_block, mb);
                calculateBlockLight(block_light, mb);

                mesh->generateTexturedCubeAt(mb.x, mb.y, mb.z, mb.faces, block_light, ao_block);
            }
        }
        kv.second->count = static_cast<u32>(mesh->vertices.size() - kv.second->start);
    }
    if(first_build)
        mesh->prepare();
    else
        mesh->upload();
}

void Chunk::calculateBlockLight(BlockLight& block_light, MaterialBlock& mb)
{
    i32 count;
    if(mb.faces.back)
    {
        //block_light.faces[BACK_FACE].v1 = block_light.faces[BACK_FACE].v2 = block_light.faces[BACK_FACE].v3 = block_light.faces[BACK_FACE].v4 = getTorchLightAt(mb.x, mb.y, mb.z - 1);

        count = 0;
        block_light.faces[BACK_FACE].v1 = (u8) (getTorchLightAndIncreaseCount(mb.x, mb.y, mb.z - 1, count)
                                                 + getTorchLightAndIncreaseCount(mb.x + 1, mb.y, mb.z - 1, count)
                                                 + getTorchLightAndIncreaseCount(mb.x, mb.y + 1, mb.z - 1, count)
                                                 + getTorchLightAndIncreaseCount(mb.x + 1, mb.y + 1, mb.z - 1, count));
        if(count > 0) block_light.faces[BACK_FACE].v1 /= count;
        count = 0;
        block_light.faces[BACK_FACE].v2 = (u8) (getTorchLightAndIncreaseCount(mb.x, mb.y, mb.z - 1, count)
                                                 + getTorchLightAndIncreaseCount(mb.x + 1, mb.y, mb.z - 1, count)
                                                 + getTorchLightAndIncreaseCount(mb.x, mb.y - 1, mb.z - 1, count)
                                                 + getTorchLightAndIncreaseCount(mb.x + 1, mb.y - 1, mb.z - 1, count));
        if(count > 0) block_light.faces[BACK_FACE].v2 /= count;
        count = 0;
        block_light.faces[BACK_FACE].v3 = (u8) (getTorchLightAndIncreaseCount(mb.x, mb.y, mb.z - 1, count)
                                                 + getTorchLightAndIncreaseCount(mb.x - 1, mb.y, mb.z - 1, count)
                                                 + getTorchLightAndIncreaseCount(mb.x, mb.y - 1, mb.z - 1, count)
                                                 + getTorchLightAndIncreaseCount(mb.x - 1, mb.y - 1, mb.z - 1, count));
        if(count > 0) block_light.faces[BACK_FACE].v3 /= count;
        count = 0;
        block_light.faces[BACK_FACE].v4 = (u8) (getTorchLightAndIncreaseCount(mb.x, mb.y, mb.z - 1, count)
                                                 + getTorchLightAndIncreaseCount(mb.x - 1, mb.y, mb.z - 1, count)
                                                 + getTorchLightAndIncreaseCount(mb.x, mb.y + 1, mb.z - 1, count)
                                                 + getTorchLightAndIncreaseCount(mb.x - 1, mb.y + 1, mb.z - 1, count));
        if(count > 0) block_light.faces[BACK_FACE].v4 /= count;
    }
    if(mb.faces.front)
    {
        count = 0;
        block_light.faces[FRONT_FACE].v1 = (u8) (getTorchLightAndIncreaseCount(mb.x, mb.y, mb.z + 1, count)
                                           + getTorchLightAndIncreaseCount(mb.x - 1, mb.y, mb.z + 1, count)
                                             + getTorchLightAndIncreaseCount(mb.x, mb.y + 1, mb.z + 1, count)
                                               + getTorchLightAndIncreaseCount(mb.x - 1, mb.y + 1, mb.z + 1, count));
        if(count > 0) block_light.faces[FRONT_FACE].v1 /= count;
        count = 0;
        block_light.faces[FRONT_FACE].v2 = (u8) (getTorchLightAndIncreaseCount(mb.x, mb.y, mb.z + 1, count)
                                                  + getTorchLightAndIncreaseCount(mb.x - 1, mb.y, mb.z + 1, count)
                                                  + getTorchLightAndIncreaseCount(mb.x, mb.y - 1, mb.z + 1, count)
                                                  + getTorchLightAndIncreaseCount(mb.x - 1, mb.y - 1, mb.z + 1, count));
        if(count > 0) block_light.faces[FRONT_FACE].v2 /= count;
        count = 0;
        block_light.faces[FRONT_FACE].v3 = (u8) (getTorchLightAndIncreaseCount(mb.x, mb.y, mb.z + 1, count)
                                                  + getTorchLightAndIncreaseCount(mb.x + 1, mb.y, mb.z + 1, count)
                                                  + getTorchLightAndIncreaseCount(mb.x, mb.y - 1, mb.z + 1, count)
                                                  + getTorchLightAndIncreaseCount(mb.x + 1, mb.y - 1, mb.z + 1, count));
        if(count > 0) block_light.faces[FRONT_FACE].v3 /= count;
        count = 0;
        block_light.faces[FRONT_FACE].v4 = (u8) (getTorchLightAndIncreaseCount(mb.x, mb.y, mb.z + 1, count)
                                                  + getTorchLightAndIncreaseCount(mb.x + 1, mb.y, mb.z + 1, count)
                                                  + getTorchLightAndIncreaseCount(mb.x, mb.y + 1, mb.z + 1, count)
                                                  + getTorchLightAndIncreaseCount(mb.x + 1, mb.y + 1, mb.z + 1, count));
        if(count > 0) block_light.faces[FRONT_FACE].v4 /= count;
    }
    if(mb.faces.left)
    {
        //block_light.faces[LEFT_FACE].v1 = block_light.faces[LEFT_FACE].v2 = block_light.faces[LEFT_FACE].v3 = block_light.faces[LEFT_FACE].v4 = getTorchLightAt(mb.x - 1, mb.y, mb.z);
        count = 0;
        block_light.faces[LEFT_FACE].v1 = (u8) (getTorchLightAndIncreaseCount(mb.x - 1, mb.y, mb.z, count)
                                                 + getTorchLightAndIncreaseCount(mb.x - 1, mb.y, mb.z - 1, count)
                                                 + getTorchLightAndIncreaseCount(mb.x - 1, mb.y + 1, mb.z, count)
                                                 + getTorchLightAndIncreaseCount(mb.x - 1, mb.y + 1, mb.z - 1, count));
        if(count > 0) block_light.faces[LEFT_FACE].v1 /= count;
        count = 0;
        block_light.faces[LEFT_FACE].v2 = (u8) (getTorchLightAndIncreaseCount(mb.x - 1, mb.y, mb.z, count)
                                                 + getTorchLightAndIncreaseCount(mb.x - 1, mb.y, mb.z - 1, count)
                                                 + getTorchLightAndIncreaseCount(mb.x - 1, mb.y - 1, mb.z, count)
                                                 + getTorchLightAndIncreaseCount(mb.x - 1, mb.y - 1, mb.z - 1, count));
        if(count > 0) block_light.faces[LEFT_FACE].v2 /= count;
        count = 0;
        block_light.faces[LEFT_FACE].v3 = (u8) (getTorchLightAndIncreaseCount(mb.x - 1, mb.y, mb.z, count)
                                                 + getTorchLightAndIncreaseCount(mb.x - 1, mb.y, mb.z + 1, count)
                                                 + getTorchLightAndIncreaseCount(mb.x - 1, mb.y - 1, mb.z, count)
                                                 + getTorchLightAndIncreaseCount(mb.x - 1, mb.y - 1, mb.z + 1, count));
        if(count > 0) block_light.faces[LEFT_FACE].v3 /= count;
        count = 0;
        block_light.faces[LEFT_FACE].v4 = (u8) (getTorchLightAndIncreaseCount(mb.x - 1, mb.y, mb.z, count)
                                                 + getTorchLightAndIncreaseCount(mb.x - 1, mb.y, mb.z + 1, count)
                                                 + getTorchLightAndIncreaseCount(mb.x - 1, mb.y + 1, mb.z, count)
                                                 + getTorchLightAndIncreaseCount(mb.x - 1, mb.y + 1, mb.z + 1, count));
        if(count > 0) block_light.faces[LEFT_FACE].v4 /= count;
    }
    if(mb.faces.right)
    {
        //block_light.faces[RIGHT_FACE].v1 = block_light.faces[RIGHT_FACE].v2 = block_light.faces[RIGHT_FACE].v3 = block_light.faces[RIGHT_FACE].v4 = getTorchLightAt(mb.x + 1, mb.y, mb.z);
        count = 0;
        block_light.faces[RIGHT_FACE].v1 = (u8) (getTorchLightAndIncreaseCount(mb.x + 1, mb.y, mb.z, count)
                                                + getTorchLightAndIncreaseCount(mb.x + 1, mb.y, mb.z + 1, count)
                                                + getTorchLightAndIncreaseCount(mb.x + 1, mb.y + 1, mb.z, count)
                                                + getTorchLightAndIncreaseCount(mb.x + 1, mb.y + 1, mb.z + 1, count));
        if(count > 0) block_light.faces[RIGHT_FACE].v1 /= count;
        count = 0;
        block_light.faces[RIGHT_FACE].v2 = (u8) (getTorchLightAndIncreaseCount(mb.x + 1, mb.y, mb.z, count)
                                                + getTorchLightAndIncreaseCount(mb.x + 1, mb.y, mb.z + 1, count)
                                                + getTorchLightAndIncreaseCount(mb.x + 1, mb.y - 1, mb.z, count)
                                                + getTorchLightAndIncreaseCount(mb.x + 1, mb.y - 1, mb.z + 1, count));
        if(count > 0) block_light.faces[RIGHT_FACE].v2 /= count;
        count = 0;
        block_light.faces[RIGHT_FACE].v3 = (u8) (getTorchLightAndIncreaseCount(mb.x + 1, mb.y, mb.z, count)
                                                + getTorchLightAndIncreaseCount(mb.x + 1, mb.y, mb.z - 1, count)
                                                + getTorchLightAndIncreaseCount(mb.x + 1, mb.y - 1, mb.z, count)
                                                + getTorchLightAndIncreaseCount(mb.x + 1, mb.y - 1, mb.z - 1, count));
        if(count > 0) block_light.faces[RIGHT_FACE].v3 /= count;
        count = 0;
        block_light.faces[RIGHT_FACE].v4 = (u8) (getTorchLightAndIncreaseCount(mb.x + 1, mb.y, mb.z, count)
                                                + getTorchLightAndIncreaseCount(mb.x + 1, mb.y, mb.z - 1, count)
                                                + getTorchLightAndIncreaseCount(mb.x + 1, mb.y + 1, mb.z, count)
                                                + getTorchLightAndIncreaseCount(mb.x + 1, mb.y + 1, mb.z - 1, count));
        if(count > 0) block_light.faces[RIGHT_FACE].v4 /= count;
    }
    if(mb.faces.bottom)
    {
        //block_light.faces[BOTTOM_FACE].v1 = block_light.faces[BOTTOM_FACE].v2 = block_light.faces[BOTTOM_FACE].v3 = block_light.faces[BOTTOM_FACE].v4 = getTorchLightAt(mb.x, mb.y - 1, mb.z);
        count = 0;
        block_light.faces[BOTTOM_FACE].v1 = (u8) (getTorchLightAndIncreaseCount(mb.x, mb.y - 1, mb.z, count)
                                               + getTorchLightAndIncreaseCount(mb.x, mb.y - 1, mb.z + 1, count)
                                               + getTorchLightAndIncreaseCount(mb.x - 1, mb.y - 1, mb.z, count)
                                               + getTorchLightAndIncreaseCount(mb.x - 1, mb.y - 1, mb.z + 1, count));
        if(count > 0) block_light.faces[BOTTOM_FACE].v1 /= count;
        count = 0;
        block_light.faces[BOTTOM_FACE].v2 = (u8) (getTorchLightAndIncreaseCount(mb.x, mb.y - 1, mb.z, count)
                                               + getTorchLightAndIncreaseCount(mb.x, mb.y - 1, mb.z - 1, count)
                                               + getTorchLightAndIncreaseCount(mb.x - 1, mb.y - 1, mb.z, count)
                                               + getTorchLightAndIncreaseCount(mb.x - 1, mb.y - 1, mb.z - 1, count));
        if(count > 0) block_light.faces[BOTTOM_FACE].v2 /= count;
        count = 0;
        block_light.faces[BOTTOM_FACE].v3 = (u8) (getTorchLightAndIncreaseCount(mb.x, mb.y - 1, mb.z, count)
                                               + getTorchLightAndIncreaseCount(mb.x, mb.y - 1, mb.z - 1, count)
                                               + getTorchLightAndIncreaseCount(mb.x + 1, mb.y - 1, mb.z, count)
                                               + getTorchLightAndIncreaseCount(mb.x + 1, mb.y - 1, mb.z - 1, count));
        if(count > 0) block_light.faces[BOTTOM_FACE].v3 /= count;
        count = 0;
        block_light.faces[BOTTOM_FACE].v4 = (u8) (getTorchLightAndIncreaseCount(mb.x, mb.y - 1, mb.z, count)
                                               + getTorchLightAndIncreaseCount(mb.x, mb.y - 1, mb.z + 1, count)
                                               + getTorchLightAndIncreaseCount(mb.x + 1, mb.y - 1, mb.z, count)
                                               + getTorchLightAndIncreaseCount(mb.x + 1, mb.y - 1, mb.z + 1, count));
        if(count > 0) block_light.faces[BOTTOM_FACE].v4 /= count;
    }
    if(mb.faces.top)
    {
        //block_light.faces[TOP_FACE].v1 = block_light.faces[TOP_FACE].v2 = block_light.faces[TOP_FACE].v3 = block_light.faces[TOP_FACE].v4 = getTorchLightAt(mb.x, mb.y + 1, mb.z);
        count = 0;
        block_light.faces[TOP_FACE].v1 = (u8) (getTorchLightAndIncreaseCount(mb.x, mb.y + 1, mb.z, count)
                                                 + getTorchLightAndIncreaseCount(mb.x, mb.y + 1, mb.z - 1, count)
                                                 + getTorchLightAndIncreaseCount(mb.x - 1, mb.y + 1, mb.z, count)
                                                 + getTorchLightAndIncreaseCount(mb.x - 1, mb.y + 1, mb.z - 1, count));
        if(count > 0) block_light.faces[TOP_FACE].v1 /= count;
        count = 0;
        block_light.faces[TOP_FACE].v2 = (u8) (getTorchLightAndIncreaseCount(mb.x, mb.y + 1, mb.z, count)
                                                 + getTorchLightAndIncreaseCount(mb.x, mb.y + 1, mb.z + 1, count)
                                                 + getTorchLightAndIncreaseCount(mb.x - 1, mb.y + 1, mb.z, count)
                                                 + getTorchLightAndIncreaseCount(mb.x - 1, mb.y + 1, mb.z + 1, count));
        if(count > 0) block_light.faces[TOP_FACE].v2 /= count;
        count = 0;
        block_light.faces[TOP_FACE].v3 = (u8) (getTorchLightAndIncreaseCount(mb.x, mb.y + 1, mb.z, count)
                                                 + getTorchLightAndIncreaseCount(mb.x, mb.y + 1, mb.z + 1, count)
                                                 + getTorchLightAndIncreaseCount(mb.x + 1, mb.y + 1, mb.z, count)
                                                 + getTorchLightAndIncreaseCount(mb.x + 1, mb.y + 1, mb.z + 1, count));
        if(count > 0) block_light.faces[TOP_FACE].v3 /= count;
        count = 0;
        block_light.faces[TOP_FACE].v4 = (u8) (getTorchLightAndIncreaseCount(mb.x, mb.y + 1, mb.z, count)
                                                 + getTorchLightAndIncreaseCount(mb.x, mb.y + 1, mb.z - 1, count)
                                                 + getTorchLightAndIncreaseCount(mb.x + 1, mb.y + 1, mb.z, count)
                                                 + getTorchLightAndIncreaseCount(mb.x + 1, mb.y + 1, mb.z - 1, count));
        if(count > 0) block_light.faces[TOP_FACE].v4 /= count;
    }
}

/*
 * Produces ao shading factor for a vertex based on 2 sides and a corner value, thus producing 4 unique values
 * from the combinations of 3 sides
 */
static inline i32 vertexAO(i32 side1, i32 side2, i32 corner) {
    // if both sides are enabled we don't need to check the corner (since its occluded and doesn't contribute to the shading)
    if(side1 && side2) {
        return 0;
    }
    return 3 - (side1 + side2 + corner);
}


void Chunk::calculateAO(AOBlock &aob, MaterialBlock& mb)
{
    aob.calcPositions(mb.x, mb.y, mb.z);
    for(auto &face : aob.faces)
    {
        for(auto &vertex : face.vertices)
        {
            vertex.AO = vertexAO((isBlockActiveAt(vertex.positions[SIDE1].x, vertex.positions[SIDE1].y, vertex.positions[SIDE1].z) ? 1 : 0),
                                 (isBlockActiveAt(vertex.positions[SIDE2].x, vertex.positions[SIDE2].y, vertex.positions[SIDE2].z) ? 1 : 0),
                                 (isBlockActiveAt(vertex.positions[CORNER].x, vertex.positions[CORNER].y, vertex.positions[CORNER].z) ? 1 : 0));

        }
    }
}

void Chunk::draw(const Shader &shader) {
    //SDL_Log("Rendering %d batches", materialBatchMap.size());
    //mesh->drawRange(shader, 0, mesh->vertices.size(), &blockTypeDict.getBlockTypeAt(0).material);
    for(auto const& kv : materialBatchMap)
    {
        //sum += kv.second->count;
        //SDL_Log("Rendering batch: blocks = %d, start = %d, count = %d, blockType = %d", kv.second->blocks.size(), kv.second->start, kv.second->count, kv.first);
        mesh->drawRange(shader, kv.second->start, kv.second->count, &blockTypeDict.getBlockTypeAt(kv.first).material);
    }
    //SDL_Log("Sum of batch counts = %d, size of mesh = %d", sum, mesh->vertices.size());
}

void Chunk::clearLightMap() {
    // clear lightmap
    memset(lightMap, 0, lightMapSize);
}


