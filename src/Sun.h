#pragma once
#include "ofMain.h"

class Sun {
public:
  glm::vec2 pos;
  float strength;

  Sun() {};
  Sun(glm::vec2 pos_, float strength_) {
    pos = pos_;
    strength = strength_;
  }

  void update(float strength_) {
    float maxVal = ofGetHeight() - 100;
    float minVal = ofGetHeight()*-.55;
    if(strength_ == -1) {
      float phase = sin(ofGetElapsedTimef()*0.1);
      strength = phase*50+60;
      phase = phase*0.5+.5;
      pos = glm::vec2(pos.x, ofGetHeight() - (phase*(maxVal-minVal)+minVal));
    } else {
      strength = strength_;
      float phase = strength/400;
      pos = glm::vec2(pos.x, ofGetHeight() - (phase*(maxVal-minVal)+minVal));
    }
  }


  void show() {
    //noStroke();
    ofSetColor(255, 80+strength*1.5, 65 + strength); // more blue = brighter yellow
    float size = ofGetWidth() - (strength* (ofGetWidth()/147.0)); 
    ofDrawEllipse(pos.x, pos.y, size, size);
  }

};
