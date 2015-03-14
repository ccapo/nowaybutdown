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
		owner->x = object->x;
		owner->y = object->y;
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

bool Item::equip(Object *owner, Object *object) {
	if ( owner->entity ) {
		object->entity->worn = owner;
		return true;
	}
	return false;
}

bool Item::wield(Object *owner, Object *object) {
	if ( owner->entity ) {
		object->entity->wielded = owner;
		return true;
	}
	return false;
}

bool Potion::use(Object *owner, Object *object) {
	if ( object->entity ) {
		switch( type ) {
			case HEAL: {
				int netAmount = object->entity->heal(amount);
				if( netAmount > 0 ) {
					engine.gui->message(TCODColor::lightGrey, "You consume a health potion that increased your HP by %d", netAmount);
					return Item::use(owner,object);
				}
				break;
			}
			case ATK: {
				object->entity->atk += amount;
				engine.gui->message(TCODColor::lightGrey, "You consume an attack potion that increased your ATK by %d", amount);
				return Item::use(owner,object);
				break;
			}
			case DEF: {
				object->entity->def += amount;
				engine.gui->message(TCODColor::lightGrey, "You consume a defense potion that increased your DEF by %d", amount);
				return Item::use(owner,object);
				break;
			}
			case UNKNOWN: {
				TCODRandom *rng = TCODRandom::getInstance();
				int potion = rng->getInt(HEAL, DEF);
				switch( potion ) {
					case HEAL: {
						int netAmount = object->entity->heal(amount);
						if( netAmount != 0 ) {
							std::string msg;
							if( netAmount > 0 ) {
								msg = "You consume a health potion that increased your HP by %d";
							} else {
								msg = "You consume a poison potion that decreased your HP by %d";
							}
							engine.gui->message(TCODColor::lightGrey, msg.c_str(), abs(netAmount));
							return Item::use(owner,object);
						}
						break;
					}
					case ATK: {
						object->entity->atk += amount;
						if( object->entity->atk < 0 ) object->entity->atk = 0;
						if( amount != 0 ) {
							std::string msg;
							if( amount > 0 ) {
								msg = "You consume an attack potion that increased your ATK by %d";
							} else {
								msg = "You consume an attack potion that decreased your ATK by %d";
							}
							engine.gui->message(TCODColor::lightGrey, msg.c_str(), abs(amount));
							return Item::use(owner,object);
						}
						break;
					}
					case DEF: {
						object->entity->def += amount;
						if( object->entity->def < 0 ) object->entity->def = 0;
						if( amount != 0 ) {
							std::string msg;
							if( amount > 0 ) {
								msg = "You consume an defense potion that increased your DEF by %d";
							} else {
								msg = "You consume an defense potion that decreased your DEF by %d";
							}
							engine.gui->message(TCODColor::lightGrey, msg.c_str(), abs(amount));
							return Item::use(owner,object);
						}
						break;
					}
					default: break;
				}
				break;
			}
			default: break;
		}
	}
	return false;
}

bool Equipment::equip(Object *owner, Object *object) {
	if ( owner->entity ) {
		std::cout << "Equipped: " << owner->name << std::endl;
		object->entity->worn = owner;

		object->entity->hpMax += owner->entity->hp;
		object->entity->hp += owner->entity->hp;
		object->entity->atk += owner->entity->atk;
		object->entity->def += owner->entity->def;
		return true;
	}
	return false;
}

bool Equipment::wield(Object *owner, Object *object) {
	if ( owner->entity ) {
		object->entity->wielded = owner;

		object->entity->hpMax += owner->entity->hp;
		object->entity->hp += owner->entity->hp;
		object->entity->atk += owner->entity->atk;
		object->entity->def += owner->entity->def;
		return true;
	}
	return false;
}

/*bool Confuser::use(Object *owner, Object *object) {
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
}*/