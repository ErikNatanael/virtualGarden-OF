#pragma once

#include "ofMain.h"
#include "Tree.h"
#include "Rose.h"
#include "RoseBush.h"
#include "Sun.h"

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

		ofTrueTypeFont font;

		vector<Tree> trees;

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

		glm::vec2 wind;

		ofImage grabImg;
};
