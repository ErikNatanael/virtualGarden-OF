#pragma once

#include "ofMain.h"

class GrowthPoint {
public:
  glm::vec2 pos;
  bool reached = false;
  GrowthPoint();
  GrowthPoint(glm::vec2 p_);
  void show();
};
