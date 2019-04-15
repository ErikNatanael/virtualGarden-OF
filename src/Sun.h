#pragma once
#include "ofMain.h"

class Sun {
public:
  glm::vec2 pos;
  float strength;
  float phase = 0;

  Sun() {};
  Sun(glm::vec2 pos_, float strength_) {
    pos = pos_;
    strength = strength_;
  }

  void update(float strength_) {
    float maxVal = ofGetHeight() - 100;
    float minVal = ofGetHeight()*-.55;
    if(strength_ == -1) {
      phase = sin(ofGetElapsedTimef()*0.1);
      strength = phase*50+60;
      phase = phase*0.5+.5;
      pos = glm::vec2(pos.x, ofGetHeight() - (phase*(maxVal-minVal)+minVal));
    } else {
      phase = strength_/200;
      strength = phase*100+10;
      pos = glm::vec2(pos.x, ofGetHeight() - (phase*(maxVal-minVal)+minVal));
    }
  }


  void show() {
    //noStroke();
    phase = pow(phase, 2)*.5 + 0.5;
    ofSetColor(255, (80+strength*1.5) * phase, (65 + strength) * phase); // more blue = brighter yellow
    float size = ofGetWidth() - (strength* (ofGetWidth()/147.0));
    ofDrawEllipse(pos.x, pos.y, size, size);
  }

};
