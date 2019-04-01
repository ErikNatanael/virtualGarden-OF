#ifndef LEAF_H_
#define LEAF_H_

#include "ofMain.h"
#include "Sun.h"
#include <memory>

class Leaf {
public:
  float angle;
  bool shiftSide = false;
  float lifeTime;
  float hp;
  float maxHp;
  float damagePerTick = 0.05;
  bool falling = false;
  glm::vec2 pos;
  float randomSeed;
  float size;
  float maxSize = 15;
  bool dead = false;
  float alpha = 150;
  ofColor fillCol;

  Leaf(float angle_);
  void drawLeafShape();
  void update(float dt, glm::vec2 parentPos);
  void show(float totalTime, glm::vec2 direction);
  float getEnergy(Sun sun);
  float fillHP(float energy);
};

typedef std::shared_ptr<Leaf> leaf_ptr; // alias to reduce typing and typos

#endif /* end of include guard: LEAF_H_ */
