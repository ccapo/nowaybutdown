#include "Main.hpp"

bool Item::grab(Object *owner, Object *object) {
	if ( object->container && object->container->add(owner) ) {
		engine.objects.remove(owner);
		return true;
	}
	return false;
}

void Item::drop(Object *owner, Object *object) {
	if ( object->container ) {
		object->container->remove(owner);
		engine.objects.push(owner);
		owner->x=object->x;
		owner->y=object->y;
		engine.gui->message(TCODColor::lightGrey,"%s drops a %s.", object->name,owner->name);
	}
}

bool Item::use(Object *owner, Object *object) {
	if ( object->container ) {
		object->container->remove(owner);
		delete owner;
		return true;
	}
	return false;
}

Healer::Healer(float amount) : amount(amount) {
}

bool Healer::use(Object *owner, Object *object) {
	if ( object->entity ) {
		int amountHealed = object->entity->heal(amount);
		if ( amountHealed > 0 ) {
			engine.gui->message(TCODColor::lightGrey, "You consume a potion that increased your HP by %d", amountHealed);
			return Item::use(owner,object);
		}
	}
	return false;
}

LightningBolt::LightningBolt(float range, float damage) 
	: range(range),damage(damage) {
}

bool LightningBolt::use(Object *owner, Object *object) {
	Object *closestMonster=engine.getClosestMonster(object->x,object->y,range);
	if (! closestMonster ) {
		engine.gui->message(TCODColor::lightGrey,"No enemy is close enough to strike.");
		return false;
	}
	// hit closest monster for <damage> hit points
	engine.gui->message(TCODColor::lightBlue,
		"A lighting bolt strikes the %s with a loud thunder!\n"
		"The damage is %g hit points.",
		closestMonster->name,damage);
	closestMonster->entity->damage(closestMonster,damage);
	return Item::use(owner,object);
}

Confuser::Confuser(int nTurns, float range)
	: nTurns(nTurns), range(range) {
}

bool Confuser::use(Object *owner, Object *object) {
	engine.gui->message(TCODColor::cyan, "Left-click an enemy to confuse it,\nor right-click to cancel.");
	int x,y;
	if (! engine.pickATile(&x,&y,range)) {
		return false;
	}

	Object *obj=engine.getObject(x,y);
	if (! obj ) {
		return false;
	}
	// confuse the monster for <nTurns> turns
	Ai *confusedAi=new ConfusedMonsterAi( nTurns, obj->entity->ai );
	object->entity->ai = confusedAi;
	engine.gui->message(TCODColor::lightGreen,"The eyes of the %s look vacant,\nas he starts to stumble around!",
		object->name);
	return Item::use(owner,obj);
}

Fireball::Fireball(float range, float damage)
	: LightningBolt(range,damage) {		
}

bool Fireball::use(Object *owner, Object *object) {
	engine.gui->message(TCODColor::cyan, "Left-click a target tile for the fireball,\nor right-click to cancel.");
	int x,y;
	if (! engine.pickATile(&x,&y)) {
		return false;
	}
	// burn everything in <range> (including player)
	engine.gui->message(TCODColor::orange,"The fireball explodes, burning everything within %g tiles!",range);
	for (Object **iterator=engine.objects.begin();
	    iterator != engine.objects.end(); iterator++) {
		Object *object=*iterator;
		if ( object->entity && !object->entity->isDead()
			&& object->getDistance(x,y) <= range) {
			engine.gui->message(TCODColor::orange,"The %s gets burned for %g hit points.",
				object->name,damage);
			object->entity->damage(object,damage);
		}
	}
	return Item::use(owner,object);
}

