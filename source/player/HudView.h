#pragma once

#include "../game/GameModel.h"
#include "../game/GameMenu.h"

class HudView {
public:
  HudView(GameModel* gameModel);
  ~HudView();

  void draw();

  void update();
  void showDeadPlayerHud();
  void showLivePlayerHud();

private:
  GameModel* mGameModel;

  GameMenu* mHud;
};
