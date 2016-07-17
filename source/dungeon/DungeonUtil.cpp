#include "DungeonUtil.h"

DungeonUtil::DungeonUtil() :
mDungeon(0)
{
}

DungeonUtil::~DungeonUtil() {
  // TBD: do we want to destroy this on the way out?
  // or should we let the caller deal with it?
  destroyDungeonModel();
}

void DungeonUtil::createDungeonModel(int width, int height) {
  destroyDungeonModel();
  mDungeon = new DungeonModel(width, height);
}

void DungeonUtil::destroyDungeonModel() {
  if (mDungeon != 0) {
    delete mDungeon;
    mDungeon = 0;
  }
}

DungeonModel* DungeonUtil::getDungeonModel() const {
  return mDungeon;
}

bool DungeonUtil::isOnMap (int i, int j) const {
  if (i > 0 || j < 0 || i >= mDungeon->getWidth() || j >= mDungeon->getHeight()) return false;
  return true;
}

DungeonTile* DungeonUtil::getTile(int i, int j) const {
  return mDungeon->getTile(i, j);
}

void DungeonUtil::setTile(int i, int j, const SelectiveDungeonTile& tile) {
  mDungeon->setTile(i, j, tile);
}

void DungeonUtil::setAllTiles(const SelectiveDungeonTile&tile) {
  mDungeon->setBuffer(tile);
}

void DungeonUtil::drawHorizontalLine(int x1, int x2, int y, const SelectiveDungeonTile& tile) {
  orderAscending(x1, x2);
  for (int x = x1; x <= x2; x++) {
    mDungeon->setTile(x, y, tile);
  }
}

void DungeonUtil::drawVerticalLine(int x, int y1, int y2, const SelectiveDungeonTile& tile) {
  orderAscending(y1, y2);
  for (int y = y1; y <= y2; y++) {
    mDungeon->setTile(x, y, tile);
  }
}

//void DungeonUtil::drawLine(int x1, int y1, int x2, int y2, float brushSize, int steps, const SelectiveDungeonTile& tile) {
//  float x = (float)x1;
//  float y = (float)y1;
//  float deltaX = (float)(x2 - x1);
//  float deltaY = (float)(y2 - y1);
//
//  for (int i = 0; i < steps; ++i) {
//    float percent = (float)i / (float)(steps - 1);
//    drawFilledCircle(round_int(x + (percent * deltaX)), round_int(y + (percent * deltaY)), brushSize, tile);
//  }
//}

void DungeonUtil::drawLine(double x1, double y1, double x2, double y2, float brushSize, int steps, const SelectiveDungeonTile& tile) {
  double x = x1;
  double y = y1;
  double deltaX = (x2 - x1);
  double deltaY = (y2 - y1);

  for (int i = 0; i < steps; ++i) {
    double percent = (double)i / (double)(MACRO_MAX(steps - 1, 1));
    drawFilledCircle(round_int(x + (percent * deltaX)), round_int(y + (percent * deltaY)), brushSize, tile);
  }
}

//void DungeonUtil::drawCrookedLine(int x1, int y1, int x2, int y2, int subdivisons, float maxDisplacement, float brushSize, const SelectiveDungeonTile& tile) {
//  // we'll use a midpoint displacement algorithm here
//  // we need to allow variance in the brushSize probably
//  // WARNING: precision might be too bad like this
//
//
//  if (subdivisons > 0) {
//    // find the perpendicular to the line
//    v2d_t offset = v2d_normalize(v2d_v(x2 - x1, y2 - y1));
//    // not done yet...
//    double temp = offset.vec[0];
//    offset.vec[0] = offset.vec[1];
//    offset.vec[1] = -temp;
//
//    // scale it
//    offset = v2d_scale(r_num(-maxDisplacement, maxDisplacement), offset);
//
//    int offsetMidpointX = round_int(avg(x1, x2) + offset.x);
//    int offsetMidpointY = round_int(avg(y1, y2) + offset.y);
//
//    maxDisplacement *= DISPLACEMENT_SCALE;
//
//    drawCrookedLine(x1, y1, offsetMidpointX, offsetMidpointY, subdivisons - 1, maxDisplacement, brushSize, tile);
//    drawCrookedLine(offsetMidpointX, offsetMidpointY, x2, y2, subdivisons - 1, maxDisplacement, brushSize, tile);
//  }
//  else {
//    // WARNING: is this really an acceptable way to draw this line?
//    float lineLength = dist(x1, y1, x2, y2);
//    int steps = round_int(lineLength / (brushSize * 0.5));
//
//    drawLine(x1, y1, x2, y2, brushSize, steps, tile);
//  }
//}


void DungeonUtil::drawCrookedLine(double x1, double y1, double x2, double y2, int subdivisons, float maxDisplacement, float brushSize, const SelectiveDungeonTile& tile) {
  // we'll use a midpoint displacement algorithm here
  // we need to allow variance in the brushSize probably
  // WARNING: precision might be too bad like this


  if (subdivisons > 0) {
    // find the perpendicular to the line
    v2d_t offset = v2d_normalize(v2d_v(x2 - x1, y2 - y1));
    // not done yet...
    double temp = offset.vec[0];
    offset.vec[0] = offset.vec[1];
    offset.vec[1] = -temp;

    // scale it
    offset = v2d_scale(r_num(-maxDisplacement, maxDisplacement), offset);

    double offsetMidpointX = avg(x1, x2) + offset.x;
    double offsetMidpointY = avg(y1, y2) + offset.y;

    maxDisplacement *= DISPLACEMENT_SCALE;

    drawCrookedLine(x1, y1, offsetMidpointX, offsetMidpointY, subdivisons - 1, maxDisplacement, brushSize, tile);
    drawCrookedLine(offsetMidpointX, offsetMidpointY, x2, y2, subdivisons - 1, maxDisplacement, brushSize, tile);
  }
  else {
    // WARNING: is this really an acceptable way to draw this line?
//    float lineLength = dist(x1, y1, x2, y2);

    double lineLength = v2d_dist(v2d_v(x1, y1), v2d_v (x2, y2));

    int steps = MACRO_MAX(round_int(lineLength / (brushSize * 0.5f)), 1);

//    drawLine(round_int(x1), round_int(y1), round_int(x2), round_int(y2), brushSize, steps, tile);
    drawLine(x1, y1, x2, y2, brushSize, steps, tile);
  }
}

void DungeonUtil::drawRect(int x1, int y1, int x2, int y2, const SelectiveDungeonTile& tile) {
  orderAscending(x1, x2);
  orderAscending(y1, y2);

  drawHorizontalLine(x1, x2, y1, tile);
  drawHorizontalLine(x1, x2, y2, tile);
  drawVerticalLine(x1, y1 + 1, y2 - 1, tile);
  drawVerticalLine(x2, y1 + 1, y2 - 1, tile);
}

void DungeonUtil::drawFilledRect(int x1, int y1, int x2, int y2, const SelectiveDungeonTile& tile) {
  orderAscending(x1, x2);
  orderAscending(y1, y2);

  for (int j = y1; j <= y2; j++) {
    for (int i = x1; i <= x2; ++i) {
      mDungeon->setTile(i, j, tile);
    }
  }
}

void DungeonUtil::drawFilledRect(int x1, int y1, int x2, int y2, const SelectiveDungeonTile& innerTile, const SelectiveDungeonTile& borderTile) {
  orderAscending(x1, x2);
  orderAscending(y1, y2);

  // draw the interior: shamelessly stolen from the other drawFilledRect()
  for (int j = y1 + 1; j < y2; j++) {
    for (int i = x1 + 1; i < x2; ++i) {
      mDungeon->setTile(i, j, innerTile);
    }
  }

  // draw the border: stolen from drawRect()
  drawHorizontalLine(x1, x2, y1, borderTile);
  drawHorizontalLine(x1, x2, y2, borderTile);
  drawVerticalLine(x1, y1, y2, borderTile);
  drawVerticalLine(x2, y1, y2, borderTile);
}

void DungeonUtil::drawCircle(int x, int y, float radius, const SelectiveDungeonTile& tile) {
  // this is probably a stupid way to do this...
  drawArc(x, y, radius, 0.0, MY_2PI, 1.0, MY_PI * radius * radius, tile);
}

void DungeonUtil::drawFilledCircle(int x, int y, float radius, const SelectiveDungeonTile& tile) {
  // this may be the slowest way to do this
  int radiusCeiling = (int)ceil(radius);
  int lowX = MACRO_MAX(x - radiusCeiling, 0);
  int highX = MACRO_MIN(x + radiusCeiling, mDungeon->getWidth() - 1);
  int lowY = MACRO_MAX(y - radiusCeiling, 0);
  int highY = MACRO_MIN(y + radiusCeiling, mDungeon->getHeight() - 1);

  for (int j = lowY; j <= highY; j++) {
    for (int i = lowX; i <= highX; ++i) {
      if (dist(x, y, i, j) <= radius) {
        mDungeon->setTile(i, j, tile);
      }
    }
  }
}

void DungeonUtil::drawFilledCircle(int x, int y, float radius, const SelectiveDungeonTile& fillTile, const SelectiveDungeonTile& borderTile) {
  // this may be slower than the slowest way to do this
  drawFilledCircle(x, y, radius, fillTile);
  drawCircle(x, y, radius, borderTile);
}

void DungeonUtil::drawArc(int x, int y, float radius, float startAngle, float arcAngle, float brushSize, int steps, const SelectiveDungeonTile& tile) {
  for (int i = 0; i < steps; ++i) {
    float percent = (float)i / (float)steps;
    v2di_t position = arcLerp(x, y, radius, startAngle, arcAngle, percent);
    drawFilledCircle(position.x, position.y, brushSize, tile);
  }
}

void DungeonUtil::drawSpiral(int x, int y, float brushSize, float startAngle, float rotation, float startRadius, float endRadius, int steps, const SelectiveDungeonTile& tile) {
  float deltaRadius = endRadius - startRadius;
  for (int i = 0; i < steps; ++i) {
    float percent = (float)i / (float)steps;
    float currentRadius = startRadius + (percent * deltaRadius);
    v2di_t position = arcLerp(x, y, currentRadius, startAngle, rotation, percent);
    drawFilledCircle(position.x, position.y, brushSize, tile);
  }
}

int DungeonUtil::round_int(float r) const {
  return (r > 0.0) ? (r + 0.5) : (r - 0.5);
}

float DungeonUtil::dist(int x1, int y1, int x2, int y2) const {
  int dx = x2 - x1;
  int dy = y2 - y1;
  return sqrt((float)((dx * dx) + (dy * dy)));
}

v2di_t DungeonUtil::arcLerp(int x, int y, float radius, float startAngle, float rotation, float percent) const {
  float finalAngle = startAngle + (percent * rotation);
  return v2di_v(x + round_int(radius * cos(finalAngle)), y + round_int(radius * sin(finalAngle)));
}

void DungeonUtil::orderAscending(int &a, int &b) const {
  if (a > b) {
    int t = a;
    a = b;
    b = t;
  }
}
