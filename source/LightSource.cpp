#include "LightSource.h"



LightSource::LightSource (void) {
}



LightSource::~LightSource (void) {
}



void LightSource::setColor (v3d_t color) {
  mColor = color;
}



v3d_t LightSource::getColor (void) {
  return mColor;
}



void LightSource::setDirection (v3d_t direction) {
  mDirection = v3d_normalize (direction);
}


v3d_t LightSource::getDirection (void) {
  return mDirection;
}


v3d_t LightSource::computeDirectionalLightIntensity (v3d_t normalVector, bool isTheSun) {
  double intensityScalar = v3d_dot (mDirection, normalVector);

  if (intensityScalar < 0.0) {
    intensityScalar = 0.0;
  }

  double overheadIntensity = v3d_dot (mDirection, v3d_v (0.0, 1.0, 0.0));

  if (overheadIntensity < 0.0) {
    intensityScalar += (overheadIntensity * 4.0);
  }
  else {
    intensityScalar += overheadIntensity;
  }

//  if (intensityScalar > 0.0 && intensityScalar < 0.5) {
//    intensityScalar = 0.5;
//  }


//  intensityScalar += (overheadIntensity + 0.3) * 0.5;

//  if (intensityScalar < 0.6) {
//    intensityScalar = 0.6;
//  }

/*
  if (isTheSun) {
    intensityScalar *= 0.2;
  }
  else {
    if (intensityScalar > 0.5) {
      intensityScalar = 0.5;
    }
  }
*/

  if (intensityScalar < 0.0) {
    intensityScalar = 0.0;
  }
  if (intensityScalar > 1.0) {
    intensityScalar = 1.0;
  }

  v3d_t color = v3d_scale (intensityScalar, mColor);

  return color;
}


