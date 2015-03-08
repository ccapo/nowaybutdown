#include <stdio.h>
#include "Main.hpp"

Destructible::Destructible(float maxHp, float defense, const char *corpseName) :
	maxHp(maxHp),hp(maxHp),defense(defense),corpseName(corpseName) {
}

Destructible::~Destructible(){
	corpseName = NULL;
}

float Destructible::takeDamage(Actor *owner, float damage) {
	damage -= defense;
	if ( damage > 0 ) {
		hp -= damage;
		if ( hp <= 0 ) {
			die(owner);
		}
	} else {
		damage=0;
	}
	return damage;
}

float Destructible::heal(float amount) {
	hp += amount;
	if ( hp > maxHp ) {
		amount -= hp-maxHp;
		hp=maxHp;
	}
	return amount;
}

void Destructible::die(Actor *owner) {
	// transform the actor into a corpse!
	owner->ch=260;
	owner->col=TCODColor::white;	
	owner->name=corpseName;
	owner->blocks=false;
	// make sure corpses are drawn before living actors
	engine.sendToBack(owner);
}

MonsterDestructible::MonsterDestructible(float maxHp, float defense, const char *corpseName) :
	Destructible(maxHp,defense,corpseName) {
}

void MonsterDestructible::die(Actor *owner) {
	// transform it into a nasty corpse! it doesn't block, can't be
	// attacked and doesn't move
	engine.gui->message(TCODColor::lightGrey,"%s is dead",owner->name);
	Destructible::die(owner);
}

PlayerDestructible::PlayerDestructible(float maxHp, float defense, const char *corpseName) :
	Destructible(maxHp,defense,corpseName) {
}

void PlayerDestructible::die(Actor *owner) {
	engine.gui->message(TCODColor::red,"You died!");
	Destructible::die(owner);
	engine.gameStatus=Engine::DEFEAT;
}
