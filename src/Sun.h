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

  void update() {
    float phase = sin(ofGetElapsedTimef()*0.1);
    strength = phase*50+60;
    float maxVal = ofGetHeight() - 100;
    float minVal = -600;
    phase = phase*0.5+.5;
    pos = glm::vec2(pos.x, ofGetHeight() - (phase*(maxVal-minVal)+minVal));
  }


  void show() {
    //noStroke();
    ofSetColor(255, 150+strength*1.5, 100 + strength); // more blue = brighter yellow
    float size = ofGetWidth() - (strength*13);
    ofDrawEllipse(pos.x, pos.y, size, size);
  }

};
