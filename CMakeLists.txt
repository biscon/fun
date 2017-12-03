cmake_minimum_required(VERSION 3.8)
project(game)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_CXX_FLAGS_DEBUG "-pthread")
# uncomment this to compile as gui app (doesnt open the console)
#set(CMAKE_CXX_FLAGS_DEBUG "-mwindows")

# includes cmake/FindSDL2.cmake
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

set(GLM_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/src/glm)

# glew
set(GLEW_INCLUDE_DIR "E:/glew-2.1.0-win32/glew-2.1.0/include")
set(GLEW_LIBRARY "E:/glew-2.1.0-win32/glew-2.1.0/lib/Release/x64/glew32s.lib")


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "../")

find_package(GLEW REQUIRED)
find_package(OPENGL REQUIRED)
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIR} ${GLEW_INCLUDE_DIR} ${GLM_INCLUDE_DIR})

set(SOURCE_FILES src/system/video/lodepng.h src/system/video/lodepng.cpp src/system/main.cpp src/defs.h src/game/Simulation.cpp src/game/Simulation.h src/game/video/IRenderer.h src/game/Game.cpp src/game/Game.h src/system/Factory.cpp src/system/Factory.h src/system/input/SDLInput.cpp src/system/input/SDLInput.h src/game/input/IKeyboardEventListener.h src/game/input/IQuitEventListener.cpp src/game/input/IQuitEventListener.h src/game/IGameMode.h src/game/IGame.h src/game/intro/IntroGameMode.cpp src/game/intro/IntroGameMode.h src/game/menu/MenuGameMode.cpp src/game/menu/MenuGameMode.h src/game/video/PixelBuffer.cpp src/game/video/PixelBuffer.h src/game/video/ITexture.h src/game/asset/IAsset.h src/game/asset/IAssetLoader.h src/system/asset/AssetLoader.cpp src/system/asset/AssetLoader.h src/game/asset/TextureAsset.cpp src/game/asset/TextureAsset.h src/game/util/Rect2D.cpp src/game/util/Rect2D.h src/game/asset/FontAsset.cpp src/game/asset/FontAsset.h src/game/ISystem.h src/system/SDLSystem.cpp src/system/SDLSystem.h src/game/FPSCounter.cpp src/game/FPSCounter.h src/game/util/math_util.cpp src/game/util/math_util.h src/game/util/Point2D.cpp src/game/util/Point2D.h src/game/util/string_util.cpp src/game/util/string_util.h src/system/video/OGLRenderer.cpp src/system/video/OGLRenderer.h src/system/video/Shader.h src/system/video/Shader.cpp src/system/video/RenderMesh.cpp src/system/video/RenderMesh.h src/system/video/shaders.h src/system/video/OGLTexture.cpp src/system/video/OGLTexture.h src/game/video/TextureAtlas.cpp src/game/video/TextureAtlas.h src/system/video/QuadRenderer.cpp src/system/video/QuadRenderer.h src/game/video/Texture.cpp src/game/video/Texture.h src/system/video/TextureManager.cpp src/system/video/TextureManager.h src/game/video/TileMap.cpp src/game/video/TileMap.h src/game/asset/ILoadTask.h src/system/asset/AssetLoader2.cpp src/system/asset/AssetLoader2.h)
add_executable(game ${SOURCE_FILES})

target_link_libraries(game ${GLEW_LIBRARY} ${OPENGL_LIBRARIES} ${SDL2_LIBRARY})