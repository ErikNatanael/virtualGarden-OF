#pragma once
#include "ofMain.h"

#include "Branch.h"

enum RoseShape {ROSE, TWO, OUTLINE};

class Rose {
public:
  glm::vec2 pos;
  float size;
  float maxSize;
  float numPetals;
  float maxPetals;
  branch_ptr parentBranch;

  RoseShape type = ROSE;

  float lifeTime = 25.;
  bool dead = false;


  Rose(branch_ptr parentBranch_) {
    pos = glm::vec2(0, 0);
    size = 2;
    numPetals = 1;
    parentBranch = parentBranch_;
    maxPetals = 4+floor(ofRandom(3));
    maxSize = 7 + ofRandom(10);
  }

  void show() {
    polarRose(size, numPetals, ofColor(255, 100, 100));
  }

  void update(float dt) {
    lifeTime -= dt;
    if(numPetals < maxPetals) numPetals += dt*0.2;
    pos = glm::vec2(parentBranch->pos.x, parentBranch->pos.y);

    if(lifeTime > 0) {
      if(size < maxSize) size += dt*2.;
    } else {
      if(size > 0) size -= dt*4.;
    }

    if(size <= 0 && lifeTime <= 0) {
      dead = true;
    }
  }


  void polarRose(float a, float b, ofColor c) {
    float x, y;
    float t;      //angle theta
    float r;
    int points = 100;


    //noFill();
    //strokeWeight(1);

    ofSetColor(c);
    ofPushMatrix();
      ofTranslate(pos.x, pos.y);


      ofBeginShape();

      for(int i = 0; i<points; i++) {
        t = (TWO_PI/points)*i;
        switch(type) {
          case ROSE:
            r = a * cos(b*t+(PI*0.5));
            break;
          case TWO:
            r = a * ( cos(tan(b*t+(PI*0.5))) );
            break;
          case OUTLINE:
            r = a * ( cos(sin(b*t+(PI*0.5))) );
            break;
        }
        x = r * cos(t);
        y = r * sin(t);
        ofVertex(x, y);
      }

      ofEndShape();

    ofPopMatrix();
  }

};
