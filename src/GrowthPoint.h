#ifndef GROWTHPOINT_H_
#define GROWTHPOINT_H_

#include "ofMain.h"

class GrowthPoint {
public:
  glm::vec2 pos;
  bool reached = false;
  bool expired = false;
  float lifeTimeLeft = 10.;
  GrowthPoint();
  GrowthPoint(glm::vec2 p_);
  void show();
  void update(float dt);
};

#endif /* end of include guard: GROWTHPOINT_H_ */
