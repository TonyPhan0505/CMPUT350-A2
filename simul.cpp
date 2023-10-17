#include <cassert>
#include <cstdlib>
#include <cmath>
#include <iostream>
#include <sstream>
#include <cstring>
#include <chrono>
#include <GL/glut.h>
#include "World.h"
#include "Unit.h"
#include "Marine.h"
#include "Tank.h"

using namespace std;

class GfxWorld;

// global => glut functions can access it
GfxWorld *world = nullptr;
int delay = 100; // frame delay in ms

class GfxWorld : public World
{
public:

  GfxWorld(double w, double h, int seed)
    : World(w, h, seed)
  { }
  
  // save attack for rendering it later
  void attack_hook(const Unit & attacker, const Unit & attacked)
  {
    attacks.push_back(&attacker);
    attacks.push_back(&attacked);
  }
  
  vector<const Unit*> attacks;
};


void OnDisplay()
{
  glClear(GL_COLOR_BUFFER_BIT);
  glPushAttrib(GL_ALL_ATTRIB_BITS);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, world->width, world->height, 0, 0, 1);
  glMatrixMode(GL_MODELVIEW);
  // (0,0) top left, (width,height) bottom right

  glutPostRedisplay();

#if 0
  // render walls
  glBegin(GL_LINE_STRIP);
  glColor3f(1, 1, 1); 
  glVertex2f(1, 1);
  glVertex2f(world->width-1, 1);
  glVertex2f(world->width-1, world->height-1);
  glVertex2f(1, world->height-1);
  glVertex2f(1, 1);
  glEnd();
#endif

  // render units
  for(size_t i=0; i < world->units.size(); ++i) {
    Unit &u = *world->units[i];
    // cout << "u " << u.pos.x << " " << u.pos.y << " " << u.radius << endl;

    if (u.team == RED) {
      glColor3f(1, 0.4, 0);
    } else {
      glColor3f(0, 1, 1);
    }

    // approximate circle
    glBegin(GL_LINE_STRIP);
    for(size_t j=0; j <= 16; ++j) {
      const double angle((j%16)*M_PI*2/16);
      glVertex2f(u.pos.x + cos(angle) * u.radius,
                 u.pos.y + sin(angle) * u.radius);
      // cout << pos.x + cos(angle) * radius << " " << pos.y + sin(angle)*radius << endl;
    }
    glEnd();
		
    // print the hp inside the unit
		
    glRasterPos2f(u.pos.x-8, u.pos.y+4);
    stringstream hpsStream;
    hpsStream << u.hp;
    const string temp = hpsStream.str();
    const char *hps = temp.c_str();
    int hpsLen = strlen(hps);

    for(int i = 0; i < hpsLen; i++) {
      glutBitmapCharacter(GLUT_BITMAP_8_BY_13, hps[i]);
    }
  }

  // render attacks
  // todo: draw arrow heads to indicate who is attacking whom
  size_t j=0;
  assert((world->attacks.size() & 1) == 0);
  while (j < world->attacks.size()) {
    glBegin(GL_LINE_STRIP);
    glColor3f(1, 1, 1);
    const Unit &u = *world->attacks[j];
    const Unit &v = *world->attacks[j+1];
    glVertex2f(u.pos.x, u.pos.y);
    glVertex2f(v.pos.x, v.pos.y);
    glEnd();
    //cout << "plot: " << sqrt(world->distance2(u, v)) << " " << u.attack_radius + v.radius << endl;
    j += 2;
  }

  glPopAttrib();
  glutSwapBuffers();
}

void timer_function(int)
{
  cout << "." << flush;
  world->attacks.clear();
  world->step();

  int red_score = world->red_score();
  if (red_score >= 0) {
    cout << "game over: ";
    if (red_score == 2) {
      cout << "RED wins" << endl;
    } else if (red_score == 1) {
      cout << "draw" << endl;
    } else {
      cout << "BLUE wins" << endl;
    }
    delete world;
    exit(0);
  }
  glutTimerFunc(delay, timer_function, 0); // 10 frames/sec
}



int main(int argc, char *argv[])
{
  if (argc != 1 && argc != 10) {
    cerr << "usage: " << argv[0] << " width height delay seed marines tanks redpol bluepol bounce" << endl;
    cerr << "       delay in ms, seed=0: use time, policies: 0 weakest/1 closest/2 most dangerous" << endl;
    exit(1);
  }

  double width  = 700;
  double height = 700;

  delay = 100;     // frame delay in ms
  int seed = 0;    // for rng, 0: use time

  int n_marines = 100;
  int n_tanks   = 100;

  AttackPolicy red_policy = ATTACK_MOST_DANGEROUS;
  AttackPolicy blue_policy = ATTACK_MOST_DANGEROUS;  

  bool bounce = true;

  if (argc == 10) {
    // todo : error handling
    int i = 1;
    width = atoi(argv[i++]);
    height = atoi(argv[i++]);
    delay = atoi(argv[i++]);
    seed = atoi(argv[i++]);
    n_marines = atoi(argv[i++]);
    n_tanks = atoi(argv[i++]);
    red_policy = (AttackPolicy)atoi(argv[i++]);
    blue_policy = (AttackPolicy)atoi(argv[i++]);
    bounce = atoi(argv[i++]);
  }

  if (seed == 0) {
    // rng seed dependent on wallclock time
    unsigned long long millis =
      std::chrono::duration_cast<std::chrono::milliseconds>
      (std::chrono::system_clock::now().time_since_epoch()).count();

    seed = millis;
  }

  cout << "width: "    << width     << endl;
  cout << "height: "   << height    << endl;
  cout << "delay: "    << delay     << endl;
  cout << "seed: "     << seed      << endl;
  cout << "marines: "  << n_marines << endl;
  cout << "tanks: "    << n_tanks   << endl;
  cout << "redpol: "   << apol2str(red_policy) << endl;
  cout << "bluepol: "  << apol2str(blue_policy) << endl;
  cout << "bounce:  "  << bounce << endl;

  // set up world
  world = new GfxWorld(width, height, seed);

  // populate with units

  // get unit sizes - a bit awkward, could be data members
  Marine *m = new Marine(RED, Vec2(0, 0), ATTACK_WEAKEST, false);
  double mr = m->radius;
  delete m;
  Tank *t = new Tank(RED, Vec2(0, 0), ATTACK_WEAKEST, false);
  double tr = t->radius;
  delete t;

  AttackPolicy red_marine_policy  = red_policy;
  AttackPolicy blue_marine_policy = blue_policy;
  AttackPolicy red_tank_policy    = red_policy;
  AttackPolicy blue_tank_policy   = blue_policy;

  // create marines
  for (int i=0; i < n_marines; ++i) {
    Unit *u = new Marine(RED, world->rnd_pos(mr), red_marine_policy, bounce);
    u->heading = world->rnd_heading();
    u->current_speed = u->max_speed;
    world->units.push_back(u);

    // mirrored    
    Unit *v = new Marine(BLUE, world->mirror(u->pos), blue_marine_policy, bounce);
    v->heading = Vec2(-u->heading.x, -u->heading.y); 
    v->current_speed = v->max_speed;
    world->units.push_back(v);
  }

  // create tanks
  for (int i=0; i < n_tanks; ++i) {
    Unit *u = new Tank(RED, world->rnd_pos(tr), red_tank_policy, bounce);
    u->heading = world->rnd_heading();
    u->current_speed = u->max_speed;
    world->units.push_back(u);

    // mirrored    
    Unit *v = new Tank(BLUE, world->mirror(u->pos), blue_tank_policy, bounce);
    v->heading = Vec2(-u->heading.x, -u->heading.y); 
    v->current_speed = v->max_speed;    
    world->units.push_back(v);
  }

  glutInit(&argc,argv);
  glutInitWindowSize(width, height);
  glutInitDisplayMode(GLUT_DOUBLE);
  glutCreateWindow("CMPUT 350 - Assignment Example App");
  glutDisplayFunc(OnDisplay);
  glutTimerFunc(1, timer_function,0);
  glutMainLoop();
  return 0;
}
