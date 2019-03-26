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
  thicknessGrowth = ofRandomuf()*0.02 + 0.01;
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

    // set color by hp
    ofSetColor((hp/maxHp) * 255, 0, 0);
    if(isDead) ofSetColor(255);

    if(thickness < 10) {
      ofSetLineWidth(thickness);
      ofDrawLine(pos.x, pos.y, parent->pos.x, parent->pos.y);
    } else {
      float w = pos.x - parent->pos.x;
      float h = pos.y - parent->pos.y;
      ofDrawEllipse(pos.x, pos.y, abs(w) + thickness, abs(h)*2 + thickness*.5);
      // weird digital artefact trees
      ofDrawRectangle(pos.x, pos.y, abs(w)*thickness, h);
    }
  }
}

void Branch::propagateGrowth(float growthAmount) {
  thickness += growthAmount;
  maxHp += damagePerTick;
  if(parent != NULL) parent->propagateGrowth(thicknessGrowth);
}

void Branch::update() {
  if(parent != NULL)
    pos = parent->pos + originalRelativePos;
  if(thickness > 20.) canSpawn = false;

  if(hp <= 0) {
    isDead = true;
    canSpawn = false;
  }
  hp -= damagePerTick;
}

void Branch::deleteBranch() {
  // go through all the child's children
  for(auto& c : children) {
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

bool Branch::canGrowBranch() {
  return canSpawn;
}

void Branch::propagateWind() {

}

float Branch::fillHP(float energy) {
  float hpMissing = maxHp-hp;
  float energySpent = 0;
  if(energy > maxHp-hp) {
    hp = maxHp;
    energySpent = hpMissing;
    energy -= energySpent;
  }
  // create energy division vector based on maxHp of children
  vector<float> energyDivision;
  float totalMaxHp = 0;
  for(auto& c : children) {
    if(!c.expired()) {
      auto cPtr = c.lock(); // lock the weak_ptr to get a temporary shared_ptr
      energyDivision.push_back(cPtr->maxHp);
      totalMaxHp += cPtr->maxHp;
    }
  }
  // normalise vector
  for( auto& f : energyDivision) { f /= totalMaxHp; }
  for(int i = 0; i < children.size(); i++) {
    auto& c  = children[i];
    if(!c.expired()) {
      auto cPtr = c.lock(); // lock the weak_ptr to get a temporary shared_ptr
      energySpent += cPtr->fillHP(energy*energyDivision[i]);
    }
  }

  return energySpent;
}

void Branch::killBranch(vector<branch_ptr>& killedBranches) {
  if(!isDead) killedBranches.push_back(shared_from_this());
  isDead = true;
  parent = NULL;
  for(int i = 0; i < children.size(); i++) {
    auto& c  = children[i];
    if(!c.expired()) {
      auto cPtr = c.lock(); // lock the weak_ptr to get a temporary shared_ptr
      cPtr->killBranch(killedBranches);
    }
  }
}
