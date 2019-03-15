#pragma once

#include "ofMain.h"
#include "Branch.h"
#include "Sun.h"

class Leaf {
public:
  float angle;
  branch_ptr parentBranch;
  bool shiftSide = false;
  float lifeTime;
  bool falling = false;
  glm::vec2 pos;
  float randomSeed;
  float size;
  float maxSize = 15;
  bool dead = false;
  ofColor fillCol;

  Leaf(branch_ptr parent, float angle_);
  void drawLeafShape();
  void update(float dt);
  void show(float totalTime);
  float getEnergy(Sun sun);
};
