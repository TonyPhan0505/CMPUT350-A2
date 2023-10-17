#include "Marine.h"

// ... implement
Marine :: Marine(Team team, const Vec2 &pos, AttackPolicy pol, bool bounce) : Unit(team, pos, 10, 40, 10, 1, 5) {
    this -> policy = pol;
    this -> bounce = bounce;
}

void Marine :: act(World &w) {}