#pragma once

// DON'T CHANGE

#include <vector>
#include <random>

// 2-component vector

struct Vec2
{
  Vec2(double x_=0, double y_=0)
    : x(x_), y(y_)
  { }
  
  double x, y;
};

// which unit to attack?
enum AttackPolicy { ATTACK_WEAKEST, ATTACK_CLOSEST, ATTACK_MOST_DANGEROUS };

// returns policy name
const char* apol2str(AttackPolicy pol);

// returns square of number
inline double square(double x) { return x * x; }

struct Unit;

// describes state of the simulation

class World
{
public:
  
  World(double w, double h, int seed)
    : width(w), height(h)
  {
    rng.seed(seed);
  }

  virtual ~World();

  // helper (see World.C)
  // moves a unit depending on current_speed and heading
  void move_unit(Unit &u);

  // deducts hit points and calls attack_hook
  void attack(Unit &attacker, Unit &attacked);

  // inform user that a unit was attacked
  virtual void attack_hook(const Unit &/*attacker*/,
                           const Unit &/*attacked*/) { }

  // return a random position at which a circle of that radius fits
  Vec2 rnd_pos(double radius) const;

  // return uniform random heading
  // length of vector = 1
  Vec2 rnd_heading() const;

  // mirror position with respect to map mid-point
  Vec2 mirror(const Vec2 &pos) const;
  
  // return squared distance between two unit centers
  static double distance2(const Unit &u, const Unit &v);

  // return true iff u can attack v, i.e. distance of u's and v's centers is
  // less than u's attack distance plus v's radius
  static bool can_attack(const Unit &u, const Unit &v);

  // populate a vector with enemy units that can be attacked by u
  // clears vector first  
  void enemies_within_attack_range(const Unit &u,
                                   std::vector<Unit*> &targets) const;

  // return a random unit that can be attacked by u with minimal hp_old value, or
  // 0 if none exists
  Unit *random_weakest_target(Unit &u) const;

  // return a random unit that can be attacked by u with minimal distance to
  // u, or 0 if none exists
  Unit *random_closest_target(Unit &u) const;

  // return a random unit that can be attacked by u with maximal damage/hp_old
  // ratio, or 0 if none exists
  Unit *random_most_dangerous_target(Unit &u) const;
  
  // return score for red: 2 for win, 0 for loss, 1 for draw, -1 for game not
  // over yet
  int red_score() const;

  // perform one simulation cycle
  void step();

  // helpers
  
  // uniform random integer in [0,n) (excluding n, n > 0)
  int rnd_int(int n) const;

  // uniform random double in [0,1)  
  double rnd01() const;

  // data

  double width, height;     // map dimensions
  std::vector<Unit*> units; // pointers to all units, owner

  mutable std::mt19937 rng; // to generate random numbers local to simulation
  // (apparently, OpenGL calls C's global random() function ...)
};
