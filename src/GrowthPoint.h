#ifndef GROWTHPOINT_H_
#define GROWTHPOINT_H_

#include "ofMain.h"

class GrowthPoint {
public:
  glm::vec2 pos;
  bool reached = false;
  GrowthPoint();
  GrowthPoint(glm::vec2 p_);
  void show();
};

#endif /* end of include guard: GROWTHPOINT_H_ */
