#pragma once
#include "ofMain.h"
#include "Tree.h"
#include "Branch.h"
#include "Rose.h"
#include "Sun.h"

class RoseBush : public Tree {
public:

  vector<Rose> roses;

  RoseBush(glm::vec2 rootPos) : Tree(rootPos) {
    maxDist = 50;
    minDist = 2; // don't make smaller than 2 (branch growing can get stuck)

    startingGrowthPoints = 100;
    h = ofRandom(150, 400);
    w = ofRandom(70, 20);
    hOffset = 0;
    rounding = 0.7;
  }

  void grow() {
    if (!startPointsSpawned) {
      startPointsSpawned = true;
      spawnGrowthPoints();
    }
    growBranches();
  }

  void growBranches() {

    for (int i = 0; i < growthPoints.size(); i++) {
      GrowthPoint* l = &growthPoints[i];
      branch_ptr closestBranch = NULL;
      float smallestDistanceFound = 1000000.;
      for (int j = 0; j < branches.size(); j++) {
        branch_ptr b = branches[j];
        float dist = glm::distance(l->pos, b->pos);
        if (dist < minDist) { // it doesn't count
          l->reached = true;
          closestBranch = NULL;
          break;
        } else if (dist > maxDist) {
        } else if (closestBranch == NULL ||
          dist < smallestDistanceFound) {
          closestBranch = b;
          smallestDistanceFound = dist;
        }
      }
      if (closestBranch != NULL) {
        glm::vec2 newDir = l->pos - closestBranch->pos;
        newDir += glm::vec2(ofRandom(4.)-2, ofRandom(4.)-2); // add a small random value to avoid getting stuck between two points
        newDir *= 10;
        float angle = atan2(-1*newDir.y, newDir.x);
        // constrain the angle
        //if(angle < PI/6 || angle > PI*1.5) angle = PI/6;
        //if(angle > (5*PI)/6) angle = (5*PI)/6;
        newDir = glm::vec2(cos(angle), -1*sin(angle));
        closestBranch->direction += newDir;
        closestBranch->count++;
      }
    }

    // remove all reached growthPoints
    for (int i = growthPoints.size()-1; i >= 0; i--) {
      if (growthPoints[i].reached) {
        growthPoints.erase(growthPoints.begin() + i);
      }
    }

    float leafProbability =  1.- endSegmentBranches.size()*0.0001;
    if (ofRandomuf() > leafProbability) {
      if(endSegmentBranches.size() > 0) {
        branch_ptr b = endSegmentBranches[floor(ofRandom(endSegmentBranches.size()))];
        // find branch by traversing from end branches
        /*int skipsFromEnd = floor((1.-pow(random(1.), 2.))*endSegmentBranches.size()*0.1);
        while (skipsFromEnd > 0) {
          if (b!=null) b = b.parent;
          skipsFromEnd--;
        }*/
        int maxRoses = floor(branches.size()*0.05);
        if(roses.size() < maxRoses) {
          Rose newRose = Rose(b);
          roses.push_back(newRose);
        }


      }
    }

    checkIfStuck();

    //growLeaves(0.2);

    for (int i = branches.size()-1; i>=0; i--) {
      branch_ptr b = branches[i];
      if (b->count > 0) {
        b->direction /=  b->count + 1;
        branch_ptr newBranch = b->next();
        newBranch->thicknessGrowth = ofRandomuf()*0.002;
        branches.push_back(newBranch);
        endSegmentBranches.push_back(newBranch);
      }
      b->resetBranch();
    }

    // remove branches that are no longer end segments
    for (int i = endSegmentBranches.size()-1; i>=0; i--) {
      if (!endSegmentBranches[i]->isEndSegment) endSegmentBranches.erase(endSegmentBranches.begin() + i);
    }
    // remove all reached growthPoints
    for (int i = growthPoints.size()-1; i >= 0; i--) {
      if (growthPoints[i].reached) {
        growthPoints.erase(growthPoints.begin() + i);
      }
    }
  }

  void show(ofTrueTypeFont font, float totalTime) {
    Tree::show(font, totalTime);

    for (int i = 0; i < roses.size(); i++) {
      roses[i].show();
    }

  }

  ofColor branchColorByIndex(int i) {
    return ofColor(i*0.3, i*0.2-100, i*0.2-200);
  }

  void update(float dt, Sun sun) {
    for (int i = roses.size()-1; i >= 0; i--) {
      roses[i].update(dt);
      if(roses[i].dead) roses.erase(roses.begin() + i);
    }

    for (int i = 0; i < branches.size(); i++) {
      branches[i]->update();
    }
    for (int i = 0; i < leaves.size(); i++) {
      leaves[i].update(dt);
    }
  }
};
