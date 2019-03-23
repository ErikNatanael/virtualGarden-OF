#pragma once

#include "ofMain.h"
#include <memory>

// std::enable_shared_from_this<Branch> is an easy way to create a shared_ptr
// from an instance of an object (which the next method needs to do)
class Branch : public std::enable_shared_from_this<Branch> {
public:
  glm::vec2 pos;
  glm::vec2 originalRelativePos;
  std::shared_ptr<Branch> parent;
  vector<std::weak_ptr<Branch> > children;
  glm::vec2 direction;
  glm::vec2 originalDirection;
  int count = 0;
  float length = 3;
  bool isEndSegment = true;
  bool canSpawn = true;
  bool isDeleted = false;

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
  void simplifyChildren(float distanceThreshold);
  void deleteBranch();
  bool canGrowBranch();

};

typedef std::shared_ptr<Branch> branch_ptr; // alias to reduce typing and typos
