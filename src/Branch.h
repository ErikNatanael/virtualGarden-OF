#ifndef BRANCH_H_
#define BRANCH_H_

#include "ofMain.h"
#include "Leaf.h"
#include <memory>

enum struct BranchVisual {GREEN, DIGITAL, RED, RANDOM};

// std::enable_shared_from_this<Branch> is an easy way to create a shared_ptr
// from an instance of an object (which the next method needs to do)
class Branch : public std::enable_shared_from_this<Branch> {
public:
  glm::vec2 pos;
  glm::vec2 originalRelativePos;
  glm::vec2 deadStartPos;
  std::shared_ptr<Branch> parent;
  vector<std::weak_ptr<Branch> > children;
  vector<leaf_ptr > leaves;
  glm::vec2 direction;
  glm::vec2 originalDirection;
  BranchVisual visualType = BranchVisual::DIGITAL;
  int count = 0;
  float length = 3;
  bool isEndSegment = true;
  bool canSpawn = true;
  bool isDeleted = false;
  bool isDead = false;
  bool needsRedraw = true;

  float maxHp = 2;
  float hp = 2;
  float damagePerTick = 0.01;

  float thickness = 1.;
  float thicknessGrowth = 0.01;

  Branch();
  Branch(std::shared_ptr<Branch> parent_, glm::vec2 p_, glm::vec2 d_);
  std::shared_ptr<Branch> next();
  void resetBranch();
  void show(ofColor col, float totalTime, bool redraw = true);
  void drawLeaves(float totalTime);
  void propagateGrowth(float growthAmount);
  void propagateDeath();
  void update(float dt);
  void propagateWind();
  void simplifyChildren(float distanceThreshold);
  void deleteBranch();
  bool canGrowBranch();
  float fillHP(float energy);
  void killBranch(vector<std::shared_ptr<Branch> >& killedBranches);
  leaf_ptr addLeaf();
  ofColor calculateBranchColor();

};

typedef std::shared_ptr<Branch> branch_ptr; // alias to reduce typing and typos

#endif /* end of include guard: BRANCH_H_ */
