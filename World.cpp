#include <cmath>
#include <cassert>
#include <cstdlib>
#include <iostream>
#include "World.h"
#include "Unit.h"

using namespace std;

/* helper that you need to implement
   
 - move unit by (current_speed * heading)

 - units only collide with the map border
 
 - when a unit hits a wall it stops at the collision point (current_speed=0)
   and collision_hook needs to be called with the previous speed and an array
   of bools encoding all the walls that were hit (the array indexes need to be
   consistent with enum CollDir). This array is used to implement bouncing by
   only changing the unit's heading, i.e. even when bouncing the unit stays at
   the collision location for the remainder of the simulation frame and only
   starts moving again in the following frame

 - it is essential that units are located inside the map at all times. There
   can be no overlap. Ensure this property by clipping coordinates after
   moving

*/

void World::move_unit(Unit &u)
{
  // ... implement
}


// returns policy name
const char *apol2str(AttackPolicy pol)
{
  switch (pol) {
    case ATTACK_WEAKEST:
      return "attack-weakest";
    case ATTACK_CLOSEST:
      return "attack-closest";
    case ATTACK_MOST_DANGEROUS:
      return "attack-most-dangerous";
  }
  return "?";
}


World::~World()
{
  // clean up
  
  // ... implement
}


// deducts hit points and calls attack_hook

void World::attack(Unit &attacker, Unit &attacked)
{
  // ... implement
}


// return a random position at which a circle of that radius fits

Vec2 World::rnd_pos(double radius) const
{
  double slack = radius * 2;

  Vec2 p((width  - 2*slack) * rnd01() + slack,
         (height - 2*slack) * rnd01() + slack);
  
  // assert circle in rectangle
  assert(p.x > radius && p.x < width - radius);
  assert(p.y > radius && p.y < height - radius);
  return p;
}


// return uniform random heading
// length of vector = 1

Vec2 World::rnd_heading() const
{
  // ... implement
}

// mirror position with respect to map mid-point

Vec2 World::mirror(const Vec2 &pos) const
{
  // ... implement
}


// return squared distance between two unit centers

double World::distance2(const Unit &u, const Unit &v)
{
  // ... implement
}

// return true iff u can attack v, i.e. distance of u's and v's centers is
// less than u's attack distance plus v's radius
bool World::can_attack(const Unit &u, const Unit &v)
{
  // ... implement
}

// populate a vector with enemy units that can be attacked by u;
// clears vector first
void World::enemies_within_attack_range(const Unit &u,
                                        vector<Unit*> &targets) const
{
  targets.clear();

  // ... implement

  // use push_back to add elements to targets
}

// return a random unit that can be attacked by u with minimal hp_old value,
// or 0 if none exists

Unit *World::random_weakest_target(Unit &u) const
{
  vector<Unit*> targets;

  // ... implement
}


// return a random unit that can be attacked by u with minimal distance to
// u, or 0 if none exists

Unit *World::random_closest_target(Unit &u) const
{
  vector<Unit*> targets;

  // ... implement
}


// return a random unit that can be attacked by u with maximal damage/hp_old
// ratio, or 0 if none exists

Unit *World::random_most_dangerous_target(Unit &u) const
{
  vector<Unit*> targets;

  // ... implement
}


// return score for red: 2 for win, 0 for loss, 1 for draw, -1 for game not
// over yet

int World::red_score() const
{
  if (units.empty()) {
    return 1;
  }

  // ... implement
}

