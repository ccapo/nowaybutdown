#include <stdio.h>
#include "Main.hpp"

Attacker::Attacker(float power) : basePower(power), power(power) {}

void Attacker::attack(Actor *owner, Actor *target) {
	if ( target->destructible && ! target->destructible->isDead() ) {
		float damage=target->destructible->takeDamage(target,power);
		if ( damage > 0 ) {
			engine.gui->message(owner==engine.player ? TCODColor::red : TCODColor::lightGrey,"%s attacks %s for %g hit points.", owner->name, target->name, damage);
		} else {
			engine.gui->message(TCODColor::lightGrey,"%s attacks %s but it has no effect!", owner->name, target->name);			
		}
	} else {
		engine.gui->message(TCODColor::lightGrey,"%s attacks %s in vain.",owner->name,target->name);
	}
}

void Attacker::wield(Actor *owner, Actor *wielder) {
	owner->attacker->power = owner->attacker->basePower + wielder->attacker->power;
}