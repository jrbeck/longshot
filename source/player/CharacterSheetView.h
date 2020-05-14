#pragma once

#include "../game/GameModel.h"
#include "../game/GameInput.h"
#include "../game/GameMenu.h"

class CharacterSheetView {
public:
  CharacterSheetView(GameModel* gameModel);
  ~CharacterSheetView();

  void handleInput(GameInput* gameInput);

  void draw();

private:
  void update();

  GameModel* mGameModel;
  GameMenu* mCharacterSheet;
};
