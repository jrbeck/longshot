#pragma once

class CharacterSheetView {
public:
  CharacterSheetView();
  ~CharacterSheetView();

  void draw();
  void update();
  void handleInput(GameInput* gameInput);

private:
  GameMenu* mCharacterSheet;
};
