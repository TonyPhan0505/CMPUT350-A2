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

void World :: move_unit(Unit &u)
{
  // ... implement
  double newX = u.pos.x + u.heading.x * u.current_speed;
  double newY = u.pos.y + u.heading.y * u.current_speed;

  // detect collision with the walls
  bool right = false;
  bool left = false;
  bool top = false;
  bool bottom = false;
  if ((newX + u.radius) >= width) {
    right = true;
    // handle clipping
    if ((newX + u.radius) > width) {
      newX = width - u.radius;
    }
  }
  if ((newX - u.radius) <= 0) {
    left = true;
    // handle clipping
    if ((newX - u.radius) < 0) {
      newX = u.radius;
    }
  }
  if ((newY - u.radius) <= 0) {
    top = true;
    // handle clipping
    if ((newY - u.radius) < 0) {
      newY = u.radius;
    }
  }
  if ((newY + u.radius) >= height) {
    bottom = true;
    // handle clipping
    if ((newY + u.radius) > height) {
      newY = height - u.radius;
    }
  }
  bool collisions[4] = {right, left, top, bottom};

  // handle wall collision
  if (right || left || top || bottom) {
    double prev_speed = u.current_speed;
    u.current_speed = 0.0;
    u.collision_hook(prev_speed, collisions);
  }

  // update position
  u.pos.x = newX;
  u.pos.y = newY;
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


World :: ~World()
{
  // clean up
  
  // ... implement
  units.clear();
}


// deducts hit points and calls attack_hook

void World :: attack(Unit &attacker, Unit &attacked)
{
  // ... implement
  attacked.hp -= 1;
  this -> attack_hook(attacker, attacked);
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

Vec2 World :: rnd_heading() const
{
  // ... implement
  uniform_real_distribution<double> distribution(-1.0, 1.0);
  double randomX = 0.0;
  double randomY = 0.0;
  while (sqrt(randomX * randomX + randomY * randomY) != 1.0) {
    randomX = distribution(rng);
    randomY = distribution(rng);
  }
  return Vec2(randomX, randomY);
}

// mirror position with respect to map mid-point

Vec2 World :: mirror(const Vec2 &pos) const
{
  // ... implement
  double xRef = pos.x;
  double xMir = (this -> width) - xRef;
  double yRef = pos.y;
  double yMir = (this -> height) - yRef;
  return Vec2(xMir, yMir);
}


// return squared distance between two unit centers

double World :: distance2(const Unit &u, const Unit &v)
{
  // ... implement
  double xDif = u.pos.x - v.pos.x;
  double yDif = u.pos.y - v.pos.y;
  double distance = sqrt(xDif * xDif + yDif * yDif);
  return distance;
}

// return true iff u can attack v, i.e. distance of u's and v's centers is
// less than u's attack distance plus v's radius
bool World :: can_attack(const Unit &u, const Unit &v)
{
  // ... implement
  double distance = distance2(u, v);
  return distance < (u.attack_radius + v.radius);
}

// populate a vector with enemy units that can be attacked by u;
// clears vector first
void World :: enemies_within_attack_range(const Unit &u,
                                        vector<Unit*> &targets) const
{
  targets.clear();

  // ... implement
  size_t numOfUnits = units.size();
  // use push_back to add elements to targets
  for (size_t i = 0; i < numOfUnits; i++) {
    if (&u != units[i] && can_attack(u, *(units[i]))) {
      targets.push_back(units[i]);
    }
  }
}

// return a random unit that can be attacked by u with minimal hp_old value,
// or 0 if none exists

Unit* World :: random_weakest_target(Unit &u) const
{
  vector<Unit*> targets;

  // ... implement
  enemies_within_attack_range(u, targets);
  if (targets.empty()) {
        return 0;
  }
  int minHpOld = 100;
  Unit* weakestTarget = nullptr;
  for (size_t i = 0; i < targets.size(); i++) {
    if (((targets[i]) -> hp_old) < minHpOld) {
      minHpOld = (targets[i]) -> hp_old;
      weakestTarget = targets[i];
    }
  }
  return weakestTarget;
}


// return a random unit that can be attacked by u with minimal distance to
// u, or 0 if none exists

Unit* World :: random_closest_target(Unit &u) const
{
  vector<Unit*> targets;

  // ... implement
  enemies_within_attack_range(u, targets);
  if (targets.empty()) {
        return 0;
  }
  double minDistance = sqrt(width * width + height * height);
  Unit* closestTarget = nullptr;
  for (size_t i = 0; i < targets.size(); i++) {
    double distance = distance2(u, *(targets[i]));
    if (distance < minDistance) {
      minDistance = distance;
      closestTarget = targets[i]; 
    }
  }
  return closestTarget;
}


// return a random unit that can be attacked by u with maximal damage/hp_old
// ratio, or 0 if none exists

Unit* World :: random_most_dangerous_target(Unit &u) const
{
  vector<Unit*> targets;

  // ... implement
  enemies_within_attack_range(u, targets);
  if (targets.empty()) {
        return 0;
  }
  double maxRatio = 0.0;
  Unit* mostDanger = nullptr;
  for (size_t i = 0; i < targets.size(); i++) {
    double ratio = ((targets[i]) -> damage) / ((targets[i]) -> hp_old);
    if (ratio > maxRatio) {
      maxRatio = ratio;
      mostDanger = targets[i];
    }
  }
  return mostDanger;
}


// return score for red: 2 for win, 0 for loss, 1 for draw, -1 for game not
// over yet

int World :: red_score() const
{
  if (units.empty()) {
    return 1;
  }

  // ... implement
  int red_count = 0;
  int blue_count = 0;
  for (size_t i = 0; i < units.size(); i++) {
    if ((units[i] -> team) == Team::RED) {
      red_count += 1;
    } else {
      blue_count += 1;
    }
  }
  if ((red_count > 0) && (blue_count == 0)) {
    return 2;
  } else if ((red_count == 0) && (blue_count > 0)) {
    return 0;
  } else if ((red_count == 0) && (blue_count == 0)) {
    return 1;
  } else {
    return -1;
  }
}

