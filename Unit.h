#pragma once

// DON'T CHANGE

#include "World.h"

// team names
enum Team { RED, BLUE };

// which wall was hit? (used as array index)
enum CollDir { RIGHT=0, LEFT, TOP, BOTTOM };

struct Unit
{
  Team team;
  Vec2 pos;
  const double radius;
  const double attack_radius;
  const double max_speed;
  const double damage;    
  int hp;
  double current_speed; // >= 0, 0 => stopped
  Vec2 heading;         // vector normalized (length = 1)

  int hp_old; // hp will be copied to this variable at the beginning of each
              // frame. Targeting decisions must depend on hp_old rather than
              // hp to ensure that targeting decisions don't depend on action
              // execution order.
  
  Unit(Team team_,
       const Vec2 &pos_,
       double radius_,
       double attack_radius_,
       double max_speed_,
       double damage_,
       int hp_)
    : team(team_),
      pos(pos_),
      radius(radius_),
      attack_radius(attack_radius_),
      max_speed(max_speed_),
      damage(damage_),
      hp(hp_)
  {
    current_speed = 0;
    hp_old = 0;
  }

  // act in the world (currently just attacking)
  // in case a unit is attacked, you need to call w.attack
  virtual void act(World &w) = 0;

  // react to collision
  // prev_speed: unit's previous speed
  // collisions[i] = true iff unit hit wall i (indexes consistent with CollDir enum)
  virtual void collision_hook(double prev_speed, bool collisions[4]) = 0;
  
  virtual ~Unit() { }
};
