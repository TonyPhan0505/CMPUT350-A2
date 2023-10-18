#include <cassert>
#include <cstdlib>
#include <cmath>
#include <iostream>
#include <sstream>
#include <cstring>
#include <chrono>
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

int main()
{
  /* ------------- Set up experiment ------------- */
  double width  = 700;
  double height = 700;
  int n_marines = 20;
  int n_tanks   = 20;
  AttackPolicy red_policies[2] = {ATTACK_CLOSEST, ATTACK_WEAKEST};
  AttackPolicy blue_policies[2] = {ATTACK_WEAKEST, ATTACK_MOST_DANGEROUS};  
  bool bounce = true;
  // rng seed dependent on wallclock time
  unsigned long long millis =
    std::chrono::duration_cast<std::chrono::milliseconds>
    (std::chrono::system_clock::now().time_since_epoch()).count();
  int seed = millis;
  /* ----------------------------------------------------- */


  /* ------------------- Experiment --------------------- */
  cout << "------------------ Experiment --------------------" << endl;
  int time = 1;
  while (time <= 100) {
    cout << "\n" << endl;  // blank line
    cout << "********** Time: " << time << endl;
    world = new GfxWorld(width, height, seed);
    if (time == 1) {
      cout << "********** Settings:" << endl;
      cout << "> width: " << width << endl;
      cout << "> height: " << height << endl;
      cout << "> seed: " << seed << endl;
      cout << "> marines: " << n_marines << endl;
      cout << "> tanks: " << n_tanks   << endl;
      cout << "> bounce:  "  << bounce << endl;
    }
    int rp_index = 0;
    int bp_index = 0;
    while (rp_index <= 1) {
      while (bp_index <= 1) {
        (world->units).clear();
        if (blue_policies[bp_index] != red_policies[rp_index]) {
          AttackPolicy red_policy = red_policies[rp_index];
          AttackPolicy blue_policy = blue_policies[bp_index];

          cout << "> Scenario:" << endl;
          cout << "red " << "(" << apol2str(red_policy) << ")" << " vs " << "blue " << "(" << apol2str(blue_policy) << ")" << endl;

          // get unit sizes - a bit awkward, could be data members
          Marine *m = new Marine(RED, Vec2(0, 0), ATTACK_WEAKEST, false);
          double mr = m->radius;
          delete m;
          Tank *t = new Tank(RED, Vec2(0, 0), ATTACK_WEAKEST, false);
          double tr = t->radius;
          delete t;

          // create marines
          for (int i=0; i < n_marines; ++i) {
            Unit *u = new Marine(BLUE, world->rnd_pos(mr), blue_policy, bounce);
            u->heading = world->rnd_heading();
            u->current_speed = u->max_speed;
            world->units.push_back(u);

            // mirrored    
            Unit *v = new Marine(RED, world->mirror(u->pos), red_policy, bounce);
            v->heading = Vec2(-u->heading.x, -u->heading.y); 
            v->current_speed = v->max_speed;
            world->units.push_back(v);
          }

          // create tanks
          for (int i=0; i < n_tanks; ++i) {
            Unit *u = new Tank(BLUE, world->rnd_pos(tr), blue_policy, bounce);
            u->heading = world->rnd_heading();
            u->current_speed = u->max_speed;
            world->units.push_back(u);

            // mirrored    
            Unit *v = new Tank(RED, world->mirror(u->pos), red_policy, bounce);
            v->heading = Vec2(-u->heading.x, -u->heading.y); 
            v->current_speed = v->max_speed;    
            world->units.push_back(v);
          }

          cout << "@@World size: " << (world->units).size() << endl;

          // while game not over yet
          int count = 1;
          int status = world -> red_score();
          while ((world->units).size() > 0 && (status == -1)) {
            // make the units move and fight each other
            world -> step();
            status = world -> red_score();
            cout << "passed simulation frame " << count << endl;
            count += 1;
          }
          if (status == 2) {
            cout << "********** Result: " << apol2str(red_policy) << " won" << endl;
          } else if (status == 0) {
            cout << "********** Result: " << apol2str(blue_policy) << " won" << endl;
          } else if (status == 1) {
            cout << "********** Result: draw" << endl;
          }
        }
        bp_index += 1;
      }
      rp_index += 1;
      bp_index = 0;
    }
    (world->units).clear();
    delete world;
    time += 1;
  }
  /* ---------------------------------------------------- */

  return 0;
}
