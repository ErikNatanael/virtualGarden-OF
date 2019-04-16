#pragma once
#include "ofMain.h"

class Sun {
public:
  glm::vec2 pos;
  float strength;
  float phase = 0;
  float extraSize = 1.;

  Sun() {};
  Sun(glm::vec2 pos_, float strength_) {
    pos = pos_;
    strength = strength_;
  }

  void update(float strength_) {
    float maxVal = ofGetHeight() - 100;
    float minVal = ofGetHeight()*-.55;
    float maxPhase = 1.;
    float offset = 0.;
    if(strength_ == -1) {
      extraSize = 0.;
    } else {
      maxPhase = ofClamp(strength_/80.0, 0.0, 1.0);
      offset = ofClamp((strength_-80.0)/100., 0.0, 1.0);
      extraSize = ofMap((strength_-80.0)/300., 0., 1., 1.0, 3.0);
    }
    phase = sin(ofGetElapsedTimef()*0.3);
    phase = ofMap(phase, -1.0, 1.0, offset, maxPhase);
    strength = phase*100+10;

    pos = glm::vec2(pos.x, ofGetHeight() - (phase*(maxVal-minVal)+minVal));
  }


  void show() {
    //noStroke();
    phase = pow(phase, 2)*.5 + 0.5;
    ofSetColor(255, (80+strength*1.5) * phase, (65 + strength) * phase); // more blue = brighter yellow
    float size = ofGetWidth() - (strength* (ofGetWidth()/147.0));
    size *= extraSize;
    ofDrawEllipse(pos.x, pos.y, size, size);
  }

};
