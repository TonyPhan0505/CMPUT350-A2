#include "Marine.h"
#include <cassert>

// ... implement
Marine :: Marine(Team team, const Vec2 &pos, AttackPolicy pol, bool bounce) : Unit(team, pos, 10, 40, 10, 1, 45) {
    assert(pol == AttackPolicy::ATTACK_WEAKEST || pol == AttackPolicy::ATTACK_CLOSEST || pol == AttackPolicy::ATTACK_MOST_DANGEROUS);
    this -> policy = pol;
    this -> bounce = bounce;
}

void Marine :: act(World &w) {
    Unit* attacker = this;
    Unit* attacked = nullptr;
    if ((this -> policy) == AttackPolicy::ATTACK_WEAKEST) {
        attacked = w.random_weakest_target(*attacker);
    } else if ((this -> policy) == AttackPolicy::ATTACK_CLOSEST) {
        attacked = w.random_closest_target(*attacker);
    } else {
        attacked = w.random_most_dangerous_target(*attacker);
    }
    if (attacked != 0) {
        w.attack(*attacker, *attacked);
    }
}

void Marine :: collision_hook(double prev_speed, bool collisions[4]) {
    if (this -> bounce) {
        this -> current_speed = prev_speed;
        if (collisions[0] || collisions[1]) {
            // change x heading when colliding with vertical borders
            heading.x = -(heading.x);
        } else if (collisions[2] || collisions[3]) {
            // change y heading when colliding with horizontal borders
            heading.y = -(heading.y);
        }
    }
}