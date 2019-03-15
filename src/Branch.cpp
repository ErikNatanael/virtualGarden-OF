#include "Branch.h"

Branch::Branch() {}

Branch::Branch(branch_ptr parent_, glm::vec2 p_, glm::vec2 d_) {
  pos = p_;
  originalPos = p_;
  parent = parent_;
  direction = d_;
  originalDirection = d_;
  thicknessGrowth = ofRandomuf()*0.02;
}

branch_ptr Branch::next() {
  isEndSegment = false;
  glm::vec2 nextDir = direction * length;
  if(parent != NULL) parent->propagateGrowth(thicknessGrowth);
  return branch_ptr(new Branch(shared_from_this(), pos + nextDir, direction));
}

void Branch::reset() {
  direction = originalDirection;
  count = 0;
}

void Branch::show(ofColor col) {
  if(parent != NULL) {
    ofSetColor(col);
    //strokeWeight(thickness);
    ofDrawLine(pos.x, pos.y, parent->pos.x, parent->pos.y);
  }
}

void Branch::propagateGrowth(float growthAmount) {
  thickness += growthAmount;
  if(parent != NULL) parent->propagateGrowth(thicknessGrowth);
}

void Branch::update() {
  if(thickness > 20.) canSpawn = false;
}

void Branch::propagateWind() {

}
