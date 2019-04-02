#ifndef TREE_H_
#define TREE_H_


#include "ofMain.h"
#include "GrowthPoint.h"
#include "Branch.h"
#include "Leaf.h"
#include "globals.h"
#include "Sun.h"

class Tree {
public:

  int maxDist = 50;
  int minDist = 10;

  branch_ptr root;
  vector<branch_ptr> branches;
  vector<branch_ptr> endSegmentBranches;
  vector<branch_ptr> looseBranches;       // dead branches that have fallen off
  vector<leaf_ptr > leaves;
  branch_ptr currentBranch;
  vector<GrowthPoint> growthPoints;

  float leafEnergyCost = 10;
  float branchEnergyCost = 15.;
  float energy = 0.;
  float maxEnergy;
  int maxLeafAmount = 100;
  int energyReserveRequirement = 0;
  float  growBiggerRequirement = 100;
  float passiveEnergyGain = 10.5;

  int startingGrowthPoints = 300;
  float w = ofRandom(50, 150);
  float h = ofRandom(100, 200);
  float hOffset = ofGetHeight()*0.4;
  float rounding = 0.9;
  float thicknessMul = 1.;

  glm::vec2 growthSpeed; // how much the tree will grow in the x and y direction per growth tick

  int lastNumPoints = 0;
  int samePointsFrames = 0;

  bool trunkFinished = false;
  bool startPointsSpawned;

  bool doLeaves = true;


  Tree(glm::vec2 rootPos) {

    root = branch_ptr( new Branch(NULL,
      rootPos, // position
      glm::vec2(0, -1)) // direction
    );

    branches.push_back(root);
    currentBranch = root;
    startingGrowthPoints = floor((w*h)*0.005);
    rounding = ofRandom(0.7)+.3;
    hOffset = ofRandom(100)+50;
    growthSpeed = glm::vec2(ofRandom(5, 15), ofRandom(5, 15));
    energy = branchEnergyCost * 200;
    maxEnergy = energy * 1.5;

    startPointsSpawned = false;
  }

  void spawnGrowthPoints(int numGrowthPoints) {
    for (int i = 0; i<numGrowthPoints; i++) {
      float y = ofRandomuf();
      float xRound = ofRandom(sin(y*PI)) + (1-sin(y*PI))*0.5;
      float x = ofRandomuf()*(1-rounding) + xRound*rounding;
      glm::vec2 pos = glm::vec2(x*w-(w/2)+root->pos.x, ofGetHeight() - (y*h+hOffset));
      growthPoints.push_back(GrowthPoint(pos));
    }
  }

  void spawnGrowthPointsAroundEdge(int numGrowthPoints, int width, int height) {
    for (int i = 0; i<numGrowthPoints; i++) {
      float y = ofRandomuf();
      float xRound = ofRandom(sin(y*PI)) + (1-sin(y*PI))*0.5;
      float x = ofRandomf();
      float xside = x > 0 ? w/2 : w/-2;
      float xpos = root->pos.x + abs(x) * width + xside;
      float ypos = ofGetHeight() - (y*(h+height)+hOffset);
      glm::vec2 pos = glm::vec2(xpos, ypos);
      growthPoints.push_back(GrowthPoint(pos));
    }
    w += width;
    h += height;
  }

  void grow() {
    if (!trunkFinished) growTrunk();

    else {
      if(energy > branchEnergyCost * 5) growBranches();
      growLeaves(1.);
    }
  }

  void growTrunk() {
    if (!startPointsSpawned) {
      startPointsSpawned = true;
      spawnGrowthPoints(startingGrowthPoints);
    }
    bool found = false;
    //while (!found) {
    for (int i = 0; i < growthPoints.size(); i++) {
      float dist = glm::distance(currentBranch->pos, growthPoints[i].pos);
      if (dist < maxDist) {
        found = true;
      }
    }
    if (!found) {
      branch_ptr newBranch = currentBranch->next(); // create new branch

      currentBranch = newBranch;
      branches.push_back(newBranch);
    } else {
      trunkFinished = true;
    }
    //}
  }


  void growBranches() {

    if(energy > energyReserveRequirement + branchEnergyCost*5) {
      for (int i = 0; i < growthPoints.size(); i++) {
        GrowthPoint* l = &growthPoints[i];
        branch_ptr closestBranch = NULL;
        float smallestDistanceFound = 1000000.;
        for (int j = 0; j < branches.size(); j++) {
          branch_ptr b = branches[j];
          if(b->canGrowBranch()) {
            float dist = glm::distance(l->pos, b->pos);
            if (dist < minDist) { // it doesn't count
              l->reached = true;
              closestBranch = NULL;
              smallestDistanceFound = dist;
              break; // we can break because this growth point has been reached
            } else if (dist > maxDist) {
            } else if (closestBranch == NULL ||
              dist < smallestDistanceFound) {
              closestBranch = b;
              smallestDistanceFound = dist;
            }
          }
        }
        if (closestBranch != NULL) {
          glm::vec2 newDir = l->pos - closestBranch->pos;
          newDir += glm::vec2(ofRandomuf()*4, ofRandomuf()*4); // add a small random value to avoid getting stuck between two points
          newDir = glm::normalize(newDir) * 2.;
          closestBranch->direction += newDir;
          closestBranch->count++;
        }
      }

      checkIfStuck();



      for (int i = branches.size()-1; i>=0; i--) {
        branch_ptr b = branches[i];
        if (b->count > 0 && b->canGrowBranch()) {
          b->direction /= b->count + 1;
          if (energy > energyReserveRequirement && energy > branchEnergyCost) {
            branch_ptr newBranch = b->next();
            newBranch->thicknessGrowth *= thicknessMul;
            // reject branches that are too close
            if(glm::distance(b->pos, newBranch->pos) > 2.) {
              branches.push_back(newBranch);
              endSegmentBranches.push_back(newBranch);
              energy -= branchEnergyCost;
            }
          }
        }
        b->resetBranch();
      }
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

  void checkIfStuck() {
    if (lastNumPoints == growthPoints.size()) {
      samePointsFrames += 1;
    } else {
      lastNumPoints = growthPoints.size();
      samePointsFrames = 0;
    }

    if (samePointsFrames > 60) {
      // if the number of points stay the same we have gotten stuck
      // therefore, remove all points
      for (int i = growthPoints.size()-1; i >= 0; i--) {
        growthPoints.erase(growthPoints.begin() + i);
      }
    }
  }

  void growLeaves(float probabilityMultiplier) {
    float leafProbability =  1.- endSegmentBranches.size()*0.002*probabilityMultiplier;
    if (ofRandom(1.0) > leafProbability && energy > leafEnergyCost && leaves.size() < maxLeafAmount) {
      if (endSegmentBranches.size() > 0) {
        branch_ptr b = endSegmentBranches[floor(ofRandom(endSegmentBranches.size()))];
        // find branch by traversing from end branches
        int skipsFromEnd = 0; //floor((1.-pow(ofRandom(1.), 2.))*endSegmentBranches.size()*0.1);
        while (skipsFromEnd > 0) {
          b = b->parent;
          skipsFromEnd--;
        }
        if (b->thickness < 10.) {

          leaf_ptr newLeaf = b->addLeaf();
          leaves.push_back(newLeaf);
          energy -= leafEnergyCost;
        }
      }
    }
  }

  void growBigger() {
    w += growthSpeed.x;
    h += growthSpeed.y;
    int numPoints = ofClamp(floor((w*h)*0.003), 10, 50);
    spawnGrowthPoints(numPoints);
  }

  ofColor branchColorByIndex(int i) {
    return ofColor(i*0.2, ofClamp(i*0.2-100, 0, 255), ofClamp(i*0.2-200, 0, 255));
  }

  void show(ofTrueTypeFont font, float totalTime) {

    for (int i = 0; i < branches.size(); i++) {

      // branches[i]->show(branchColorByIndex(i), totalTime);
      branches[i]->show(branches[i]->calculateBranchColor(), totalTime);
    }

    for (int i = 0; i < looseBranches.size(); i++) {

      looseBranches[i]->show(ofColor(0), totalTime);
    }

    if (overlay) {
      string eg = "energy: " + to_string(energy);
      ofSetColor(255);
      font.drawString(eg, root->pos.x-100, ofGetHeight()-(h+hOffset));

      for (int i = 0; i < growthPoints.size(); i++) {
        growthPoints[i].show();
      }
    }

    // branch heatmap (shows where the most amount of branches are)
    #ifdef DEBUG
    ofSetColor(255, 1, 2, 2);
    for(branch_ptr b : branches) {
      ofDrawEllipse(b->pos.x, b->pos.y, 10, 10);
    }
    #endif

    // show latest branch
    // Branch b = branches.get(branches.size()-1);
    // ellipse(b.pos.x, b.pos.y, 20, 20);
  }

  void simplifyTree(float distanceThreshold) {
    for(int i = 0; i < 100; i++) {
      root->simplifyChildren(distanceThreshold); // recursively go through the tree structure from the root

      // remove branches that have been deleted
      for(int i = branches.size()-1; i>=0; i--) {
        if(branches[i]->isDeleted) branches.erase(branches.begin()+i);
      }
    }

  }

  // doesn't work
  void simplifyTreeEveryOther() {
    for(int i = 2; i < branches.size(); i+=2) {
      branches[i]->parent = branches[i]->parent->parent;
      branches[i]->originalRelativePos += branches[i]->parent->originalRelativePos;
    }
    for(int i = 1; i < branches.size(); i+=2) {
      branches.erase(branches.begin() + i);
      i--;
    }

  }

  void checkBranchSamePosition() {
    int num = 0;
    for(int i = 0; i < branches.size(); i++) {
      auto ba = branches[i];
      for(int j = i + 1; j < branches.size(); j++) {
        auto bb = branches[j];
          if(ba->pos.x == bb->pos.x &&
            ba->pos.y == bb->pos.y) {
              num++;
            }
      }
    }
    cout << "Number of branches: " << branches.size() << endl;
    cout << "Number of branches at the same position as another branch: " << num << endl;
  }
  void checkBranchSameParent() {
    int num = 0;
    for(int i = 0; i < branches.size(); i++) {
      auto ba = branches[i];
      for(int j = i + 1; j < branches.size(); j++) {
        auto bb = branches[j];
          if((ba->parent) == (bb->parent))
              num++;
      }
    }
    cout << "Number of branches with the same parents: " << num << endl;
  }

  void moveAllBranches(glm::vec2 move) {
    for(branch_ptr b : branches) {
      b->pos += move;
    }
  }



  void update(float dt, Sun sun) {
    maxLeafAmount = endSegmentBranches.size();
    energyReserveRequirement = branches.size()*0.5;
    growBiggerRequirement = max(energyReserveRequirement*1.5, 100.0);

    if(trunkFinished) maxEnergy = max(float(branches.size()*2), 1000.0f);
    if(energy > maxEnergy) energy = maxEnergy;


    if (doLeaves) {
      for (int i = leaves.size()-1; i >= 0; i--) {
        auto lPtr = leaves[i];
        energy += lPtr->getEnergy(sun);
        if (lPtr->dead) leaves.erase(leaves.begin() + i);
      }
    }

    energy += passiveEnergyGain;

    for (int i = 0; i < branches.size(); i++) {
      branches[i]->update(dt);
    }
    // remove dead branches from the tree
    for(auto& b : branches) {
      if(b->isDead) {
        vector<branch_ptr> killedBranches;
        b->killBranch(killedBranches);
        looseBranches.insert(looseBranches.end(), killedBranches.begin(), killedBranches.end());
      }
    }
    branches.erase(
      std::remove_if(
          branches.begin(),
          branches.end(),
          [](branch_ptr const & p) { return p->isDead; }
      ),
      branches.end()
    );
    // update the loose branches (dead branches)
    for (int i = looseBranches.size()-1; i >= 0; i--) {
      looseBranches[i]->update(dt);
      looseBranches[i]->deadStartPos += glm::vec2(0, -1);
    }
    looseBranches.erase(
      std::remove_if(
          looseBranches.begin(),
          looseBranches.end(),
          [](branch_ptr const & p) { return p->deadStartPos.y < -100; }
      ),
      looseBranches.end()
    );

    // fill up the root node and let it propagate through the tree
    float energySpent = root->fillHP(energy);
    energy -= energySpent;

    //if(energy > growBiggerRequirement && trunkFinished && growthPoints.size() < 5) growBigger();

    // update GrowthPoints
    for(int i = growthPoints.size()-1; i >= 0; i--) {
      growthPoints[i].update(dt);
      if(growthPoints[i].expired) growthPoints.erase(growthPoints.begin() + i);
    }
  }
};

#endif /* end of include guard: TREE_H_ */
