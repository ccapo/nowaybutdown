#include <stdio.h>
#include <math.h>
#include "Main.hpp"

// How many turns the monster chases the player after losing his sight
static const int TRACKING_TURNS = 3;

MonsterAi::MonsterAi() : moveCount(0) {}

void MonsterAi::update(Actor *owner) {
    if ( owner->destructible && owner->destructible->isDead() ) {
    	return;
    }
	if ( engine.map->isInFov(owner->x, owner->y) ) {
    	// we can see the player. move towards him
    	moveCount = TRACKING_TURNS;
    } else {
    	moveCount--;
    }
   	if ( moveCount > 0 ) {
   		moveOrAttack(owner, engine.player->x, engine.player->y);
   	}
}

void MonsterAi::moveOrAttack(Actor *owner, int targetx, int targety) {
	int dx = targetx - owner->x;
	int dy = targety - owner->y;
	int stepdx = (dx > 0 ? 1 : -1);
	int stepdy = (dy > 0 ? 1 : -1);
	float distance = sqrtf(dx*dx + dy*dy);
	if ( distance >= 2 ) {
		dx = (int)(round(dx/distance));
		dy = (int)(round(dy/distance));
		if ( engine.map->canWalk(owner->x + dx, owner->y + dy) ) {
			owner->x += dx;
			owner->y += dy;
		} else if ( engine.map->canWalk(owner->x + stepdx, owner->y) ) {
			owner->x += stepdx;
		} else if ( engine.map->canWalk(owner->x, owner->y + stepdy) ) {
			owner->y += stepdy;
		}
	} else if ( owner->attacker ) {
		owner->attacker->attack(owner, engine.player);
	}
}

ConfusedMonsterAi::ConfusedMonsterAi(int nbTurns, Ai *oldAi) : nbTurns(nbTurns), oldAi(oldAi) {}

void ConfusedMonsterAi::update(Actor *owner) {
	TCODRandom *rng = TCODRandom::getInstance();
	int dx = rng->getInt(-1, 1);
	int dy = rng->getInt(-1, 1);
	if ( dx != 0 || dy != 0 ) {
		int destx = owner->x + dx;
		int desty = owner->y + dy;
		if ( engine.map->canWalk(destx, desty) ) {
			owner->x = destx;
			owner->y = desty;
		} else {
			Actor *actor = engine.getActor(destx, desty);
			if ( actor ) {
				owner->attacker->attack(owner, actor);
			}
		}
	}
	nbTurns--;
	if ( nbTurns == 0 ) {
		owner->ai = oldAi;
		delete this;
	}
}

void PlayerAi::update(Actor *owner) {
    if ( owner->destructible && owner->destructible->isDead() ) {
    	return;
    }
	int dx = 0, dy = 0;
	switch(engine.lastKey.vk) {
		case TCODK_UP : dy = -1; break;
		case TCODK_DOWN : dy = 1; break;
		case TCODK_LEFT : dx = -1; break;
		case TCODK_RIGHT : dx = 1; break;
		case TCODK_ESCAPE : // display inventory
		{
			Actor *actor = choseFromInventory(owner);
			if ( actor ) {
				actor->item->use(actor, owner);
				engine.gameStatus = Engine::NEW_TURN;
			}
			break;
		}
		case TCODK_CHAR : handleActionKey(owner, engine.lastKey.c, dx, dy); break;
        default: break;
    }
    if (dx != 0 || dy != 0) {
    	engine.gameStatus = Engine::NEW_TURN;
    	if (moveOrAttack(owner, owner->x + dx,owner->y + dy)) {
    		engine.map->computeFov();
    	}
	}

}

bool PlayerAi::moveOrAttack(Actor *owner, int targetx, int targety) {
	if ( engine.map->isWall(targetx, targety) ) return false;
	// look for living actors to attack
	for (Actor **iterator = engine.actors.begin(); iterator != engine.actors.end(); iterator++) {
		Actor *actor = *iterator;
		if ( actor->destructible && !actor->destructible->isDead() && actor->x == targetx && actor->y == targety ) {
			owner->attacker->attack(owner, actor);
			return false;
		}
	}
	// look for corpses or items
	for (Actor **iterator=engine.actors.begin(); iterator != engine.actors.end(); iterator++) {
		Actor *actor = *iterator;
		bool corpseOrItem = (actor->destructible && actor->destructible->isDead()) || actor->item;
		if ( corpseOrItem && actor->x == targetx && actor->y == targety ) {
			engine.gui->message(TCODColor::lightGrey, "There's a %s here.", actor->name);
		}
	}
	owner->x = targetx;
	owner->y = targety;
	return true;
}

void PlayerAi::handleActionKey(Actor *owner, int ascii, int &dx, int &dy) {
	switch(ascii) {
		case 'i' : dy = -1; break; 
		case 'k' : dy = 1; break;
		case 'j' : dx = -1; break;
		case 'l' : dx = 1; break;
		//case 'd' : // drop item 
		//{
		//	Actor *actor = choseFromInventory(owner);
		//	if ( actor ) {
		//		actor->item->drop(actor, owner);
		//		engine.gameStatus = Engine::NEW_TURN;
		//	}
		//	break;
		//}
		case 'g' : // grab item
		{
			bool found=false;
			for (Actor **iterator = engine.actors.begin(); iterator != engine.actors.end(); iterator++) {
				Actor *actor = *iterator;
				if ( actor->item && actor->x == owner->x && actor->y == owner->y ) {
					if (actor->item->grab(actor,owner)) {
						found = true;
						engine.gui->message(TCODColor::lightGrey, "You grab the %s.", actor->name);
						break;
					} else if (! found) {
						found = true;
						engine.gui->message(TCODColor::red, "Your inventory is full.");
					}
				}
			}
			if (!found) {
				engine.gui->message(TCODColor::lightGrey, "There's nothing here that you can grab.");
			}
			engine.gameStatus = Engine::NEW_TURN;
			break;
		}
		//case 'w' : // wield item 
		//{
		//	Actor *actor=choseFromInventory(owner);
		//	if ( actor ) {
		//		//actor->item->drop(actor, owner);
		//		owner->attacker->wield(actor);
		//		engine.gameStatus = Engine::NEW_TURN;
		//	}
		//	break;
		//}		
		case '?' : // help screen
		{
			helpScreen();
			break;
		}
		default: break;
	}
}

Actor *PlayerAi::choseFromInventory(Actor *owner) {
	static const int INVENTORY_WIDTH = 50;
	static const int INVENTORY_HEIGHT = 28;
	static TCODConsole con(INVENTORY_WIDTH, INVENTORY_HEIGHT);

	// display the inventory frame
	con.setDefaultForeground(TCODColor(200,180,50));
	con.printFrame(0, 0, INVENTORY_WIDTH, INVENTORY_HEIGHT, true, TCOD_BKGND_DEFAULT, "Inventory");

	int y = 2;
	con.setDefaultForeground(TCODColor::white);
	con.print(2, y++, "%s", "Inventory Options");
	con.hline(2, y++, 18);
	y++;
	con.print(2, y++, "%s", "Drop Selected Item : d");
	con.print(2, y++, "%s", "Equip Selected Item: e");
	con.print(2, y++, "%s", "Use Selected Item  : u");
	con.print(2, y++, "%s", "Wield Selected Item: w");
	con.print(2, y++, "%s", "Exit Inventory     : Escape");	

	// display the items with their keyboard shortcut
	con.setDefaultForeground(TCODColor::white);
	int shortcut = 'a';
	for (Actor **it = owner->container->inventory.begin(); it != owner->container->inventory.end(); it++) {
		Actor *actor=*it;
		con.print(2,y++,"(%c) %s", shortcut, actor->name);
		shortcut++;
	}

	// blit the inventory console on the root console
	TCODConsole::blit(&con, 0, 0, INVENTORY_WIDTH, INVENTORY_HEIGHT, TCODConsole::root, engine.windowWidth/2 - INVENTORY_WIDTH/2, engine.windowHeight/2 - INVENTORY_HEIGHT/2);
	TCODConsole::flush();

	// wait for a key press
	TCOD_key_t key;
	//TCODSystem::waitForEvent(TCOD_EVENT_KEY_PRESS, &key, NULL, true);
	//if ( key.vk == TCODK_CHAR ) {
	//	int actorIndex=key.c - 'a';
	//	if ( actorIndex >= 0 && actorIndex < owner->container->inventory.size() ) {
	//		return owner->container->inventory.get(actorIndex);
	//	}
	//}
	while( TCODSystem::waitForEvent(TCOD_EVENT_KEY_PRESS, &key, NULL, true) ) {
		if( key.vk == TCODK_ESCAPE ) break;
		switch( key.vk ) {
			case TCODK_ESCAPE: break;
			case TCODK_CHAR: {
				switch( key.c ) {
					case 'd': {
						std::cout << "Drop selected item" << std::endl;
						//Actor *actor = owner->container->inventory.get(cursor)
						//actor->item->drop(actor, owner);
						//engine.gameStatus = Engine::NEW_TURN;
						break;
					}
					case 'e': {
						std::cout << "Equip selected item" << std::endl;
						//Actor *actor = owner->container->inventory.get(cursor);

						break;
					}
					case 'u': {
						std::cout << "Use selected item" << std::endl;
						//Actor *actor = owner->container->inventory.get(cursor);
						//actor->item->use(actor, owner);
						//engine.gameStatus = Engine::NEW_TURN;
						break;
					}
					case 'w': {
						std::cout << "Wield selected item" << std::endl;
						//Actor *actor = owner->container->inventory.get(cursor);
						//actor->attacker->wield(actor, owner);
						//engine.gameStatus = Engine::NEW_TURN;						
						break;
					}
					default: break;
				}
			}
			default: break;
		}
	}
	return NULL;
}

void PlayerAi::helpScreen() {
	static const int HELP_WIDTH = 40;
	static const int HELP_HEIGHT = 20;
	static TCODConsole con(HELP_WIDTH, HELP_HEIGHT);
	int y = 2;

	// display the help frame
	con.setDefaultForeground(TCODColor(200, 180, 50));
	con.printFrame(0, 0, HELP_WIDTH, HELP_HEIGHT, true, TCOD_BKGND_DEFAULT, "Help");

	// display the help screen
	con.setDefaultForeground(TCODColor::white);
	con.print(2, y++, "%s", "Command List");
	con.hline(2, y++, 12);
	y++;

	// content of help screen
	con.print(2, y++, "%s", "Movement : Arrow Keys or IJKL");
	con.print(2, y++, "%s", "Inventory: Escape");
	con.print(2, y++, "%s", "Grab     : g");
	con.print(2, y++, "%s", "Drop     : d (From Inventory)");
	con.print(2, y++, "%s", "Equip    : e (From Inventory)");
	con.print(2, y++, "%s", "Wield    : w (From Inventory)");
	con.print(2, y++, "%s", "Use      : u (From Inventory)");
	con.print(2, y++, "%s", "Quit     : Close Window");
	con.print(2, y++, "%s", "Help     : ?");

	// blit the help console on the root console
	TCODConsole::blit(&con, 0, 0, HELP_WIDTH,HELP_HEIGHT, TCODConsole::root, engine.windowWidth/2 - HELP_WIDTH/2, engine.windowHeight/2 - HELP_HEIGHT/2);
	TCODConsole::flush();

	// wait for a key press
	TCOD_key_t key;
	TCODSystem::waitForEvent(TCOD_EVENT_KEY_PRESS, &key, NULL, true);
}