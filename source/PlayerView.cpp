#include "PlayerView.h"













void PlayerView::drawPlayerTargetBlock() {
  int targetFace;

  v3di_t* playerTarg = mPlayer->getTargetBlock(targetFace);
  if (playerTarg == NULL) {
    return;
  }

  v3d_t nearCorner = v3d_v(*playerTarg);
  v3d_t farCorner = v3d_add(nearCorner, v3d_v(1.01, 1.01, 1.01));
  nearCorner = v3d_add(nearCorner, v3d_v(-0.01, -0.01, -0.01));

  /*
  v3d_t nearCorner2 = v3d_v (*playerTarg);
  GLfloat faceColor[4] = { 0.5, 0.0, 0.0, 0.5 };

  // draw the targeted face

  glPushMatrix ();
  glTranslated (nearCorner2.x, nearCorner2.y, nearCorner2.z);

  glBegin (GL_QUADS);
  mAssetManager.drawBlockFace (targetFace, faceColor);
  glEnd ();
  glPopMatrix ();
  */

  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, 0);

  glColor4f(0.3f, 0.3f, 0.3f, 1.0f);

  glBegin(GL_LINES);
  glVertex3d(nearCorner.x, nearCorner.y, nearCorner.z);
  glVertex3d(farCorner.x, nearCorner.y, nearCorner.z);

  glVertex3d(nearCorner.x, nearCorner.y, nearCorner.z);
  glVertex3d(nearCorner.x, farCorner.y, nearCorner.z);

  glVertex3d(nearCorner.x, nearCorner.y, nearCorner.z);
  glVertex3d(nearCorner.x, nearCorner.y, farCorner.z);

  glVertex3d(farCorner.x, farCorner.y, farCorner.z);
  glVertex3d(nearCorner.x, farCorner.y, farCorner.z);

  glVertex3d(farCorner.x, farCorner.y, farCorner.z);
  glVertex3d(farCorner.x, nearCorner.y, farCorner.z);

  glVertex3d(farCorner.x, farCorner.y, farCorner.z);
  glVertex3d(farCorner.x, farCorner.y, nearCorner.z);

  glVertex3d(farCorner.x, nearCorner.y, nearCorner.z);
  glVertex3d(farCorner.x, nearCorner.y, farCorner.z);

  glVertex3d(farCorner.x, nearCorner.y, nearCorner.z);
  glVertex3d(farCorner.x, farCorner.y, nearCorner.z);

  glVertex3d(nearCorner.x, farCorner.y, nearCorner.z);
  glVertex3d(farCorner.x, farCorner.y, nearCorner.z);

  glVertex3d(nearCorner.x, farCorner.y, nearCorner.z);
  glVertex3d(nearCorner.x, farCorner.y, farCorner.z);

  glVertex3d(nearCorner.x, nearCorner.y, farCorner.z);
  glVertex3d(nearCorner.x, farCorner.y, farCorner.z);

  glVertex3d(nearCorner.x, nearCorner.y, farCorner.z);
  glVertex3d(farCorner.x, nearCorner.y, farCorner.z);
  glEnd();
}





