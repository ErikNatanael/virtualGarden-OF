
ArrayList<Tree> trees;

float time = 0;
float dt = 0.01;
double lastTime = 0;

ArrayList<Rose> roses;
ArrayList<RoseBush> roseBushes;
Sun sun;

boolean grow = true;

boolean doTrees = true;
boolean doRoses = true;
boolean overlay = true;
boolean pause = false;

PVector wind;

void setup() {
  size(1920, 1080);
  //fullScreen();

  trees = new ArrayList<Tree>();
  int numTrees = 2;
  for (int i = 0; i < numTrees; i++) {
    int x = (width/(numTrees+1))*(i+1);
    trees.add(new Tree(new PVector(x, height)));
  }

  roses = new ArrayList<Rose>();
  /*for(int i = 0; i < 5; i++ ) {
   roses.add(new Rose(
   new PVector(random(width*0.2, width*0.8), random(height*0.7, height*0.9))));
   }*/

  roseBushes = new ArrayList<RoseBush>();
  for (int i = 0; i < 10; i++ ) {
    roseBushes.add(new RoseBush(
      new PVector(random(width*0.2, width*0.8), height)));
  }

  wind = new PVector(0, 0);
  sun = new Sun(new PVector(width/2, 0), 10.);
}

void draw() {

  double currentTime = millis()*0.001;
  dt = (float)(currentTime - lastTime);
  lastTime = currentTime;
  time += dt;
  
  if(!pause) {
    sun.update();
  }
  
  background(color(41+sun.strength*1., 41+sun.strength*1.1, 41+sun.strength*1.4));

  
  sun.show();

  if (doTrees) {
    for (int i = 0; i < trees.size(); i++) {
      Tree tree = trees.get(i);
      tree.show();
      if (grow && !pause) tree.grow();
      if(!pause)tree.update(dt);
    }
  }

  if (doRoses) {
    for (int i = 0; i < roseBushes.size(); i++) {
      if(!pause) if (grow) roseBushes.get(i).grow();
      if(!pause) roseBushes.get(i).update(dt);
      roseBushes.get(i).show();
    }
  }

  if (overlay) {
    String fps = "fps " + frameRate;
    fill(255);
    textSize(32);
    text(fps, 10, 30); 

    int nb = 0;
    for (Tree t : trees) nb += t.branches.size();
    for (RoseBush t : roseBushes) nb += t.branches.size();
    String numBranches = "number of branches: " + nb;
    text(numBranches, 10, 60);

    int ngp = 0;
    for (Tree t : trees) ngp += t.growthPoints.size();
    String numPoints = "number of points: " + ngp;
    text(numPoints, 10, 90);

    int nl = 0;
    for (Tree t : trees) nl += t.leaves.size();
    for (RoseBush t : roseBushes) nl += t.leaves.size();
    String numLeaves = "number of leaves: " + nl;
    text(numLeaves, 10, 120);

    int nr = 0;
    for (RoseBush t : roseBushes) nr += t.roses.size();
    String numRoses = "number of roses: " + nr;
    text(numRoses, 10, 150);
  }
}

void mousePressed() {
  //trees.add(new Tree(new PVector(random(width*0.1, width*0.9), height)));
}

void mouseDragged() {
  //growthPoints.add(new GrowthPoint(new PVector(random(100)+mouseX-50, random(100)+mouseY-50)));
}

void keyPressed() {
  if (key == 'g') grow = !grow;
  if (key == 'o') overlay = !overlay;
  if (key == 's') saveFrame(timestamp("Virtual_garden", ".jpg"));
  if (key == 'p') pause = !pause;
}

static final String timestamp(final String name, final String ext) {
  return name + "-" + year() + nf(month(), 2) + nf(day(), 2) +
    "-" + nf(hour(), 2) + nf(minute(), 2) + nf(second(), 2) + ext;
}
