#include "ofApp.h"
#include "globals.h"
#include "ofxPlotter.h"

#include <sstream>

// function from https://stackoverflow.com/questions/16605967/set-precision-of-stdto-string-when-converting-floating-point-values
template <typename T>
std::string to_string_with_precision(const T a_value, const int n = 6)
{
    std::ostringstream out;
    out.precision(n);
    out << std::fixed << a_value;
    return out.str();
}

bool overlay = true; // initialising the global variable overlay

//--------------------------------------------------------------
void ofApp::setup(){

  ofSetWindowTitle("!openFrameworks!: Virtual Garden");
  ofHideCursor();

  font.load("SpaceMono-Regular.ttf", 20);

  deadTreesFbo.allocate(ofGetWidth(), ofGetHeight());

  oscReceiver.setup(7771);
  motionTrackingValues = vector<glm::vec2>(motionTrackingPoints);

  int baud = 115200;
  serial.setup(0, baud); //open the first device and talk to it at 57600 baud
  serial.flush();

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

  plotter["energy"] << 0;
  plotter["points"] << 0;
  plotter["energyHistory"] << 0;
  plotter["pointsHistory"] << 0;

}

//--------------------------------------------------------------
void ofApp::update(){

  double currentTime = ofGetElapsedTimef();
  dt = (float)(currentTime - lastTime);
  lastTime = currentTime;
  totalTime += dt;

  if(currentTime - lastTreeReset > resetTreeTime) {
    lastTreeReset = currentTime;
    resetTreeTime = ofRandom(2, 30);
    makeNewTree();
  }

  receiveOscMessages();
  readSerialData();

  // simplify tree if frameRate drops too low
  static double lastTimeSimplification = 0;
  if(ofGetFrameRate() < 30.0 && ((currentTime - lastTimeSimplification) > 5.) ) {

    if(simplificationThresh < 3) {
      for(auto& t : trees) {
        t.simplifyTree(simplificationThresh);
      }
      simplificationThresh += 0.5;
    } else {
      // make a new tree
      makeNewTree();
    }
    lastTimeSimplification = currentTime;
  }

  if(!pause) {
    sun.update(-1);
    //sun.update(light);
  }

  if (doTrees) {
    for (int i = 0; i < trees.size(); i++) {
      Tree* tree = &trees[i];
      if (grow && !pause) trees[i].grow();
      if(!pause)trees[i].update(dt, sun);
    }

    // add plotting data
    plotter["energy"] << trees[0].energy;
    plotter["points"] << float(trees[0].growthPoints.size());
    const int AVERAGE_FRAMES = 30;
    static float averageEnergy = 0;
    static float averagePoints = 0;
    averageEnergy += trees[0].energy;
    averagePoints += float(trees[0].growthPoints.size());
    if(ofGetFrameNum() % AVERAGE_FRAMES == 0) {
      averageEnergy /= AVERAGE_FRAMES;
      averagePoints /= AVERAGE_FRAMES;
      plotter["energyHistory"] << averageEnergy;
      plotter["pointsHistory"] << averagePoints;
      averagePoints = 0;
      averageEnergy = 0;
    } else {
      plotter.history["energyHistory"].erase(plotter.history["energyHistory"].end()-1);
      plotter.history["pointsHistory"].erase(plotter.history["pointsHistory"].end()-1);
    }

    for(int i = deadTrees.size()-1; i >= 0; i--) {
      deadTrees[i].deadAlpha -= 0.01; // should reach 0 alpha after ca 5 min at 60 fps
      if(deadTrees[i].deadAlpha <= 0) {
        deadTrees.erase(deadTrees.begin() + i);
      }
    }
  }


  /*for(int i = 0; i < motionTrackingValues.size(); i++) {
    if(ofRandomuf() > 0.99) {
      auto& v = motionTrackingValues[i];
      GrowthPoint temp = GrowthPoint(glm::vec2(v.x + ofRandomf()*20, v.y + ofRandomf()*20));
      for(auto& t : trees) {
        t.growthPoints.push_back(temp);
      }
    }
  }*/

  if (doRoses) {
    for (int i = 0; i < roseBushes.size(); i++) {
      if(!pause) if (grow) roseBushes[i].grow();
      if(!pause) roseBushes[i].update(dt, sun);
    }
  }

  // flickering

  static int flickerOverlay = 0;
  if(flickerOverlay > 0) {
    if(ofRandomuf() > 0.8) {
      overlay = !overlay;
    }
    flickerOverlay--;
  } else {
    if(ofRandomuf() > 0.998) {
      flickerOverlay = ofRandom(20, 40);
    }
  }

  // static int flickerDeadTrees = 0;
  // if(flickerDeadTrees > 0) {
  //   if(ofRandomuf() > 0.6) {
  //     showDeadTrees = !showDeadTrees;
  //   }
  //   flickerDeadTrees--;
  // } else {
  //   showDeadTrees = false;
  //   if(ofRandomuf() > 0.995) {
  //     flickerDeadTrees = ofRandom(40, 80);
  //   }
  // }
  if(ofRandomuf() > 0.995) {
    showDeadTrees = !showDeadTrees;
  }


}

//--------------------------------------------------------------
void ofApp::draw(){
  //ofBackground(31+sun.strength*1., 31+sun.strength*1.1, 31+sun.strength*1.4);
  ofBackground(0);
  ofPushMatrix();
  if(overlay) {
    // offset by distance to center to center the current tree
    int center = ofGetWidth()/2;
    int xoffset = center - trees[0].root->pos.x;
    ofTranslate(xoffset, 0);
  }

  sun.show();

  if (doTrees) {

    deadTreesFbo.begin();
    ofSetColor(0, 30);
    ofDrawRectangle(0, 0, ofGetWidth(), ofGetHeight());
    if(showDeadTrees) {
      ofPushMatrix();
      if(ofRandomuf() > 0.9) ofTranslate(pow(ofRandomuf(), 3)*14, ofRandomf()*2);
      for (int i = 0; i < deadTrees.size(); i++) {
        deadTrees[i].show(font, totalTime);
      }
      ofPopMatrix();
    }
    deadTreesFbo.end();
    ofSetColor(255, 200);
    deadTreesFbo.draw(0, 0);

    for (int i = 0; i < trees.size(); i++) {
      trees[i].show(font, totalTime);
    }
  }

  if (doRoses) {
    for (int i = 0; i < roseBushes.size(); i++) {
      roseBushes[i].show(font, totalTime);
    }
  }

  // draw the tracked values
  ofSetColor(0, 150);
  for(auto& v : motionTrackingValues) {
    ofDrawEllipse(v.x, v.y, 20, 20);
  }

  ofPopMatrix();

  if (overlay) {
    string fps = "fps: " + to_string_with_precision(ofGetFrameRate(), 1);
    ofSetColor(255);
    font.drawString(fps, 10, 30);

    int nb = 0;
    for (Tree& t : trees) nb += t.branches.size();
    for (RoseBush& t : roseBushes) nb += t.branches.size();
    string numBranches = "branches: " + to_string(nb);
    font.drawString(numBranches, 10, 60);

    int ngp = 0;
    for (Tree& t : trees) ngp += t.growthPoints.size();
    for (RoseBush& t : roseBushes) ngp += t.growthPoints.size();
    string numPoints = "attraction points: " + to_string(ngp);
    font.drawString(numPoints, 10, 90);

    int nl = 0;
    for (Tree& t : trees) nl += t.leaves.size();
    for (RoseBush& t : roseBushes) nl += t.leaves.size();
    string numLeaves = "leaves: " + to_string(nl);
    font.drawString(numLeaves, 10, 120);

    int nr = 0;
    for (RoseBush& t : roseBushes) nr += t.roses.size();
    string numRoses = "roses: " + to_string(nr);
    font.drawString(numRoses, 10, 150);

    int nlb = 0;
    for (auto& t : trees) nlb += t.looseBranches.size();
    string numLooseBranches = "loose branches: " + to_string(nlb);
    font.drawString(numLooseBranches, 10, 180);

    string str;
    str = "humidity:     " + to_string_with_precision(humidity, 1);
    font.drawString(str, 10, 240);
    str = "light:        " + to_string_with_precision(light, 1);
    font.drawString(str, 10, 270);
    str = "temperature1: " + to_string_with_precision(temperature1, 1);
    font.drawString(str, 10, 300);
    str = "temperature2: " + to_string_with_precision(temperature2, 1);
    font.drawString(str, 10, 330);
    str = "fluorescence: " + to_string_with_precision(fluorescence, 1);
    font.drawString(str, 10, 360);

    plotter.drawCustomPlot("energy", 0, ofGetHeight()*0.8, ofGetWidth()*0.4, ofGetHeight()*0.2);
    plotter.drawCustomPlot("points", ofGetWidth()*0.6, ofGetHeight()*0.8, ofGetWidth()*0.4, ofGetHeight()*0.2);
    plotter.drawCustomPlot("energyHistory", 0, ofGetHeight()*0.6, ofGetWidth()*0.4, ofGetHeight()*0.2);
    plotter.drawCustomPlot("pointsHistory", ofGetWidth()*0.6, ofGetHeight()*0.6, ofGetWidth()*0.4, ofGetHeight()*0.2);
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
    case 'x':
      static float thresh = 1;
      for(auto& t : trees) {
        t.simplifyTree(thresh);
      }
      thresh += 0.5;
      break;
    case OF_KEY_UP:
      for(auto& t : trees) {
        //t.spawnGrowthPointsAroundEdge(20, 20, 20);
        //t.w += 30;
        //t.h += 20;
        // t.w *= 1.05;
        // t.h *= 1.05;
        //t.spawnGrowthPoints(100);

        t.growBigger();
      }
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
  if(ofRandomuf() > 0.5) {
    GrowthPoint temp = GrowthPoint(glm::vec2(x + ofRandomf()*20, y + ofRandomf()*20));
    for(auto& t : trees) {
      t.growthPoints.push_back(temp);
    }
  }

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

void ofApp::receiveOscMessages() {
  while(oscReceiver.hasWaitingMessages()) {
    ofxOscMessage m;
    oscReceiver.getNextMessage(m);
    cout << "address: " << m.getAddress() << " message: " << m << endl;

    // parse the message
    if (m.getAddress() == "/coords"  )
		{
      // convert from 320 x 240 coordinates to the canvas
      float xratio = float(ofGetWidth())/320.0;
      float yratio = float(ofGetHeight())/240.0;

      for(int i = 0; i < motionTrackingValues.size(); i++) {
        // values are sent as x1 y1 x2 y2 etc
        motionTrackingValues[i] = glm::vec2(m.getArgAsFloat(i*2) * xratio, m.getArgAsFloat(i*2 + 1) * yratio);
      }
		}

    else if (m.getAddress() == "/temperature"  )
		{
			temperature1 = m.getArgAsFloat(0);
		}
    else if (m.getAddress() == "/humidity"  )
		{
			humidity = m.getArgAsFloat(0);
		}
    else if (m.getAddress() == "/light"  )
		{
			light = m.getArgAsFloat(0);
		}
    else if (m.getAddress() == "/fluorescence"  )
		{
			fluorescence = m.getArgAsFloat(0);
		}
  }
}

void ofApp::readSerialData() {
  while(serial.available()) {
    char byte = serial.readByte();
    //cout << "byte: " << byte << endl;
    if ( byte == OF_SERIAL_NO_DATA )
      printf("no data was read");
    else if ( byte == OF_SERIAL_ERROR )
      printf("an error occurred");
    else if (byte == 13) parseSerialData();
    else {
      // add the byte to the current string
      currentMessage << byte;
    }
  }
}

// function from https://stackoverflow.com/questions/4654636/how-to-determine-if-a-string-is-a-number-with-c
bool is_number(const std::string& s)
{
    return !s.empty() && std::find_if(s.begin(),
        s.end(), [](char c) { return !std::isdigit(c); }) == s.end();
}

void ofApp::parseSerialData() {
  //cout << currentMessage.str() << endl;
  vector<float> values(5, -1);
  string v;
  int i = 0;
  while(currentMessage >> v && i < 5) {
    if(is_number(v)) {
      values[i] = stoi(v);
      i++;
    } else {
      cout << "Error: serial message was not a number: " << v << endl;
    }
  }
  if(i == 5) {
    // all values were registered
    humidity = values[3];
    light = values[1];
    temperature1 = values[0]/2.0;
    temperature2 = values[2];
    fluorescence = values[4];
  }

  currentMessage.str("");
  currentMessage.clear();
}

void ofApp::makeNewTree() {
  trees[0].killTree();
  deadTrees.push_back(trees[0]);
  trees.clear();
  int x = ofGetWidth()*0.5 + ofRandom(ofGetWidth()*-0.4, ofGetWidth()*0.4);
  Tree newTree = Tree(glm::vec2(x, ofGetHeight()));
  newTree.visualType = static_cast<TreeVisual>(ofRandom((int)TreeVisual::LAST));
  newTree.branchType = static_cast<BranchVisual>(ofRandom((int)BranchVisual::RANDOM));
  trees.push_back(newTree);

  simplificationThresh = 1;
}
