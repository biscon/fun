//
// Created by bison on 26-12-2017.
//

#include "Chunk.h"
#include "VertexBlockNeighbours.h"

#define GLEW_STATIC

#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

// cache friendly order y,z,x
Chunk::Chunk(BlockTypeDictionary &blockTypeDict) : blockTypeDict(blockTypeDict) {
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

                // wall
                if(x >= 6 && x <= 12 && z == 8 && y >= 100 && y <= 105)
                {
                    blocks[POS_TO_INDEX(y,z,x)].setFlag(BLOCK_FLAG_ACTIVE, true);
                    blocks[POS_TO_INDEX(y,z,x)].setFlag(BLOCK_FLAG_TRANSPARENT, false);
                    blocks[POS_TO_INDEX(y,z,x)].type = temple_block;
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
                if(x == 7 && z == 11 && y == 110)
                {
                    blocks[POS_TO_INDEX(y,z,x)].setFlag(BLOCK_FLAG_ACTIVE, true);
                    blocks[POS_TO_INDEX(y,z,x)].setFlag(BLOCK_FLAG_TRANSPARENT, false);
                    blocks[POS_TO_INDEX(y,z,x)].type = BLOCK_GOLD;
                }
            }
        }
    }
    //placeTorchLight(7, 102, 11, 14);
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
void Chunk::rebuild(const ChunkPos& position, ChunkNeighbours& neighbours) {
    bool first_build = false;
    if(mesh == nullptr) {
        first_build = true;
        mesh = std::unique_ptr<BlockMesh>(new BlockMesh());
    }
    if(!first_build)
        mesh->clear();

    materialBatchMap.clear();

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
                    if (!isBlockActiveAt(neighbours, x, y, z - 1)) should_mesh = true; // back
                    if (!isBlockActiveAt(neighbours, x, y, z + 1)) should_mesh = true; // front
                    if (!isBlockActiveAt(neighbours, x - 1, y, z)) should_mesh = true; // left
                    if (!isBlockActiveAt(neighbours, x + 1, y, z)) should_mesh = true; // right
                    if (!isBlockActiveAt(neighbours, x, y - 1, z)) should_mesh = true; // bottom
                    if (!isBlockActiveAt(neighbours, x, y + 1, z)) should_mesh = true; // top

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

    FaceLight face_light;
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
                if (!shouldBlockMeshAt(neighbours, mb.x, mb.y, mb.z - 1)) {
                    mb.faces.back = true; // back
                    face_light.back = getLightTorchLightAt(neighbours, mb.x, mb.y, mb.z - 1);
                }
                // front
                if (!shouldBlockMeshAt(neighbours, mb.x, mb.y, mb.z + 1)) {
                    mb.faces.front = true;
                    face_light.front = getLightTorchLightAt(neighbours, mb.x, mb.y, mb.z + 1);
                }
                // left
                if (!shouldBlockMeshAt(neighbours, mb.x - 1, mb.y, mb.z)) {
                    mb.faces.left = true;
                    face_light.left = getLightTorchLightAt(neighbours, mb.x -1, mb.y, mb.z);
                }
                // right
                if (!shouldBlockMeshAt(neighbours, mb.x + 1, mb.y, mb.z)) {
                    mb.faces.right = true;
                    face_light.right = getLightTorchLightAt(neighbours, mb.x + 1, mb.y, mb.z);
                }
                // bottom
                if (!shouldBlockMeshAt(neighbours, mb.x, mb.y - 1, mb.z)) {
                    mb.faces.bottom = true;
                    face_light.bottom = getLightTorchLightAt(neighbours, mb.x, mb.y - 1, mb.z);
                }
                // top
                if (!shouldBlockMeshAt(neighbours, mb.x, mb.y + 1, mb.z)) {
                    mb.faces.top = true;
                    face_light.top = getLightTorchLightAt(neighbours, mb.x, mb.y + 1, mb.z);

                }
            }


            //SDL_Log("Meshing cube at %d,%d,%d", mb.x, mb.y, mb.z);
            if(mb.faces.anyFacesEnabled()) {
                calculateAO(neighbours, ao_block, mb);

                mesh->generateTexturedCubeAt(mb.x, mb.y, mb.z, mb.faces, face_light, ao_block);
            }
        }
        kv.second->count = static_cast<u32>(mesh->vertices.size() - kv.second->start);
    }
    if(first_build)
        mesh->prepare();
    else
        mesh->upload();
}

static inline i32 vertexAO(i32 side1, i32 side2, i32 corner) {
    if(side1 && side2) {
        return 0;
    }
    return 3 - (side1 + side2 + corner);
}

bool Chunk::isBlockActiveAtClipped(ChunkNeighbours& neighbours, i32 x, i32 y, i32 z)
{
    // early out since our chunk grid is 2d
    if(y < 0)
        return true;
    if(y >= CHUNK_HEIGHT)
        return false;

    // within this chunk
    if(x >= 0 && x < CHUNK_SIZE
       && y >= 0 && y < CHUNK_HEIGHT
       && z >= 0 && z < CHUNK_SIZE) {
        return blocks[POS_TO_INDEX(y, z, x)].isFlagSet(BLOCK_FLAG_ACTIVE);
    }

    // find chunk neighbour offset
    i32 offset_x = 0;
    i32 offset_z = 0;
    // north
    if(z < 0) {
        offset_z = -1;
    }
    // south
    if(z >= CHUNK_SIZE)
    {
        offset_z = 1;
    }
    // west
    if(x < 0) {
        offset_x = -1;
    }
    // east
    if(x >= CHUNK_SIZE)
    {
        offset_x = 1;
    }

    // if north
    if(offset_x == 0 && offset_z == -1)
    {
        if(neighbours.n == nullptr)
            return false;
        auto nz = CHUNK_SIZE + z;
        return neighbours.n->blocks[POS_TO_INDEX(y,nz,x)].isFlagSet(BLOCK_FLAG_ACTIVE);
    }

    // if south
    if(offset_x == 0 && offset_z == 1)
    {
        if(neighbours.s == nullptr)
            return false;
        auto nz = z - CHUNK_SIZE;
        return neighbours.s->blocks[POS_TO_INDEX(y,nz,x)].isFlagSet(BLOCK_FLAG_ACTIVE);
    }

    // if west
    if(offset_x == -1 && offset_z == 0)
    {
        if(neighbours.w == nullptr)
            return false;
        auto nx = CHUNK_SIZE + x;
        return neighbours.w->blocks[POS_TO_INDEX(y,z,nx)].isFlagSet(BLOCK_FLAG_ACTIVE);
    }

    // if east
    if(offset_x == 1 && offset_z == 0)
    {
        if(neighbours.e == nullptr)
            return true;

        auto nx = x - CHUNK_SIZE;
        return neighbours.e->blocks[POS_TO_INDEX(y,z,nx)].isFlagSet(BLOCK_FLAG_ACTIVE);
    }

    // diagonals -------------------------------------------------------------------------

    // if north west
    if(offset_x == -1 && offset_z == -1)
    {
        if(neighbours.nw == nullptr)
            return true;
        auto nx = CHUNK_SIZE + x;
        auto nz = CHUNK_SIZE + z;
        return neighbours.nw->blocks[POS_TO_INDEX(y,nz,nx)].isFlagSet(BLOCK_FLAG_ACTIVE);
    }

    // if north east
    if(offset_x == 1 && offset_z == -1) {
        if (neighbours.ne == nullptr)
            return true;
        auto nz = CHUNK_SIZE + z;
        auto nx = x - CHUNK_SIZE;
        return neighbours.ne->blocks[POS_TO_INDEX(y,nz,nx)].isFlagSet(BLOCK_FLAG_ACTIVE);
    }

    // if south west
    if(offset_x == -1 && offset_z == 1)
    {
        if(neighbours.sw == nullptr)
            return true;
        auto nx = CHUNK_SIZE + x;
        auto nz = z - CHUNK_SIZE;
        return neighbours.sw->blocks[POS_TO_INDEX(y,nz,nx)].isFlagSet(BLOCK_FLAG_ACTIVE);
    }

    // if south east
    if(offset_x == 1 && offset_z == 1)
    {
        if(neighbours.se == nullptr)
            return true;
        auto nx = x - CHUNK_SIZE;
        auto nz = z - CHUNK_SIZE;
        return neighbours.se->blocks[POS_TO_INDEX(y,nz,nx)].isFlagSet(BLOCK_FLAG_ACTIVE);
    }

    SDL_Log("Not handled requested chunk offset %d,%d", offset_x, offset_z);
    return false;
}

void Chunk::calculateAO(ChunkNeighbours& neighbours, AOBlock &aob, MaterialBlock& mb)
{
    aob.calcPositions(mb.x, mb.y, mb.z);
    for(auto &face : aob.faces)
    {
        for(auto &vertex : face.vertices)
        {
            vertex.AO = vertexAO((isBlockActiveAtClipped(neighbours, vertex.positions[SIDE1].x, vertex.positions[SIDE1].y, vertex.positions[SIDE1].z) ? 1 : 0),
                                 (isBlockActiveAtClipped(neighbours, vertex.positions[SIDE2].x, vertex.positions[SIDE2].y, vertex.positions[SIDE2].z) ? 1 : 0),
                                 (isBlockActiveAtClipped(neighbours, vertex.positions[CORNER].x, vertex.positions[CORNER].y, vertex.positions[CORNER].z) ? 1 : 0));

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

void Chunk::propagateTorchLight(Chunk *chunk, i32 x, i32 y, i32 z, i32 lightlevel)
{
    // we need to do the usual neighbour check and fetch here, but for now we just clip
    if(x >= 0 && x < CHUNK_SIZE
                 && y >= 0 && y < CHUNK_HEIGHT
                             && z >= 0 && z < CHUNK_SIZE)
    {
        if (chunk->getTorchlight(x, y, z) + 2 <= lightlevel && chunk->blocks[POS_TO_INDEX(y, z, x)].isFlagSet(BLOCK_FLAG_TRANSPARENT)) {
            // Set its light level
            chunk->setTorchlight(x, y, z, lightlevel - 1);
            //SDL_Log("Setting block %d,%d,%d to light level %d", x, y ,z, lightlevel-1);
            // Construct index
            u16 idx = POS_TO_INDEX(y, z, x);
            // Emplace new node to queue. (could use push as well)
            lightQueue.emplace(idx, chunk);
        }
    }
}

// does a BFS search (brett first search ;)
void Chunk::placeTorchLight(i32 x, i32 y, i32 z, u8 level) {
    SDL_Log("Placing torchlight at %d,%d,%d light level %d", x, y ,z, level);
    setTorchlight(x,y,z,level);
    u16 idx = POS_TO_INDEX(y, z, x);
    //clearLightQueue();
    lightQueue.emplace(idx, this);
    while(!lightQueue.empty())
    {
        // Get a reference to the front node.
        auto &node = lightQueue.front();
        u16 index = node.index;
        Chunk* chunk = node.chunk;
        lightQueue.pop();

        // inflate coords from index
        i32 nx = index % CHUNK_SIZE;
        i32 nz = index / (CHUNK_SIZE * CHUNK_HEIGHT);
        i32 ny = (index % (CHUNK_SIZE * CHUNK_HEIGHT) ) / CHUNK_SIZE;

        // Grab the light level of the current node
        i32 lightlevel = chunk->getTorchlight(nx, ny, nz);

        //SDL_Log("Current lightnode %d,%d,%d light level %d (index = %d)", nx, ny, nz, lightlevel, index);

        // propagate light to all 6 surrounding blocks
        // sides
        propagateTorchLight(chunk, nx - 1, ny, nz, lightlevel);
        propagateTorchLight(chunk, nx + 1, ny, nz, lightlevel);

        // top bottom
        propagateTorchLight(chunk, nx, ny - 1, nz, lightlevel);
        propagateTorchLight(chunk, nx, ny + 1, nz, lightlevel);

        // front and back
        propagateTorchLight(chunk, nx, ny, nz - 1, lightlevel);
        propagateTorchLight(chunk, nx, ny, nz + 1, lightlevel);

    }
}

void Chunk::clearLightMap() {
    // clear lightmap
    memset(lightMap, 0, lightMapSize);
}


