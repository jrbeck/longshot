cmake_minimum_required(VERSION 3.9)
project(Longshot)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lSDL2 -lSDL2_image -lSDL2_mixer -lGLEW -lGL -lGLU -lpthread -ansi -Wall -Werror -Wextra -pedantic -pedantic-errors -O3 -g")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w -Isource -Isource/include -lSDL2 -lSDL2_image -lSDL2_mixer -lGLEW -lGL -lGLU")

# ls source/main.cpp >> files.txt && \
# ls source/ai/*.cpp >> files.txt && \
# ls source/assets/*.cpp >> files.txt && \
# ls source/dungeon/*.cpp >> files.txt && \
# ls source/game/*.cpp >> files.txt && \
# ls source/items/*.cpp >> files.txt && \
# ls source/math/*.cpp >> files.txt && \
# ls source/physics/*.cpp >> files.txt && \
# ls source/player/*.cpp >> files.txt && \
# ls source/world/*.cpp >> files.txt

set(LONGSHOT_SOURCE_FILES
  source/main.cpp
  source/ai/AiEntity.cpp
  source/ai/AiManager.cpp
  source/ai/AiSpecies.cpp
  source/ai/AiStateMachine.cpp
  source/ai/AiView.cpp
  source/assets/Animation.cpp
  source/assets/AssetManager.cpp
  source/assets/BitmapModel.cpp
  source/assets/GlBitmapFont.cpp
  source/assets/ObjectLoader.cpp
  source/assets/SoundSystem.cpp
  source/assets/SpriteBatcher.cpp
  source/assets/Texture.cpp
  source/assets/TextureRegion.cpp
  source/assets/XmlParser.cpp
  source/dungeon/BigRoomDungeon.cpp
  source/dungeon/DungeonFeature.cpp
  source/dungeon/DungeonModel.cpp
  source/dungeon/DungeonTile.cpp
  source/dungeon/DungeonUtil.cpp
  source/dungeon/FeatureGenerator.cpp
  source/dungeon/FeatureUtil.cpp
  source/dungeon/MoleculeDungeon.cpp
  source/dungeon/RogueMap.cpp
  source/dungeon/RogueMapViewer.cpp
  source/dungeon/SnakeDungeon.cpp
  source/game/Constants.cpp
  source/game/FileSystem.cpp
  source/game/Game.cpp
  source/game/GameInput.cpp
  source/game/GameMenu.cpp
  source/game/GameModel.cpp
  source/game/GameView.cpp
  source/game/GameWindow.cpp
  source/game/LoadScreen.cpp
  source/game/Longshot.cpp
  source/game/MessageBus.cpp
  source/items/Inventory.cpp
  source/items/ItemContainer.cpp
  source/items/ItemManager.cpp
  source/items/MerchantView.cpp
  source/math/BoundingBox.cpp
  source/math/BoundingSphere.cpp
  source/math/GlCamera.cpp
  source/math/MathUtil.cpp
  source/math/Periodics.cpp
  source/math/Plane3d.cpp
  source/math/PseudoRandom.cpp
  source/math/Rectangle2d.cpp
  source/math/RtsCam.cpp
  source/math/Simplex.cpp
  source/math/v2d.cpp
  source/math/v3d.cpp
  source/physics/InactiveList.cpp
  source/physics/Physics.cpp
  source/physics/PhysicsView.cpp
  source/player/CharacterSheetView.cpp
  source/player/HeadBobble.cpp
  source/player/HudView.cpp
  source/player/PlayerController.cpp
  source/player/Player.cpp
  source/player/PlayerView.cpp
  source/world/BiomeMap.cpp
  source/world/BlockTypeData.cpp
  source/world/CloudSim.cpp
  source/world/Galaxy.cpp
  source/world/GalaxyMap.cpp
  source/world/InactiveColumnManager.cpp
  source/world/LightManager.cpp
  source/world/LightSource.cpp
  source/world/Location.cpp
  source/world/OrbitSky.cpp
  source/world/OverdrawManager.cpp
  source/world/Planet.cpp
  source/world/PlanetMap.cpp
  source/world/RogueViewer.cpp
  source/world/SkySim.cpp
  source/world/StarShip.cpp
  source/world/StarSystem.cpp
  source/world/Terrain.cpp
  source/world/WorldChunk.cpp
  source/world/WorldColumn.cpp
  source/world/World.cpp
  source/world/WorldLight.cpp
  source/world/WorldLighting.cpp
  source/world/WorldMap.cpp
  source/world/WorldMapView.cpp
  source/world/WorldUtil.cpp
)

# g++ -w -Isource -Isource/include -lSDL2 -lSDL2_image -lSDL2_mixer -lGLEW -lGL -lGLU \
#   source/main.cpp \
#   source/ai/*.cpp \
#   source/assets/*.cpp \
#   source/dungeon/*.cpp \
#   source/game/*.cpp \
#   source/items/*.cpp \
#   source/math/*.cpp \
#   source/physics/*.cpp \
#   source/player/*.cpp \
#   source/world/*.cpp \
#   -o Release/longshot


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Release)

add_executable(longshot ${LONGSHOT_SOURCE_FILES})
