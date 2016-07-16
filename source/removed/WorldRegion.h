#ifndef _WorldRegion_h
#define _WorldRegion_h

#include <math.h>
#include <malloc.h>

#include <gl_camera.h>

#include "v3d.h"
#include "sdl_util.h"

#include "terrain.h"
#include "Noise3d.h"
#include "BoundingBox.h"
#include "BoundingSphere.h"
#include "AssetManager.h"


// for the default constructor
#define DEFAULT_SIDE_LENGTH    (32)


// the eight octants (3d cardinal directions)
// we'll use up, down, north, south, east, west
#define OCT_DIR_LSW      (0)
#define OCT_DIR_LNW      (1)
#define OCT_DIR_LNE      (2)
#define OCT_DIR_LSE      (3)
#define OCT_DIR_USW      (4)
#define OCT_DIR_UNW      (5)
#define OCT_DIR_UNE      (6)
#define OCT_DIR_USE      (7)



// TYPEDEFS * * * * * * * * * * * * * * * * * * * * * * * * * * *

struct octree_node {
  int side_length;        // the length of the volume side
  v3di_t relativePosition;    // the node's relative position
  v3di_t worldPosition;      // the node's world position

  BoundingBox boundingBox;
  BoundingSphere boundingSphere;

  block_t *block;          // block data

  int num_children;        // how many children this node has
  struct octree_node *node[8];  // these are the child nodes
};



// WorldRegion * * * * * * * * * * * * * * * * * * * * * * * * * * * *
class WorldRegion {
public:

  WorldRegion (void);
  WorldRegion (int side);
  ~WorldRegion (void);

  int resize(int side);

  void setWorldPosition (v3di_t worldPosition);

  int clear (void);

  void clearBlockAtWorldPosition (v3di_t position);

  int setBlockAtWorldPosition (v3di_t position, block_t block);

  bool isInRegion (v3di_t pos);

  block_t *getBlockAtWorldPosition (v3di_t pos);
  bool isSolidBlockAtWorldPosition (v3di_t pos);

  int draw (gl_camera_c &cam, AssetManager &assetManager);

  int generateFromTerrain (int *terrain, v3di_t worldPosition, terrain_c &randomMap);

  void generateFromNoise (v3di_t worldPosition, Noise3d &noise);

  int get_num_blocks (void);
  int get_num_nodes (void);

  int generateDisplayList (AssetManager &assetManager);

// public members
  v3di_t mWorldIndex;

  bool mIsNeeded;

private:
  v3di_t mWorldPosition;

  void clear_block (v3di_t pos);
  int clear_rec (octree_node *n);

  int set (v3di_t pos, block_t b);
  int set_rec (octree_node *n, v3di_t pos, block_t b);

  int create_node (octree_node *parent, int octant);
  block_t *create_block (block_t b);

  v3di_t getRelativeNodePosition (v3di_t lsw, int side_length, int octant);

  int pick_octant (v3di_t lsw, v3di_t pos, int side_length);

  void clear_block_rec (octree_node *n, v3di_t pos);

  block_t *block_at (v3di_t pos);

  void drawForDisplayList (AssetManager &assetManager);
  void drawForDisplayList_rec (octree_node *n, AssetManager &assetManager);

  int draw_rec (octree_node *n, gl_camera_c &cam, AssetManager &assetManager);
  int draw_rec (octree_node *n, AssetManager &assetManager);

// members * * * * * * * * * * * * * * * * * * * * * * * * * *
  struct octree_node root;

  int num_nodes;
  int num_blocks;

  GLuint mDisplayListHandle;

};


// METHODS * * * * * * * * * * * * * * * * * * * * * * * * * * * *


// default constructor
WorldRegion::WorldRegion (void) {
  // since this is the default constructor
  root.side_length = DEFAULT_SIDE_LENGTH;

  // set the relative position of the root to the origin
  root.relativePosition = v3di_v (0, 0, 0);

  // put it at (0, 0, 0) in the world
  mWorldPosition = root.worldPosition = v3di_v (0, 0, 0);

  // this hasn't been set yet
  root.block = NULL;

  // no children yet
  root.num_children = 0;

  // set up the bounding box
  double side = DEFAULT_SIDE_LENGTH;

  v3d_t dimensions = v3d_v (side, side, side);

  root.boundingBox.setDimensions (dimensions);

  // set up the bounding sphere
  double half_side = side * 0.5;

  root.boundingSphere.setPosition (v3d_v (half_side, half_side, half_side));
  root.boundingSphere.setRadius (1.7322 * half_side);

  // reset counters
  num_nodes = 0;
  num_blocks = 0;

  // set the children to NULL
  for (int i = 0; i < 8; i++) {
    root.node[i] = NULL; }

  // we don't have anything to draw yet
  mDisplayListHandle = -1;
}


// constructor
WorldRegion::WorldRegion (int side) {
  // since this is the default constructor
  root.side_length = side;

  // set the relative position of the root to the origin
  root.relativePosition = v3di_v (0, 0, 0);

  // put it at (0, 0, 0) in the world
  mWorldPosition = root.worldPosition = v3di_v (0, 0, 0);

  // this hasn't been set yet
  root.block = NULL;

  // no children yet
  root.num_children = 0;

  // set up the bounding box
  v3d_t dimensions = v3d_v (side, side, side);

  root.boundingBox.setDimensions (dimensions);

  // set up the bounding sphere
  double half_side = side * 0.5;

  root.boundingSphere.setPosition (v3d_v (half_side, half_side, half_side));
  root.boundingSphere.setRadius (1.7322 * half_side);

  // reset counters
  num_nodes = 0;
  num_blocks = 0;

  // set the children to NULL
  for (int i = 0; i < 8; i++) {
    root.node[i] = NULL; }

  // we don't have anything to draw yet
  mDisplayListHandle = -1;
}


// destructor
WorldRegion::~WorldRegion (void) {
  // free up the nodes
  clear ();
}



// clear and resize the array
int WorldRegion::resize(int side) {
  clear ();

  root.side_length = side;

  // set up the bounding box
  v3d_t dimensions = v3d_v (side, side, side);

  root.boundingBox.setDimensions (dimensions);

  // set up the bounding sphere
  double halfSide = static_cast<double>(side) * 0.5;
  v3d_t worldPos = v3d_v (root.worldPosition);

  v3d_t boundingSpherePosition = v3d_add (v3d_v (halfSide, halfSide, halfSide),
                      worldPos);
  root.boundingSphere.setPosition (boundingSpherePosition);
  root.boundingSphere.setRadius (1.7322 * halfSide);

  /*
  // reset counters
  num_nodes = 0;
  num_blocks = 0;

  // set the children to NULL
  for (int i = 0; i < 8; i++) {
    root.node[i] = NULL; }
*/

  return 0;
}


void WorldRegion::setWorldPosition (v3di_t worldPosition) {
  // FIXME: this will not work if a tree has children
  // if you want it to work, all the children must be translated accordingly
  // i.e. write the function you lazy punk
  clear ();

  // set the position
  mWorldPosition = worldPosition;
  root.worldPosition = worldPosition;

  // set the new bounding sphere
  double halfSide = static_cast<double>(root.side_length) * 0.5;
  v3d_t worldPos = v3d_v (root.worldPosition);

  v3d_t boundingSpherePosition = v3d_add (v3d_v (halfSide, halfSide, halfSide),
                      worldPos);
  root.boundingSphere.setPosition (boundingSpherePosition);
  root.boundingSphere.setRadius (1.7322 * halfSide);
}


// free the tree, return total number of blocks (leaves) freed
int WorldRegion::clear (void) {
  int num = clear_rec (&root);

  if (mDisplayListHandle != -1) {
    glDeleteLists (mDisplayListHandle, 1);
    mDisplayListHandle = -1;
  }

  // return the total number of blocks freed
  return num;
}


bool WorldRegion::isInRegion (v3di_t pos) {
  if (pos.x < root.worldPosition.x ||
    pos.x >= root.worldPosition.x + root.side_length ||
    pos.y < root.worldPosition.y ||
    pos.y >= root.worldPosition.y + root.side_length ||
    pos.z < root.worldPosition.z ||
    pos.z >= root.worldPosition.z + root.side_length) return false;

  return true;
}



// the recursive part of clear ()
int WorldRegion::clear_rec (octree_node *n) {
  int num = 0; // keep track of the number of freed blocks

  if (n->side_length == 1) { // leaf
    // free the block if it has been allocated
    if (n->block != NULL) {
      free (n->block);

      // update the counter for the tree
      num_blocks--;

      // freed a block! bottom of recursion
      return 1; } }

  // internal node
  if (n->num_children > 0) {
    for (int i = 0; i < 8 && n->num_children > 0; i++) {
      // free the child node if it has been allocated
      if (n->node[i] != NULL) {
        // got to see about all the descendents
        num += clear_rec (n->node[i]);

        // now free the current child
        free (n->node[i]);
        n->node[i] = NULL;

        // update the node's child count
        n->num_children--;

        // update the tree's counter
        num_nodes--; } } }

  // return the number of blocks freed by this node's descendents
  return num;
}



int WorldRegion::setBlockAtWorldPosition (v3di_t position, block_t block) {
  return set (v3di_sub (position, root.worldPosition), block);
}



// set the block at relative pos
// returns 0 on success, -1 on failure
int WorldRegion::set (v3di_t pos, block_t b) {
  // check to see if the pos is in range, return -1 if out of range
  if (pos.x < 0 || pos.x >= root.side_length ||
    pos.y < 0 || pos.y >= root.side_length ||
    pos.z < 0 || pos.z >= root.side_length) return -1;


  // we need to pick the right child
  int child = pick_octant (root.relativePosition, pos, root.side_length);

  // now call the set_rec to recursively set the leaf
  if (root.node[child] == NULL) {
    // bail if the node is not alloc'ed
    if (create_node (&root, child) == -1) {
      return -1; } }

  // start digging through the tree
  return set_rec (root.node[child], pos, b);
}


// the recursive paert of set (...)
int WorldRegion::set_rec (octree_node *n, v3di_t pos, block_t b) {
  // if we're at a leaf, add a new block or replace the one that's there
  if (n->side_length == 1) {
    // block has already been added, so just replace it
    if (n->block != NULL) {
      // let's back up the old visibility data first
      int old_numFacesVisible = n->block->numFacesVisible;
      bool old_faceVisible[6];

      memcpy (old_faceVisible, n->block->faceVisible, sizeof (old_faceVisible));

      // set the new block
      *(n->block) = b;

      // copy old visibility info
      n->block->numFacesVisible = old_numFacesVisible;
      memcpy (n->block->faceVisible, old_faceVisible, sizeof (old_faceVisible));
    }
    else {
      // create a new block
      n->block = create_block (b);

      // failed to allocate a new block
      if (n->block == NULL) return -1;

      // FIXME: set face visibility
      v3di_t n_pos;    // neighbor pos
      block_t *neighbor;  // keep track of our neighbor

      n->block->numFacesVisible = 6;

      // BLOCK_SIDE_LEF
      n_pos.x = pos.x - 1;
      n_pos.y = pos.y;
      n_pos.z = pos.z;

      neighbor = block_at (n_pos);

      if (neighbor != NULL) {
        n->block->faceVisible[BLOCK_SIDE_LEF] = false;
        n->block->numFacesVisible--;

        if (neighbor->faceVisible[BLOCK_SIDE_RIG]) {
          neighbor->faceVisible[BLOCK_SIDE_RIG] = false;
          neighbor->numFacesVisible--;
        }
      }
      else {
        n->block->faceVisible[BLOCK_SIDE_LEF] = true;
      }

      // BLOCK_SIDE_RIG
      n_pos.x = pos.x + 1;
      n_pos.y = pos.y;
      n_pos.z = pos.z;

      neighbor = block_at (n_pos);

      if (neighbor != NULL) {
        n->block->faceVisible[BLOCK_SIDE_RIG] = false;
        n->block->numFacesVisible--;

        if (neighbor->faceVisible[BLOCK_SIDE_LEF]) {
          neighbor->faceVisible[BLOCK_SIDE_LEF] = false;
          neighbor->numFacesVisible--;
        }
      }
      else {
        n->block->faceVisible[BLOCK_SIDE_RIG] = true;
      }

      // BLOCK_SIDE_BOT
      n_pos.x = pos.x;
      n_pos.y = pos.y - 1;
      n_pos.z = pos.z;

      neighbor = block_at (n_pos);

      if (neighbor != NULL) {
        n->block->faceVisible[BLOCK_SIDE_BOT] = false;
        n->block->numFacesVisible--;

        if (neighbor->faceVisible[BLOCK_SIDE_TOP]) {
          neighbor->faceVisible[BLOCK_SIDE_TOP] = false;
          neighbor->numFacesVisible--;
        }

      }
      else {
        n->block->faceVisible[BLOCK_SIDE_BOT] = true;
      }



      // BLOCK_SIDE_TOP
      n_pos.x = pos.x;
      n_pos.y = pos.y + 1;
      n_pos.z = pos.z;

      neighbor = block_at (n_pos);

      if (neighbor != NULL) {
        n->block->faceVisible[BLOCK_SIDE_TOP] = false;
        n->block->numFacesVisible--;

        if (neighbor->faceVisible[BLOCK_SIDE_BOT]) {
          neighbor->faceVisible[BLOCK_SIDE_BOT] = false;
          neighbor->numFacesVisible--;
        }

      }
      else {
        n->block->faceVisible[BLOCK_SIDE_TOP] = true;
      }

      // BLOCK_SIDE_FRO
      n_pos.x = pos.x;
      n_pos.y = pos.y;
      n_pos.z = pos.z + 1;

      neighbor = block_at (n_pos);

      if (neighbor != NULL) {
        n->block->faceVisible[BLOCK_SIDE_FRO] = false;
        n->block->numFacesVisible--;

        if (neighbor->faceVisible[BLOCK_SIDE_BAC]) {
          neighbor->faceVisible[BLOCK_SIDE_BAC] = false;
          neighbor->numFacesVisible--;
        }

      }
      else {
        n->block->faceVisible[BLOCK_SIDE_FRO] = true;
      }

      // BLOCK_SIDE_BAC
      n_pos.x = pos.x;
      n_pos.y = pos.y;
      n_pos.z = pos.z - 1;

      neighbor = block_at (n_pos);

      if (neighbor != NULL) {
        n->block->faceVisible[BLOCK_SIDE_BAC] = false;
        n->block->numFacesVisible--;

        if (neighbor->faceVisible[BLOCK_SIDE_FRO]) {
          neighbor->faceVisible[BLOCK_SIDE_FRO] = false;
          neighbor->numFacesVisible--;
        }

      }
      else {
        n->block->faceVisible[BLOCK_SIDE_BAC] = true;
      }
    }

    // successfully set the block!
    return 0;
  }

  // pick the child we need
  int child = pick_octant (n->relativePosition, pos, n->side_length);

  // see if we need a new node
  if (n->node[child] == NULL) {
    // bail if we fail to create the new node
    if (create_node (n, child) == -1) {
      return -1; } }

  // keep digging
  return set_rec (n->node[child], pos, b);
}




// create a new child node inheriting values from parent
// returns 0 on success, -1 on failure
int WorldRegion::create_node (octree_node *parent, int child) {
  // try to allocate the memory for the new node
  parent->node[child] = (octree_node *)malloc (sizeof (octree_node));

  // return NULL if malloc failed
  if (parent->node[child] == NULL) return -1;

  // increment the node counter for the entire tree
  num_nodes++;

  // increment the child count for the parent
  parent->num_children++;

  // set the initial values for the new node
  v3di_t relativePosition = getRelativeNodePosition (parent->relativePosition,
                  parent->side_length, child);

  v3di_t worldPosition =
    v3di_v (relativePosition.x + root.worldPosition.x,
        relativePosition.y + root.worldPosition.y,
        relativePosition.z + root.worldPosition.z);

  int len = parent->side_length >> 1;

  parent->node[child]->relativePosition = relativePosition;
  parent->node[child]->worldPosition = worldPosition;
  parent->node[child]->side_length = len;
  parent->node[child]->block = NULL;
  parent->node[child]->num_children = 0;

  // set up the bounding box
  double side = static_cast<double>(len);

  v3d_t dimensions = v3d_v (side, side, side);

  parent->node[child]->boundingBox.setDimensions (dimensions);
  parent->node[child]->boundingBox.setPosition (v3d_v (worldPosition));

  // set up the bounding sphere
  double halfSide = static_cast<double>(len) * 0.5;
  int halfSideI = len >> 1;

  v3d_t relativeCenterPosition = v3d_v (halfSide, halfSide, halfSide);

  parent->node[child]->boundingSphere.setPosition (v3d_add (v3d_v (worldPosition), relativeCenterPosition));
  parent->node[child]->boundingSphere.setRadius (1.7322 * halfSide);

  // set all the child nodes to NULL
  for (int i = 0; i < 8; i++) {
    parent->node[child]->node[i] = NULL; }

  // alert success
  return 0;
}


// allocate and set a new block_t
block_t *WorldRegion::create_block (block_t b) {
  block_t *ret;

  // allocate the memory for the new block
  ret = (block_t *)malloc (sizeof (block_t));

  // failed to allocate the memory
  if (ret == NULL) return NULL;

  // set the values for the new block
  *ret = b;

//  ret->visible = 0;

  // increment the (WorldRegion) counter since we created a new block
  num_blocks++;

  return ret;
}


// figure out the pos of a child node in a particular octant
v3di_t WorldRegion::getRelativeNodePosition (v3di_t lsw, int side_length, int octant) {
  switch (octant) {
    default: // sure hope this never happens!
    case OCT_DIR_LSW:
      return lsw;

    case OCT_DIR_LNW:
      lsw.z += side_length >> 1;
      return lsw;

    case OCT_DIR_LNE:
      lsw.x += side_length >> 1;
      lsw.z += side_length >> 1;
      return lsw;

    case OCT_DIR_LSE:
      lsw.x += side_length >> 1;
      return lsw;

    case OCT_DIR_USW:
      lsw.y += side_length >> 1;
      return lsw;

    case OCT_DIR_UNW:
      lsw.y += side_length >> 1;
      lsw.z += side_length >> 1;
      return lsw;

    case OCT_DIR_UNE:
      lsw.x += side_length >> 1;
      lsw.y += side_length >> 1;
      lsw.z += side_length >> 1;
      return lsw;

    case OCT_DIR_USE:
      lsw.x += side_length >> 1;
      lsw.y += side_length >> 1;
      return lsw;
  }
}


// figures out which octant a particular position is in relative to lsw_coord and side_length
int WorldRegion::pick_octant (v3di_t lsw, v3di_t pos, int side_length) {
  v3di_t rel_pos; // relative position

  // subtract the lower south west corner from the pos
  rel_pos = v3di_sub (pos, lsw);

/*  // check to see if the pos is in range
  if (pos.x < 0.0 || pos.x >= side_length ||
    pos.y < 0.0 || pos.y >= side_length ||
    pos.z < 0.0 || pos.z >= side_length) return -1; */

  if (rel_pos.x < side_length >> 1) { // west
    if (rel_pos.y < side_length >> 1) { // lower
      if (rel_pos.z < side_length >> 1) { // south
        return OCT_DIR_LSW; }
      else { // north
        return OCT_DIR_LNW; } }
    else { // upper
      if (rel_pos.z < side_length >> 1) { // south
        return OCT_DIR_USW; }
      else { // north
        return OCT_DIR_UNW; } } }
  else { // east
    if (rel_pos.y < side_length >> 1) { // lower
      if (rel_pos.z < side_length >> 1) { // south
        return OCT_DIR_LSE; }
      else { // north
        return OCT_DIR_LNE; } }
    else { // upper
      if (rel_pos.z < side_length >> 1) { // south
        return OCT_DIR_USE; }
      else { // north
        return OCT_DIR_UNE; } } }

  // should never get here
  return -1;
}


void WorldRegion::clearBlockAtWorldPosition (v3di_t position) {
  clear_block (v3di_sub (position, root.worldPosition));
}


// clear the block at pos if it exists, collapse tree behind it as possible
void WorldRegion::clear_block (v3di_t pos) {
  // if the root has no children, then no blocks have been set
  if (root.num_children == 0) return;

  // check to see if the pos is in range
  if (pos.x < 0.0 || pos.x >= root.side_length ||
    pos.y < 0.0 || pos.y >= root.side_length ||
    pos.z < 0.0 || pos.z >= root.side_length) return;

  // start the recursion
  clear_block_rec (&root, pos);
}


void WorldRegion::clear_block_rec (octree_node *n, v3di_t pos) {
  block_t temp;

  temp.type = BLOCK_TYPE_DIRT;
  temp.numFacesVisible = 6;
  for (int i = 0; i < 6; i++) {
    temp.faceVisible[i] = true;
    temp.facesEarth[i] = true;
  }

  // check if we're at a leaf
  if (n->side_length == 1) {
    // if the block has been allocated, free it
    if (n->block != NULL) {
      // update neighbors' visible faces
      v3di_t n_pos;    // neighbor pos
      block_t *neighbor;  // keep track of our neighbor

      // BLOCK_SIDE_LEF
      n_pos.x = pos.x - 1;
      n_pos.y = pos.y;
      n_pos.z = pos.z;

      neighbor = block_at (n_pos);

      if (neighbor != NULL) {
        neighbor->faceVisible[BLOCK_SIDE_RIG] = true;
        neighbor->facesEarth[BLOCK_SIDE_RIG] = false;
        neighbor->numFacesVisible--;
      }
      else if (n->block->facesEarth[BLOCK_SIDE_LEF]) {
        temp.facesEarth[BLOCK_SIDE_RIG] = false;
        set (n_pos, temp);
        temp.facesEarth[BLOCK_SIDE_RIG] = true;
      }

      // BLOCK_SIDE_RIG
      n_pos.x = pos.x + 1;
      n_pos.y = pos.y;
      n_pos.z = pos.z;

      neighbor = block_at (n_pos);

      if (neighbor != NULL) {
        neighbor->faceVisible[BLOCK_SIDE_LEF] = true;
        neighbor->facesEarth[BLOCK_SIDE_LEF] = false;
        neighbor->numFacesVisible--;
      }
      else if (n->block->facesEarth[BLOCK_SIDE_RIG]) {
        temp.facesEarth[BLOCK_SIDE_LEF] = false;
        set (n_pos, temp);
        temp.facesEarth[BLOCK_SIDE_LEF] = true;
      }

      // BLOCK_SIDE_BOT
      n_pos.x = pos.x;
      n_pos.y = pos.y - 1;
      n_pos.z = pos.z;

      neighbor = block_at (n_pos);

      if (neighbor != NULL) {
        neighbor->faceVisible[BLOCK_SIDE_TOP] = true;
        neighbor->facesEarth[BLOCK_SIDE_TOP] = false;
        neighbor->numFacesVisible--;
      }
      else if (n->block->facesEarth[BLOCK_SIDE_BOT]) {
        temp.facesEarth[BLOCK_SIDE_TOP] = false;
        set (n_pos, temp);
        temp.facesEarth[BLOCK_SIDE_TOP] = true;
      }

      // BLOCK_SIDE_TOP
      n_pos.x = pos.x;
      n_pos.y = pos.y + 1;
      n_pos.z = pos.z;

      neighbor = block_at (n_pos);

      if (neighbor != NULL) {
        neighbor->faceVisible[BLOCK_SIDE_BOT] = true;
        neighbor->facesEarth[BLOCK_SIDE_BOT] = false;
        neighbor->numFacesVisible--;
      }
      else if (n->block->facesEarth[BLOCK_SIDE_TOP]) {
        temp.facesEarth[BLOCK_SIDE_BOT] = false;
        set (n_pos, temp);
        temp.facesEarth[BLOCK_SIDE_BOT] = true;
      }

      // BLOCK_SIDE_FRO
      n_pos.x = pos.x;
      n_pos.y = pos.y;
      n_pos.z = pos.z - 1;

      neighbor = block_at (n_pos);

      if (neighbor != NULL) {
        neighbor->faceVisible[BLOCK_SIDE_BAC] = true;
        neighbor->facesEarth[BLOCK_SIDE_BAC] = false;
        neighbor->numFacesVisible--;
      }
      else if (n->block->facesEarth[BLOCK_SIDE_FRO]) {
        temp.facesEarth[BLOCK_SIDE_BAC] = false;
        set (n_pos, temp);
        temp.facesEarth[BLOCK_SIDE_BAC] = true;
      }

      // BLOCK_SIDE_BAC
      n_pos.x = pos.x;
      n_pos.y = pos.y;
      n_pos.z = pos.z + 1;

      neighbor = block_at (n_pos);

      if (neighbor != NULL) {
        neighbor->faceVisible[BLOCK_SIDE_FRO] = true;
        neighbor->facesEarth[BLOCK_SIDE_FRO] = false;
        neighbor->numFacesVisible--;
      }
      else if (n->block->facesEarth[BLOCK_SIDE_BAC]) {
        temp.facesEarth[BLOCK_SIDE_FRO] = false;
        set (n_pos, temp);
        temp.facesEarth[BLOCK_SIDE_FRO] = true;
      }

      // we don't need this anymore
      free (n->block);

      // decrease the number of blocks in the tree
      num_blocks--;
    }

    // bottom out the recursion
    return;
  }

  // if the node has no children, then no blocks have been set
  if (n->num_children == 0) return;

  // we need to pick the right child
  int child = pick_octant (n->relativePosition, pos, n->side_length);

  // bail if the proper child hasn't been allocated
  if (n->node[child] == NULL) return;

  // recurse
  clear_block_rec (n->node[child], pos);

  // free the child if it has no more children
  if (n->node[child]->num_children == 0) {
    free (n->node[child]);
    n->node[child] = NULL;

    // decrement the child count
    n->num_children--;

    // decrease the count of the tree's total nodes
    num_nodes--; }
}


// get the block at a position if it exists
block_t *WorldRegion::block_at (v3di_t pos) {
  // check to see if the pos is in range
  if (pos.x < 0 || pos.x >= root.side_length ||
    pos.y < 0 || pos.y >= root.side_length ||
    pos.z < 0 || pos.z >= root.side_length) {
      return NULL;
  }

  // no blocks if the root is childless
  if (root.num_children == 0) return NULL;

  int child = pick_octant (root.relativePosition, pos, root.side_length);

  octree_node *n = root.node[child];

  while (n != NULL) {
    if (n->side_length == 1) {
      return n->block; }

    if (n->num_children == 0) return NULL;

    child = pick_octant (n->relativePosition, pos, n->side_length);

    n = n->node[child]; }

  // didn't get to the block
  return NULL;
}



block_t *WorldRegion::getBlockAtWorldPosition (v3di_t pos) {
//  v3di_print ("wp", pos);

  pos = v3di_sub (pos, root.worldPosition);

//  v3di_print ("rp", pos);

  // now that we've accounted for the world displacement, proceed to find block
  return block_at (pos);
}



// get the block at a position if it exists
bool WorldRegion::isSolidBlockAtWorldPosition (v3di_t pos) {
  pos = v3di_sub (pos, root.worldPosition);

  // check to see if the pos is in range
  // say there is a block there if out of range
  if (pos.x < 0 || pos.x >= root.side_length ||
    pos.y < 0 || pos.y >= root.side_length ||
    pos.z < 0 || pos.z >= root.side_length) return false;

  // no blocks if the root is childless
  if (root.num_children == 0) return false;

  int child = pick_octant (root.relativePosition, pos, root.side_length);

  octree_node *n = root.node[child];

  while (n != NULL) {
    if (n->side_length == 1) {
      // block exists
      // FIXME: assuming it's solid for now
      return true; }

    if (n->num_children == 0) return false;

    child = pick_octant (n->relativePosition, pos, n->side_length);

    n = n->node[child]; }

  // didn't make it to a block
  return false;
}



//

//

//

//

//


// draw the blocks for the display list
void WorldRegion::drawForDisplayList (AssetManager &assetManager) {
  // bail if the root has no children
  if (root.num_children == 0) return;

  // adress the child nodes
  for (int i = 0; i < 8; i++) {
    if (root.node[i] != NULL) {
      drawForDisplayList_rec (root.node[i], assetManager);
    }
  }
}



void WorldRegion::drawForDisplayList_rec (octree_node *n, AssetManager &assetManager) {
  // draw the block if we're at a leaf
  if (n->side_length == 1) {
    if (n->block->numFacesVisible > 0) {
      assetManager.drawBlock (n->worldPosition, *n->block);
    }

    return;
  }

  // bail if the node has no children
  if (n->num_children == 0) return;

  // adress the progeny
  for (int i = 0; i < 8; i++) {
    if (n->node[i] != NULL) {
      drawForDisplayList_rec (n->node[i], assetManager);
    }
  }
}


//

//

//

//

//
















// draw the blocks
int WorldRegion::draw (gl_camera_c &cam, AssetManager &assetManager) {
  // bail if the root has no children
  if (root.num_children == 0) return 0;

  // clip the entire region against the frustum if possible
  if (cam.bounding_sphere_test (root.boundingSphere) == FRUSTUM_OUTSIDE) {
    return 0;
  }

  glCallList (mDisplayListHandle);
  return 0;



  int count = 0;

  // adress the child nodes
  for (int i = 0; i < 8; i++) {
    if (root.node[i] != NULL) {
//      int cull = cam.bounding_box_test (root.node[i]->b_box);
      int cull = cam.bounding_sphere_test (root.node[i]->boundingSphere);

      // if the whole box is in, don't do any more tests
      if (cull == FRUSTUM_INSIDE) {
        count += draw_rec (root.node[i], assetManager); }
      // if the box intersects the frustum, check the children
      else if (cull == FRUSTUM_INTERSECT) {
        count += draw_rec (root.node[i], cam, assetManager); } } }

  return count;
}



// recursive part of draw () - no frustum culling
int WorldRegion::draw_rec (octree_node *n, AssetManager &assetManager) {
  // draw the block if we're at a leaf
  if (n->side_length == 1) {
    if (n->block->faceVisible > 0) {
      assetManager.drawBlock (n->worldPosition, *n->block);
    }
    else {
      return 0;
    }

    return 1;
  }

  // bail if the node has no children
  if (n->num_children == 0) return 0;

  int count = 0;

  // adress the progeny
  for (int i = 0; i < 8; i++) {
    if (n->node[i] != NULL) {
        count += draw_rec (n->node[i], assetManager); } }

  return count;
}



// recursive part of draw () - using frustum culling
int WorldRegion::draw_rec (octree_node *n, gl_camera_c &cam, AssetManager &assetManager) {
  // draw the block if we're at a leaf
  if (n->side_length == 1) {
    if (n->block->faceVisible > 0) {
      assetManager.drawBlock (n->worldPosition, *n->block);
    }
    else {
      return 0;
    }

    return 1;
  }

  // bail if the node has no children
  if (n->num_children == 0) return 0;

  int count = 0;

  // adress the progeny
  for (int i = 0; i < 8; i++) {
    if (n->node[i] != NULL) {
//      int cull = cam.bounding_box_test (n->node[i]->b_box);
      int cull = cam.bounding_sphere_test (n->node[i]->boundingSphere);

      if (cull == FRUSTUM_INSIDE) {
        count += draw_rec (n->node[i], assetManager); }
      else if (cull == FRUSTUM_INTERSECT) {
        count += draw_rec (n->node[i], cam, assetManager); } } }

  return count;
}



// return the number of blocks in the tree
int WorldRegion::get_num_blocks (void) {
  return num_blocks;
}


// return the total number of nodes in the tree
int WorldRegion::get_num_nodes (void) {
  return num_nodes;
}



int WorldRegion::generateFromTerrain (int *terrain, v3di_t worldPosition, terrain_c &randomMap) {
  int start_ticks = SDL_GetTicks ();
//  printf ("%6d: WorldRegion::generateFromTerrain --------------\n", SDL_GetTicks ());
//  v3di_print ("worldPosition", worldPosition);

  // start with a clean slate
  setWorldPosition (worldPosition);

  block_t temp;
  int neighbors[4];


  for (int z = 2; z < (root.side_length + 2); z++) {
    for (int x = 2; x < (root.side_length + 2); x++) {
      int ter_y = terrain[x + (z * (root.side_length + 4))];
      // FIXME: sea level hack
      if (ter_y < 0 && worldPosition.y == 0) ter_y = 0;
      //if (ter_y < worldPosition.y || ter_y >= (worldPosition.y + root.side_length)) continue;


      neighbors[0] = terrain[(x - 1) + (z * (root.side_length + 4))];
      neighbors[1] = terrain[(x + 1) + (z * (root.side_length + 4))];
      neighbors[2] = terrain[x + ((z - 1) * (root.side_length + 4))];
      neighbors[3] = terrain[x + ((z + 1) * (root.side_length + 4))];

      int shortestNeighbor = neighbors[0];
      shortestNeighbor = min (shortestNeighbor, neighbors[1]);
      shortestNeighbor = min (shortestNeighbor, neighbors[2]);
      shortestNeighbor = min (shortestNeighbor, neighbors[3]);

      shortestNeighbor++;

      double randomMapValue = randomMap.getValueBilerp (worldPosition.x + x, worldPosition.z + z);

      int stone = 0;
      int snow = 0;
      if ((ter_y - shortestNeighbor > 2) || ter_y > 120) stone = 1;

      for (int y = min (ter_y, shortestNeighbor); y < ter_y + 1; y++) {
        // mark the origin corner
        if (stone && y < ter_y) {
          temp.type = BLOCK_TYPE_STONE;
        }
        else if (y == 0) { // water
          temp.type = BLOCK_TYPE_WATER;
        }
        else if (y < 2) {
          if (randomMapValue > 0.3) temp.type = BLOCK_TYPE_STONE_GRASS;
          else temp.type = BLOCK_TYPE_STONE;
        }
        else if (y < 2 + (4.0 * randomMapValue)) { // stone around the water
          temp.type = BLOCK_TYPE_STONE;
        }
        else if (y <= ter_y - 1) {
          temp.type = BLOCK_TYPE_DIRT; }
        else if (y > (100 + (20.0 * randomMapValue)) && y < 130 + (30.0 * randomMapValue)) {
          temp.type = BLOCK_TYPE_DIRT_SNOW; }
        else if (y >= 120 + (20.0 * randomMapValue)) {
          if (shortestNeighbor >= ter_y) {
            temp.type = BLOCK_TYPE_SNOW; }
          else {
            temp.type = BLOCK_TYPE_STONE_SNOW; }
        }
        else {
          if (stone) {
            if (shortestNeighbor >= ter_y) {
              temp.type = BLOCK_TYPE_STONE; }
            else {
              temp.type = BLOCK_TYPE_STONE_GRASS;} }
          else {
            if (shortestNeighbor >= ter_y) {
              if (randomMapValue < 0.6) {
                temp.type = BLOCK_TYPE_GRASS;
              }
              else {
                temp.type = BLOCK_TYPE_STONE;
              }
            }
            else {
              temp.type = BLOCK_TYPE_DIRT_GRASS;
            }
          }
        }

        // set up the earth facing sides
        // TOP
        if (y == ter_y) {
          temp.facesEarth[BLOCK_SIDE_TOP] = false;
        }
        else {
          temp.facesEarth[BLOCK_SIDE_TOP] = true;
        }

        // BOT
        temp.facesEarth[BLOCK_SIDE_BOT] = true;

        // LEF
        if (neighbors[0] < y) {
          temp.facesEarth[BLOCK_SIDE_LEF] = false;
        }
        else {
          temp.facesEarth[BLOCK_SIDE_LEF] = true;
        }

        // RIG
        if (neighbors[1] < y) {
          temp.facesEarth[BLOCK_SIDE_RIG] = false;
        }
        else {
          temp.facesEarth[BLOCK_SIDE_RIG] = true;
        }

        // FRO
        if (neighbors[2] < y) {
          temp.facesEarth[BLOCK_SIDE_FRO] = false;
        }
        else {
          temp.facesEarth[BLOCK_SIDE_FRO] = true;
        }

        // BAC
        if (neighbors[3] < y) {
          temp.facesEarth[BLOCK_SIDE_BAC] = false;
        }
        else {
          temp.facesEarth[BLOCK_SIDE_BAC] = true;
        }

        // now set the block in region relative coordinates
        setBlockAtWorldPosition (v3di_v ((x - 2) + worldPosition.x, y, (z - 2) + worldPosition.z), temp);
      }
    }
  }

//  printf ("      nodes: %d, blocks: %d\n", get_num_nodes (), get_num_blocks ());
//  printf ("      time: %d\n\n", SDL_GetTicks () - start_ticks);

  return 0;
}


void WorldRegion::generateFromNoise (v3di_t worldPosition, Noise3d &noise) {
  int start_ticks = SDL_GetTicks ();
  printf ("%6d: WorldRegion::generateFromNoise --------------\n", SDL_GetTicks ());
//  v3di_print ("worldPosition", worldPosition);

  // start with a clean slate
  setWorldPosition (worldPosition);

  block_t temp;

  temp.type = BLOCK_TYPE_STONE_GRASS;

  v3di_t relativePosition;

  int blockCount = 0;

  for (relativePosition.z = 0; relativePosition.z < root.side_length; relativePosition.z++) {
    for (relativePosition.y = 0; relativePosition.y < root.side_length; relativePosition.y++) {
      for (relativePosition.x = 0; relativePosition.x < root.side_length; relativePosition.x++) {
        v3di_t blockPosition = v3di_add (worldPosition, relativePosition);

        double x = 0.1 * static_cast<double>(blockPosition.x);
        double y = 0.1 * static_cast<double>(blockPosition.y);
        double z = 0.1 * static_cast<double>(blockPosition.z);

        double value = noise.getValueTrilerp (x, y, z);

        value -= y * 0.5;

        if (value > -0.25) {
          blockCount++;
          setBlockAtWorldPosition (blockPosition, temp);
        }

      }
    }
  }

  printf ("blocks: %d\n", blockCount);
//  printf ("      nodes: %d, blocks: %d\n", get_num_nodes (), get_num_blocks ());
  printf ("      time: %d\n\n", SDL_GetTicks () - start_ticks);



}


int WorldRegion::generateDisplayList (AssetManager &assetManager) {
  if (mDisplayListHandle != -1) {
    glDeleteLists (mDisplayListHandle, 1);
  }

  // generate the display list
  mDisplayListHandle = glGenLists (1);

  // create the list, but don't draw it yet
  glNewList (mDisplayListHandle, GL_COMPILE);
    drawForDisplayList (assetManager);
  glEndList();

  return mDisplayListHandle;
}



#endif // _WorldRegion_h
