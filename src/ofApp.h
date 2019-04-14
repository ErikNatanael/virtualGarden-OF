#pragma once

#include "ofMain.h"
#include "ofxOsc.h"
#include "ofxPlotter.h"

#include "Tree.h"
#include "Rose.h"
#include "RoseBush.h"
#include "Sun.h"

#define DEBUG

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		void receiveOscMessages();
		void readSerialData();
		void parseSerialData();
		void makeNewTree();

		ofTrueTypeFont font;

		vector<Tree> trees;
		vector<Tree> deadTrees; // dead trees are sometimes shown, but never updated
		ofFbo deadTreesFbo;

		float totalTime = 0;
		float dt = 0.01;
		double lastTime = 0;

		vector<Rose> roses;
		vector<RoseBush> roseBushes;
		Sun sun;

		bool grow = true;

		bool doTrees = true;
		bool doRoses = false;
		bool pause = false;
		bool showDeadTrees = true;

		ofFbo deadTreesCanvasFbo;

		glm::vec2 wind;

		ofImage grabImg;

		ofxOscReceiver oscReceiver;
		ofSerial serial;
		stringstream currentMessage;
		vector<glm::vec2> motionTrackingValues;
		int motionTrackingPoints = 20;
		float temperature1 = 0;
		float temperature2 = 0;
		int humidity = 0;
		int light = 0;
		float fluorescence = 0;

		float resetTreeTime = 0;
		float lastTreeReset = 0;
		float simplificationThresh = 1;

		ofxPlotter plotter;

};
