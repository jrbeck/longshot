#pragma once

class HudView {
public:
  HudView();
  ~HudView();

  void draw();

  void updateHud();
  void showDeadPlayerHud();
  void showLivePlayerHud();

private:
  GameMenu* mHud;
};
