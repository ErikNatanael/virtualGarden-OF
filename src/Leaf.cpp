#include "Leaf.h"
#include "Branch.h"

Leaf::Leaf(branch_ptr parent, float angle_) {
  parentBranch = parent;
  angle = angle_;
  if(ofRandomuf()>0.5) {
    shiftSide = true;
    angle = angle*-1;
  }
  size = 1;
  fillCol = ofColor(100, 200, 100);
  maxSize = ofRandom(7, 15);

  lifeTime = 15.;
  randomSeed = ofRandom(100);
}

void Leaf::drawLeafShape() {
  ofBeginShape();
  ofSetColor(fillCol);
  //stroke(0);

  ofVertex(0, 0);
  ofVertex(-size, -size);
  ofVertex(0, -3*size);
  ofVertex(10, -size);
  ofVertex(0, 0);

  ofEndShape();

}

void Leaf::update(float dt) {
  lifeTime -= dt;
  if(size < maxSize) {
    size += 0.1;
  }
  if(lifeTime < 2.) {
    fillCol = ofColor(255,140,0);
  }
  if(lifeTime < 0.) {
    falling = true;
  }
  if(!falling) {
    pos = glm::vec2(parentBranch->pos.x, parentBranch->pos.y);
  } else {
    pos = glm::vec2(pos.x, pos.y+3);
  }

  if(falling && pos.y > ofGetHeight() + 20) dead = true;

}

void Leaf::show(float totalTime) {

  float rotation = atan2(parentBranch->direction.y, parentBranch->direction.x);
  if(shiftSide) rotation += PI;
  float wind = ofNoise(parentBranch->pos.x*0.01, parentBranch->pos.y*0.01, totalTime);
  wind += ofNoise(parentBranch->pos.x*0.01, parentBranch->pos.y*0.01, totalTime*2)*0.3;
  wind *= TWO_PI;

  if(!falling) {
    rotation = rotation + angle;
    rotation = rotation*0.5 + wind*0.5;
  } else {
    rotation = sin(totalTime*5 + randomSeed)*1.5-1.5 + wind*.3;
  }
  ofPushMatrix();
  ofTranslate(pos.x, pos.y);
  ofRotateRad(rotation);
  drawLeafShape();
  ofPopMatrix();
}

float Leaf::getEnergy(Sun sun) {
  float energy = sun.strength*0.0001*size;
  if(falling) return 0;

  return energy;

}
