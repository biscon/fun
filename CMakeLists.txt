cmake_minimum_required(VERSION 3.8)
project(game)

set(CMAKE_CXX_STANDARD 11)

#set(CMAKE_CXX_FLAGS_DEBUG "-pthread")
# uncomment this to compile as gui app (doesnt open the console)
#set(CMAKE_CXX_FLAGS_DEBUG "-mwindows")

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
        src/engine/video/lodepng.h
        src/engine/video/lodepng.cpp
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
        src/engine/video/PixelBuffer.cpp
        src/engine/video/PixelBuffer.h
        src/engine/video/ITexture.h
        src/engine/asset/IAssetLoader.h
        src/engine/util/Rect2D.cpp
        src/engine/util/Rect2D.h
        src/engine/asset/FontAsset.cpp
        src/engine/asset/FontAsset.h
        src/engine/ISystem.h
        src/engine/SDLSystem.cpp
        src/engine/SDLSystem.h
        src/engine/FPSCounter.cpp
        src/engine/FPSCounter.h
        src/engine/util/math_util.cpp
        src/engine/util/math_util.h
        src/engine/util/Point2D.cpp
        src/engine/util/Point2D.h
        src/engine/util/string_util.cpp
        src/engine/util/string_util.h
        src/engine/video/OGLRenderer.cpp
        src/engine/video/OGLRenderer.h
        src/engine/video/IndexedMesh.cpp
        src/engine/video/IndexedMesh.h
        src/engine/video/Mesh.cpp
        src/engine/video/Mesh.h
        src/engine/video/shaders.h
        src/engine/video/OGLTexture.cpp
        src/engine/video/OGLTexture.h
        src/engine/video/TextureAtlas.cpp
        src/engine/video/TextureAtlas.h
        src/engine/video/QuadRenderer.cpp
        src/engine/video/QuadRenderer.h
        src/engine/video/CubeRenderer.cpp
        src/engine/video/CubeRenderer.h
        src/engine/video/TileMap.cpp
        src/engine/video/TileMap.h
        src/engine/asset/ILoadTask.h
        src/engine/asset/AssetLoader.cpp
        src/engine/asset/AssetLoader.h
        src/engine/video/FontRenderer.cpp
        src/engine/video/FontRenderer.h
        src/engine/video/Shader.cpp
        src/engine/video/Shader.h
        src/engine/video/MeshUpdateType.h
        src/engine/video/Camera.h
        src/engine/asset/tiny_obj_loader.cpp
        src/engine/video/LightSceneRenderer.cpp
        src/engine/video/LightSceneRenderer.h
        src/engine/input/IMouseEventListener.h
        src/engine/video/Mesh2.cpp
        src/engine/video/Mesh2.h
        src/engine/video/Mesh3.cpp
        src/engine/video/Mesh3.h
        src/engine/video/Model.cpp
        src/engine/video/Model.h
        src/engine/video/Material.h
        src/engine/video/MaterialDictionary.cpp
        src/engine/video/MaterialDictionary.h
        src/engine/voxel/Block.cpp
        src/engine/voxel/Block.h
        src/engine/voxel/Chunk.cpp
        src/engine/voxel/Chunk.h
        src/engine/video/ModelRenderer.cpp
        src/engine/video/ModelRenderer.h
        src/engine/video/tiles/TileRenderer.cpp
        src/engine/video/tiles/TileRenderer.h
        src/engine/video/tiles/TileChunk.cpp
        src/engine/video/tiles/TileChunk.h src/engine/video/tiles/TileTypeDictionary.cpp src/engine/video/tiles/TileTypeDictionary.h)
add_executable(game ${SOURCE_FILES})

target_link_libraries(game ${GLEW_LIBRARY} ${OPENGL_LIBRARIES} ${SDL2_LIBRARY})