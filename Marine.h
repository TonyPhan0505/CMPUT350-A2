#pragma once

// DON'T CHANGE

#include "Unit.h"

struct Marine : public Unit
{
  Marine(Team team, const Vec2 &pos, AttackPolicy pol, bool bounce);
  
  // execute one attack depending on policy
  // when attacking you need to call w.attack
  void act(World &w);

  // deal with wall collisions depending on bounce
  void collision_hook(double prev_speed, bool collisions[4]);

  // which attack policy is this unit using?
  AttackPolicy policy;

  // does it bounce off walls?
  bool bounce;
};
