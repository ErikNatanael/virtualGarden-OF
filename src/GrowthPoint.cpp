#include "GrowthPoint.h"

GrowthPoint::GrowthPoint() {
  pos = glm::vec2(ofRandom(0, ofGetWidth()), ofRandom(ofGetHeight()-200));
}

GrowthPoint::GrowthPoint(glm::vec2 p_) {
  pos = p_;
}

void GrowthPoint::show() {
  //noStroke();
  ofSetColor(255);
  ofDrawEllipse(pos.x, pos.y, 4, 4);
}

void GrowthPoint::update(float dt) {
  lifeTimeLeft -= dt;
  if (lifeTimeLeft < 0) expired = true;
}
