#pragma once

#include "../game/GameModel.h"
#include "../game/GameInput.h"
#include "../game/GameMenu.h"

class CharacterSheetView {
public:
  CharacterSheetView(GameModel* gameModel);
  ~CharacterSheetView();

  void draw();

private:
  void update();
  void handleInput(GameInput* gameInput);

  GameModel* mGameModel;

  GameMenu* mCharacterSheet;
};
