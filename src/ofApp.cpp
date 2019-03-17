#include "ofApp.h"
#include "globals.h"

bool overlay = true;

//--------------------------------------------------------------
void ofApp::setup(){

  ofSetWindowTitle("!openFrameworks!: Virtual Garden");

  font.load("SpaceMono-Regular.ttf", 20);

  int numTrees = 1;
  for (int i = 0; i < numTrees; i++) {
    int x = (ofGetWidth()/(numTrees+1))*(i+1);
    trees.push_back(Tree(glm::vec2(x, ofGetHeight())));
  }

  for (int i = 0; i < 10; i++ ) {
    roseBushes.push_back(RoseBush(
      glm::vec2(ofRandom(ofGetWidth()*0.2, ofGetWidth()*0.8), ofGetHeight())));
  }

  sun = Sun(glm::vec2(ofGetWidth()/2, 0), 10.);

}

//--------------------------------------------------------------
void ofApp::update(){

  double currentTime = ofGetElapsedTimef();
  dt = (float)(currentTime - lastTime);
  lastTime = currentTime;
  totalTime += dt;

  if(!pause) {
    sun.update();
  }

  if (doTrees) {
    for (int i = 0; i < trees.size(); i++) {
      Tree* tree = &trees[i];
      if (grow && !pause) trees[i].grow();
      if(!pause)trees[i].update(dt, sun);
    }
  }

  if (doRoses) {
    for (int i = 0; i < roseBushes.size(); i++) {
      if(!pause) if (grow) roseBushes[i].grow();
      if(!pause) roseBushes[i].update(dt, sun);
    }
  }

}

//--------------------------------------------------------------
void ofApp::draw(){
  ofBackground(41+sun.strength*1., 41+sun.strength*1.1, 41+sun.strength*1.4);
  sun.show();

  if (doTrees) {
    for (int i = 0; i < trees.size(); i++) {
      trees[i].show(font, totalTime);
    }
  }

  if (doRoses) {
    for (int i = 0; i < roseBushes.size(); i++) {
      roseBushes[i].show(font, totalTime);
    }
  }

  if (overlay) {
    string fps = "fps " + to_string(ofGetFrameRate());
    ofSetColor(255);
    font.drawString(fps, 10, 30);

    int nb = 0;
    for (Tree t : trees) nb += t.branches.size();
    for (RoseBush t : roseBushes) nb += t.branches.size();
    string numBranches = "number of branches: " + to_string(nb);
    font.drawString(numBranches, 10, 60);

    int ngp = 0;
    for (Tree t : trees) ngp += t.growthPoints.size();
    string numPoints = "number of points: " + to_string(ngp);
    font.drawString(numPoints, 10, 90);

    int nl = 0;
    for (Tree t : trees) nl += t.leaves.size();
    for (RoseBush t : roseBushes) nl += t.leaves.size();
    string numLeaves = "number of leaves: " + to_string(nl);
    font.drawString(numLeaves, 10, 120);

    int nr = 0;
    for (RoseBush t : roseBushes) nr += t.roses.size();
    string numRoses = "number of roses: " + to_string(nr);
    font.drawString(numRoses, 10, 150);
  }

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
  if (key == 'g') grow = !grow;
  if (key == 'o') overlay = !overlay;
  if (key == 's') {
    grabImg.grabScreen(0, 0 , ofGetWidth(), ofGetHeight());
    grabImg.save("screenshots/screenshot" + ofGetTimestampString() + ".png");
  }
  if (key == 'c') {
    trees[0].checkBranchSamePosition();
    trees[0].checkBranchSameParent();
  }
  if (key == 'p') pause = !pause;
  switch(key) {
    case OF_KEY_LEFT:
      trees[0].moveAllBranches(glm::vec2(-50, 0));
      break;
    case OF_KEY_RIGHT:
      trees[0].moveAllBranches(glm::vec2(50, 0));
      break;
  }

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){

}
