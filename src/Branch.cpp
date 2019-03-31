#include "Branch.h"
#include "Leaf.h"

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

void Branch::show(ofColor col, float totalTime) {
  if(parent != NULL || isDead) {
    ofSetColor(col);
    //strokeWeight(thickness);

    // set color by hp
    ofSetColor((hp/maxHp) * 255, 0, 0);
    if(isDead) ofSetColor(255);

    glm::vec2 drawTo;
    if(isDead) drawTo = deadStartPos;
    else       drawTo = parent->pos;

    if(thickness < 10) {
      ofSetLineWidth(thickness);
      ofDrawLine(pos.x, pos.y, drawTo.x, drawTo.y);
    } else {
      float w = pos.x - drawTo.x;
      float h = pos.y - drawTo.y;
      ofDrawEllipse(pos.x, pos.y, abs(w) + thickness, abs(h)*2 + thickness*.5);
      // weird digital artefact trees
      ofDrawRectangle(pos.x, pos.y, abs(w)*thickness, h);
    }

    // draw leaves
    for (int i = 0; i < leaves.size(); i++) {
      leaves[i]->show(totalTime, direction);
    }
  }
}

void Branch::propagateGrowth(float growthAmount) {
  thickness += growthAmount;
  maxHp += damagePerTick;
  if(parent != NULL) parent->propagateGrowth(thicknessGrowth);
}

void Branch::update(float dt) {
  if(parent != NULL)
    pos = parent->pos + originalRelativePos;
  else if(isDead)
    pos = deadStartPos + originalRelativePos;
  if(thickness > 20.) canSpawn = false;

  if(hp <= 0) {
    isDead = true;
    canSpawn = false;
  }
  hp -= damagePerTick;

  for (int i = leaves.size()-1; i >= 0; i--) {
    leaves[i]->update(dt, pos);
    if (leaves[i]->dead) leaves.erase(leaves.begin() + i);
  }
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
  // fill up leaves
  for(auto& c : leaves) {
    float e = c->fillHP(energy);
    energySpent += e;
    energy -= e;
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
  // this is called if a branch dies and need to be removed from the tree
  if(!isDead) {
    killedBranches.push_back(shared_from_this());
    if(parent!=NULL) deadStartPos = parent->pos;
    else deadStartPos = glm::vec2(ofGetWidth()*.5, ofGetHeight());
  }
  isDead = true;
  parent = NULL;

  for(int i = 0; i < children.size(); i++) {
    auto& c  = children[i];
    if(!c.expired()) {
      auto cPtr = c.lock(); // lock the weak_ptr to get a temporary shared_ptr
      cPtr->killBranch(killedBranches);
    }
  }

  for(auto& l : leaves) {
    l->lifeTime = 0;
    l->falling = true;
  }
}

leaf_ptr Branch::addLeaf() {
  leaf_ptr newLeaf = make_shared<Leaf>(0.5);
  leaves.push_back(newLeaf);
  return newLeaf;
}
