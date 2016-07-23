// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
// * LightSource
// *
// * this maintains information about a light souce and allows the calculation
// * of lighting intensity given a surface vector, etc...
// *
// *
// *
// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *

#pragma once

#include "../math/v3d.h"


class LightSource {
public:
  LightSource();
  ~LightSource();

  void setColor(v3d_t color);
  v3d_t getColor();

  void setDirection(v3d_t direction);
  v3d_t getDirection();

  v3d_t computeDirectionalLightIntensity(v3d_t normalVector, bool isTheSun);

private:
  v3d_t mColor;
  v3d_t mDirection;
};
