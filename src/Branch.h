#pragma once

#include "ofMain.h"
#include <memory>

// std::enable_shared_from_this<Branch> is an easy way to create a shared_ptr
// from an instance of an object (which the next method needs to do)
class Branch : public std::enable_shared_from_this<Branch> {
public:
  glm::vec2 pos;
  glm::vec2 originalPos;
  std::shared_ptr<Branch> parent;
  glm::vec2 direction;
  glm::vec2 originalDirection;
  int count = 0;
  float length = 10;
  bool isEndSegment = true;
  bool canSpawn = true;

  float thickness = 1.;
  float thicknessGrowth = 0.01;

  Branch();
  Branch(std::shared_ptr<Branch> parent_, glm::vec2 p_, glm::vec2 d_);
  std::shared_ptr<Branch> next();
  void resetBranch();
  void show(ofColor col);
  void propagateGrowth(float growthAmount);
  void update();
  void propagateWind();

};

typedef std::shared_ptr<Branch> branch_ptr; // alias to reduce typing and typos
