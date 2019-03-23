#include "Branch.h"

Branch::Branch() {}

Branch::Branch(branch_ptr parent_, glm::vec2 p_, glm::vec2 d_) {
  pos = p_;
  originalRelativePos = p_;
  parent = parent_;
  if(parent != NULL) {
    pos = parent->pos + originalRelativePos;
  }
  direction = d_;
  originalDirection = d_;
  thicknessGrowth = ofRandomuf()*0.03 + 0.01;
}

branch_ptr Branch::next() {
  isEndSegment = false;
  glm::vec2 nextDir = direction * length;
  if(parent != NULL) parent->propagateGrowth(thicknessGrowth);
  branch_ptr newBranch = branch_ptr(new Branch(shared_from_this(), nextDir, direction));
  std::weak_ptr<Branch> newWeakPtr = newBranch;
  children.push_back(newWeakPtr);
  return newBranch;
}

void Branch::resetBranch() {
  direction = originalDirection;
  count = 0;
}

void Branch::show(ofColor col) {
  if(parent != NULL) {
    ofSetColor(col);
    //strokeWeight(thickness);

    if(thickness < 10) {
      ofSetLineWidth(thickness);
      ofDrawLine(pos.x, pos.y, parent->pos.x, parent->pos.y);
    } else {
    float w = pos.x - parent->pos.x;
    float h = pos.y - parent->pos.y;
    ofDrawRectangle(pos.x, pos.y, w*thickness, h);
    }
  }
}

void Branch::propagateGrowth(float growthAmount) {
  thickness += growthAmount;
  if(parent != NULL) parent->propagateGrowth(thicknessGrowth);
}

void Branch::update() {
  if(parent != NULL)
    pos = parent->pos + originalRelativePos;
  if(thickness > 20.) canSpawn = false;
}

void Branch::deleteBranch() {
  // go through all the child's children
  for(auto c : children) {
    if(!c.expired()) {
      auto cPtr = c.lock(); // lock the weak_ptr to get a temporary shared_ptr
      // 1. Set their parent to this Branch's parent
      cPtr->parent = parent;
      // 2. Add this Branch's originalRelativePos to theirs
      cPtr->originalRelativePos += originalRelativePos;
      // 3. Add as a child to parent
      parent->children.push_back(c);
    }
  }
  // 4. Set isDeleted
  isDeleted = true;

}

void Branch::simplifyChildren(float distanceThreshold) {
  if(children.size() == 0) return;

  for(int i = children.size()-1; i >= 0; i--) {
    auto c = children[i];

    if(!c.expired()) {
      auto cPtr = c.lock(); // lock the weak_ptr to get a temporary shared_ptr
      if(!cPtr->isDeleted) {
        // can this be improved so that both delete branch and simplifyChildren can be run the same run through?
        if(glm::distance(cPtr->originalRelativePos, originalRelativePos) < distanceThreshold) {
          cPtr->deleteBranch();
        } else {
          cPtr->simplifyChildren(distanceThreshold); // could set its children to a node that is removed
        }
      } else {
        // child has been deleted, remove from list
        children.erase(children.begin() + i);
      }
    } else {
      // child has been deleted, remove from list
      children.erase(children.begin() + i);
    }
  }
}

void Branch::propagateWind() {

}
