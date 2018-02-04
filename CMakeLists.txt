cmake_minimum_required(VERSION 3.8)
project(game)

set(CMAKE_CXX_STANDARD 11)
#set(CMAKE_CXX_FLAGS_DEBUG "-gdwarf-2")
set(CMAKE_CXX_FLAGS_DEBUG "-O3 -mwindows -funroll-loops -fomit-frame-pointer -ftree-loop-distribution -ftree-vectorize")
# uncomment this to compile as gui app (doesnt open the console)
set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++ -static")

# includes cmake/FindSDL2.cmake
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

set(GLM_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/src/glm)

# glew (windows) on os x: brew install glew
if(WIN32)
    set(GLEW_INCLUDE_DIR "E:/glew-2.1.0-win32/glew-2.1.0/include")
    set(GLEW_LIBRARY "E:/glew-2.1.0-win32/glew-2.1.0/lib/Release/x64/glew32s.lib")
endif()


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "../")

find_package(GLEW REQUIRED)
find_package(OPENGL REQUIRED)
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIR} ${GLEW_INCLUDE_DIR} ${GLM_INCLUDE_DIR} ${PROJECT_SOURCE_DIR}/src/)

set(SOURCE_FILES
        src/engine/asset/lodepng.h
        src/engine/asset/lodepng.cpp
        src/engine/main.cpp
        src/defs.h
        src/game/Simulation.cpp
        src/game/Simulation.h
        src/engine/video/IRenderer.h
        src/engine/Game.cpp
        src/engine/Game.h
        src/engine/Factory.cpp
        src/engine/Factory.h
        src/engine/input/SDLInput.cpp
        src/engine/input/SDLInput.h
        src/engine/input/IKeyboardEventListener.h
        src/engine/input/IQuitEventListener.cpp
        src/engine/input/IQuitEventListener.h
        src/engine/IGameMode.h
        src/engine/IGame.h
        src/game/intro/IntroGameMode.cpp
        src/game/intro/IntroGameMode.h
        src/game/menu/MenuGameMode.cpp
        src/game/menu/MenuGameMode.h
        src/engine/video/raster/PixelBuffer.cpp
        src/engine/video/raster/PixelBuffer.h
        src/engine/video/texture/ITexture.h
        src/engine/asset/IAssetLoader.h
        src/engine/util/Rect2D.cpp
        src/engine/util/Rect2D.h
        src/engine/asset/FontAsset.cpp
        src/engine/asset/FontAsset.h
        src/engine/system/ISystem.h
        src/engine/system/SDLSystem.cpp
        src/engine/system/SDLSystem.h
        src/engine/util/math_util.cpp
        src/engine/util/math_util.h
        src/engine/util/Point2D.cpp
        src/engine/util/Point2D.h
        src/engine/util/string_util.cpp
        src/engine/util/string_util.h
        src/engine/video/Renderer.cpp
        src/engine/video/Renderer.h
        src/engine/video/mesh/IndexedMesh.cpp
        src/engine/video/mesh/IndexedMesh.h
        src/engine/video/mesh/Mesh.cpp
        src/engine/video/mesh/Mesh.h
        src/engine/video/shader/shaders.h
        src/engine/video/texture/OGLTexture.cpp
        src/engine/video/texture/OGLTexture.h
        src/engine/video/texture/TextureAtlas.cpp
        src/engine/video/texture/TextureAtlas.h
        src/engine/video/ui/QuadRenderer.cpp
        src/engine/video/ui/QuadRenderer.h
        src/engine/video/other/CubeRenderer.cpp
        src/engine/video/other/CubeRenderer.h
        src/engine/video/voxel/ChunkManager.cpp
        src/engine/video/voxel/ChunkManager.h
        src/engine/asset/ILoadTask.h
        src/engine/asset/AssetLoader.cpp
        src/engine/asset/AssetLoader.h
        src/engine/video/font/FontRenderer.cpp
        src/engine/video/font/FontRenderer.h
        src/engine/video/shader/Shader.cpp
        src/engine/video/shader/Shader.h
        src/engine/video/mesh/MeshUpdateType.h
        src/engine/video/camera/Camera.h
        src/engine/asset/tiny_obj_loader.cpp
        src/engine/video/other/LightSceneRenderer.cpp
        src/engine/video/other/LightSceneRenderer.h
        src/engine/input/IMouseEventListener.h
        src/engine/video/mesh/Mesh2.cpp
        src/engine/video/mesh/Mesh2.h
        src/engine/video/mesh/Mesh3.cpp
        src/engine/video/mesh/Mesh3.h
        src/engine/video/mesh/BlockMesh.cpp
        src/engine/video/mesh/BlockMesh.h
        src/engine/video/model/Model.cpp
        src/engine/video/model/Model.h
        src/engine/video/shader/Material.h
        src/engine/video/model/MaterialDictionary.cpp
        src/engine/video/model/MaterialDictionary.h
        src/engine/video/voxel/Block.cpp
        src/engine/video/voxel/Block.h
        src/engine/video/model/ModelRenderer.cpp
        src/engine/video/model/ModelRenderer.h
        src/engine/video/voxel/ChunkRenderer.cpp
        src/engine/video/voxel/ChunkRenderer.h
        src/engine/video/voxel/Chunk.cpp
        src/engine/video/voxel/Chunk.h
        src/engine/video/voxel/BlockTypeDictionary.cpp
        src/engine/video/voxel/BlockTypeDictionary.h
        src/engine/video/voxel/BiomeType.h
        src/engine/video/voxel/BiomeType.cpp
        src/engine/video/voxel/HeightMap.h
        src/engine/video/voxel/HeightMap.cpp
        src/engine/video/voxel/Terrain.h
        src/engine/video/voxel/Terrain.cpp
        src/engine/video/voxel/ChunkPos.h
        src/engine/video/frustrum/Vec3.cpp
        src/engine/video/frustrum/Vec3.h
        src/engine/video/frustrum/AABox.cpp
        src/engine/video/frustrum/AABox.h
        src/engine/video/frustrum/Plane.cpp
        src/engine/video/frustrum/Plane.h
        src/engine/video/frustrum/ViewFrustum.cpp
        src/engine/video/frustrum/ViewFrustum.h
        src/engine/video/shader/Fog.h
        src/engine/video/sky/Skybox.cpp
        src/engine/video/sky/Skybox.h
        src/engine/video/shader/DirectionalLight.h
        src/engine/video/voxel/AmbientOcclusion.h
        src/engine/video/voxel/ChunkBlockPos.h
        src/engine/video/voxel/IChunkManager.h
        src/engine/util/interpolation.h
        src/engine/video/voxel/LightMapper.cpp
        src/engine/video/voxel/LightMapper.h
        src/engine/video/voxel/ChunkMetrics.h
        src/engine/util/BlockingQueue.h
        src/engine/video/voxel/AChunkManager.cpp
        src/engine/video/voxel/AChunkManager.h
        src/engine/video/texture/OGLArrayTexture.cpp
        src/engine/video/texture/OGLArrayTexture.h
        src/engine/video/voxel/SceneryManager.cpp
        src/engine/video/voxel/SceneryManager.h)
add_executable(game ${SOURCE_FILES})

target_link_libraries(game ${GLEW_LIBRARY} ${OPENGL_LIBRARIES} ${SDL2_LIBRARY})