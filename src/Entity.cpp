#include <stdio.h>
#include "Main.hpp"

Entity::Entity(int hpMax, int atk, int def, const char *corpseName) :
	baseHpMax(hpMax), hpMax(hpMax), hp(hpMax), baseAtk(atk), atk(atk), baseDef(def), def(def),
	corpseName(corpseName), ai(NULL), wielded(NULL), worn(NULL) {
}

Entity::~Entity() {
	corpseName = NULL;
	if ( ai ) delete ai;
	wielded = NULL;
	worn = NULL;
}

void Entity::attack(Object *owner, Object *target) {
	if ( target->entity && !target->entity->isDead() ) {
		int damage = target->entity->damage(target, atk);
		if ( damage > 0 ) {
			engine.gui->message(owner == engine.player ? TCODColor::red : TCODColor::lightGrey, "%s attacks %s for %d hit points.", owner->name, target->name, damage);
		} else {
			engine.gui->message(TCODColor::lightGrey, "%s attacks %s but it has no effect!", owner->name, target->name);
		}
	} else {
		engine.gui->message(TCODColor::lightGrey, "%s attacks %s in vain.", owner->name, target->name);
	}
}

int Entity::damage(Object *owner, int amount) {
	amount /= 2;
	amount -= def/4;
	if ( amount > 0 ) {
		hp -= amount;
		if ( hp <= 0 ) {
			die(owner);
		}
	} else {
		amount = 0;
	}
	return amount;
}

int Entity::heal(int amount) {
	hp += amount;
	if ( hp > hpMax ) {
		amount -= hp - hpMax;
		hp = hpMax;
	}
	return amount;
}

void Entity::die(Object *owner) {
	// transform the object into a corpse!
	owner->ch = CHAR_SKULL;
	owner->col = TCODColor::white;	
	owner->name = corpseName;
	owner->blocks = false;
	if( owner != engine.player ) {
		delete owner->entity->ai;
		owner->entity->ai = NULL;
		delete owner->entity;
		owner->entity = NULL;
		owner->item = new Equipment(Equipment::CORPSE, 0, 0, 1);
	}
	// make sure corpses are drawn before living objects
	engine.sendToBack(owner);
}

CreatureEntity::CreatureEntity(int hpMax, int atk, int def, const char *corpseName) :
	Entity(hpMax, atk, def, corpseName) {
}

void CreatureEntity::die(Object *owner) {
	// transform it into a nasty corpse! it doesn't block, can't be
	// attacked and doesn't move
	engine.gui->message(TCODColor::lightGrey, "%s is dead", owner->name);
	Entity::die(owner);
}

PlayerEntity::PlayerEntity(int hpMax, int atk, int def, const char *corpseName) :
	Entity(hpMax, atk, def, corpseName) {
}

void PlayerEntity::die(Object *owner) {
	engine.gui->message(TCODColor::red, "You died!");
	Entity::die(owner);
	engine.gameStatus = Engine::DEFEAT;
}
