#include <stdio.h>
#include "Main.hpp"

Entity::Entity(int hpMax, int atk, int def, const char *corpseName) :
	baseHpMax(hpMax), hpMax(hpMax), hp(hpMax), baseAtk(atk), atk(atk), baseDef(def), def(def),
	corpseName(corpseName), ai(NULL) {
}

Entity::~Entity() {
	corpseName = NULL;
	if ( ai ) delete ai;
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
	amount -= def;
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

void Entity::equip(Object *object) {
	hpMax = baseHpMax + object->entity->hpMax;
	hp += object->entity->hpMax;
	atk = baseAtk + object->entity->atk;
	def = baseDef + object->entity->def;
}

void Entity::dequip(Object *object) {
	hpMax = baseHpMax - object->entity->hpMax;
	hp -= object->entity->hpMax;
	atk = baseAtk - object->entity->atk;
	def = baseDef - object->entity->def;
}

void Entity::die(Object *owner) {
	// transform the object into a corpse!
	owner->ch = 260;
	owner->col = TCODColor::white;	
	owner->name = corpseName;
	owner->blocks = false;
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
