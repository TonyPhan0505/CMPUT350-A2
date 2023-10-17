#include <cstdlib>
#include <cassert>
#include <limits>
#include "Unit.h"
#include "World.h"

// THIS FILE WILL NOT BE SUBMITTED
// ANY CHANGES HERE WE WON'T SEE

// create pseudo-random numbers

// integer [0,n)
int World::rnd_int(int n) const
{
  assert(n > 0);
  std::uniform_int_distribution<int> dist(0,n-1); // range [0,n-1]
  int r = dist(rng);
  assert(r >= 0 && r < n);
  return r;
}

// double [0,1)
double World::rnd01() const
{
  std::uniform_real_distribution<double> dist(0,1); // range [0,1)
  double r = dist(rng);
  assert(r >= 0 && r < 1);
  return r;
}

// perform one simulation cycle

void World::step()
{
  // remove dead objects
  int killed = 0;
  for (size_t i=0; i < units.size(); ++i) {
    if (units[i]->hp <= 0) {
      delete units[i];
      units[i] = units[units.size()-1];
      units.pop_back();
      --i;
      ++killed;
    }
  }

  // cout << "killed: " << killed << endl;
  
  // move objects
  for (auto p : units) {
    if (p->current_speed > 0) {
      move_unit(*p);
    }
  }

  // let each unit act in the world

  // copy hp values for targeting decisions
  for (auto p : units) {
    p->hp_old = p->hp;
  }

  // actions assumed to be commutative (otherwise action order needs to be
  // randomized)
  // attacks are commutative if targeting decisions only depend on hp_old
  for (auto p : units) {
    p->act(*this);
  }
}
