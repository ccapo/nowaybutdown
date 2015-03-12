#include <stdio.h>
#include <math.h>
#include "Main.hpp"

// How many turns the monster chases the player after losing his sight
static const int TRACKING_TURNS = 3;

MonsterAi::MonsterAi() : moveCount(0) {}

void MonsterAi::update(Object *owner) {
    if ( owner->entity && owner->entity->isDead() ) {
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

void MonsterAi::moveOrAttack(Object *owner, int targetx, int targety) {
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
	} else if ( owner->entity ) {
		owner->entity->attack(owner, engine.player);
	}
}

ConfusedMonsterAi::ConfusedMonsterAi(int nTurns, Ai *prevAi) : nTurns(nTurns), prevAi(prevAi) {}

void ConfusedMonsterAi::update(Object *owner) {
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
			Object *object = engine.getObject(destx, desty);
			if ( object ) {
				owner->entity->attack(owner, object);
			}
		}
	}
	nTurns--;
	if ( nTurns == 0 ) {
		owner->entity->ai = prevAi;
		delete this;
	}
}

void PlayerAi::update(Object *owner) {
    if ( owner->entity && owner->entity->isDead() ) {
		engine.gameStatus = Engine::DEFEAT;
    	return;
    }
	int dx = 0, dy = 0;
	switch(engine.lastKey.vk) {
		case TCODK_UP : dy = -1; break;
		case TCODK_DOWN : dy = 1; break;
		case TCODK_LEFT : dx = -1; break;
		case TCODK_RIGHT : dx = 1; break;
		case TCODK_ENTER :
		{
			// We have to go deeper!
			if( owner->x == engine.tunnel->x && owner->y == engine.tunnel->y ) {
				engine.gui->message(TCODColor::lightYellow, "You tumble deeper into the cave");
				engine.gui->message(TCODColor::lightYellow, "Unfortunately you cannot go back the way you came!");
			}
			break;
		}
		case TCODK_ESCAPE :
		{
			// display inventory
			choseFromInventory(owner);
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

bool PlayerAi::moveOrAttack(Object *owner, int targetx, int targety) {
	if ( engine.map->isWall(targetx, targety) ) return false;
	// look for living objects to attack
	for (Object **iterator = engine.objects.begin(); iterator != engine.objects.end(); iterator++) {
		Object *object = *iterator;
		if ( object->entity && !object->entity->isDead() && object->x == targetx && object->y == targety ) {
			owner->entity->attack(owner, object);
			return false;
		}
	}
	// look for corpses or items
	for (Object **iterator=engine.objects.begin(); iterator != engine.objects.end(); iterator++) {
		Object *object = *iterator;
		bool corpseOrItem = (object->entity && object->entity->isDead()) || object->item;
		if ( corpseOrItem && object->x == targetx && object->y == targety ) {
			engine.gui->message(TCODColor::lightGrey, "There's a %s here.", object->name);
		}
	}
	owner->x = targetx;
	owner->y = targety;
	return true;
}

void PlayerAi::handleActionKey(Object *owner, int ascii, int &dx, int &dy) {
	switch(ascii) {
		case 'i' : dy = -1; break; 
		case 'k' : dy = 1; break;
		case 'j' : dx = -1; break;
		case 'l' : dx = 1; break;
		case 'g' : // grab item
		{
			bool found=false;
			for (Object **iterator = engine.objects.begin(); iterator != engine.objects.end(); iterator++) {
				Object *object = *iterator;
				if ( object->item && object->x == owner->x && object->y == owner->y ) {
					if (object->item->grab(object,owner)) {
						found = true;
						engine.gui->message(TCODColor::lightGrey, "You grab the %s.", object->name);
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
		case '?' : // help screen
		{
			helpScreen();
			break;
		}
		default: break;
	}
}

void PlayerAi::choseFromInventory(Object *owner) {
	static const int INVENTORY_WIDTH = 50;
	static const int INVENTORY_HEIGHT = 28;
	static TCODConsole con(INVENTORY_WIDTH, INVENTORY_HEIGHT);
	static int cursor = 0;

	con.setAlignment(TCOD_LEFT);
	con.setBackgroundFlag(TCOD_BKGND_SET);
	con.setDefaultBackground(TCODColor::black);
	con.setDefaultForeground(TCODColor::white);
	con.clear();

	// display the inventory frame
	con.setDefaultForeground(TCODColor(200,180,50));
	con.printFrame(0, 0, INVENTORY_WIDTH, INVENTORY_HEIGHT, true, TCOD_BKGND_DEFAULT, "Inventory");

	int y = 2;
	con.setDefaultForeground(TCODColor::white);
	con.print(2, y,    "HP: %d/%d", engine.player->entity->hp, engine.player->entity->hpMax);
	con.print(22, y,   "ATK: %d", engine.player->entity->atk);
	con.print(32, y++, "DEF: %d", engine.player->entity->def);
	con.print(2,  y,   "Drop: d ");
	con.print(12, y,   "Use: u  ");
	con.print(22, y,   "Equip: e");
	con.print(32, y++, "Wield: w");
	con.hline(2, y++, 46);

	// Set text colours
	TCODConsole::setColorControl(TCOD_COLCTRL_1, TCODColor::white, TCODColor::black);
	TCODConsole::setColorControl(TCOD_COLCTRL_2, TCODColor::white, TCODColor::lightBlue);
	TCODConsole::setColorControl(TCOD_COLCTRL_3, TCODColor::lighterYellow, TCODColor::black);
	TCODConsole::setColorControl(TCOD_COLCTRL_4, TCODColor::red, TCODColor::black);

	// display the items with the cursor
	con.setDefaultForeground(TCODColor::white);
	y = 5;
	for (int i = 0; i < owner->container->inventory.size(); i++) {
		Object *object = owner->container->inventory.get(i);
		std::string name = "%c";
		name.append(object->name);
		name.append("%c");
		if( i == cursor ) {
			con.print(2, y++, name.c_str(), TCOD_COLCTRL_2, TCOD_COLCTRL_STOP);
		} else {
			con.print(2, y++, name.c_str(), TCOD_COLCTRL_3, TCOD_COLCTRL_STOP);
		}
	}

	// blit the inventory console on the root console
	TCODConsole::blit(&con, 0, 0, INVENTORY_WIDTH, INVENTORY_HEIGHT, TCODConsole::root, engine.windowWidth/2 - INVENTORY_WIDTH/2, engine.windowHeight/2 - INVENTORY_HEIGHT/2);
	TCODConsole::flush();

	// wait for a key press
	TCOD_key_t key;
	while( TCODSystem::waitForEvent(TCOD_EVENT_KEY_PRESS, &key, NULL, true) ) {
		if( key.vk == TCODK_ESCAPE ) {
			cursor = 0;
			break;
		}
		switch( key.vk ) {
			case TCODK_DOWN:
			{
				// Move the cursor position down
				if( owner->container->inventory.size() > 0 ) cursor = (cursor + 1) % owner->container->inventory.size();
				break;
			}
			case TCODK_UP:
			{
				// Move the cursor position up
				if( owner->container->inventory.size() > 0 ) {
					cursor--;
					if(cursor < 0) cursor = owner->container->inventory.size() - 1;
				}
				break;
			}
			case TCODK_CHAR: {
				switch( key.c ) {
					case 'd': {
						if( owner->container->inventory.size() > 0 ) {
							Object *object = owner->container->inventory.get(cursor);
							object->item->drop(object, owner);
							cursor--;
							if(cursor < 0) cursor = owner->container->inventory.size() - 1;
							engine.gameStatus = Engine::NEW_TURN;
						}
						break;
					}
					case 'e': {
						std::cout << "Equip selected item" << std::endl;
						//Object *object = owner->container->inventory.get(cursor);
						break;
					}
					case 'u': {
						if( owner->container->inventory.size() > 0 ) {
							Object *object = owner->container->inventory.get(cursor);
							if( object->item->use(object, owner) ) {
								cursor--;
								if(cursor < 0) cursor = owner->container->inventory.size() - 1;
								engine.gameStatus = Engine::NEW_TURN;
							}
						}
						break;
					}
					case 'w': {
						std::cout << "Wield selected item" << std::endl;
						//Object *object = owner->container->inventory.get(cursor);
						//object->entity->wield(object, owner);
						//engine.gameStatus = Engine::NEW_TURN;						
						break;
					}
					default: break;
				}
			}
			default: break;
		}

		con.clear();

		// display the inventory frame
		con.setDefaultForeground(TCODColor(200,180,50));
		con.printFrame(0, 0, INVENTORY_WIDTH, INVENTORY_HEIGHT, true, TCOD_BKGND_DEFAULT, "Inventory");

		y = 2;
		con.setDefaultForeground(TCODColor::white);
		con.print(2, y,    "HP: %d/%d", engine.player->entity->hp, engine.player->entity->hpMax);
		con.print(22, y,   "ATK: %d", engine.player->entity->atk);
		con.print(32, y++, "DEF: %d", engine.player->entity->def);
		con.print(2,  y,   "Drop: d ");
		con.print(12, y,   "Use: u  ");
		con.print(22, y,   "Equip: e");
		con.print(32, y++, "Wield: w");
		con.hline(2, y++, 46);

		// display the items with the cursor
		con.setDefaultForeground(TCODColor::white);
		y = 5;
		for (int i = 0; i < owner->container->inventory.size(); i++) {
			Object *object = owner->container->inventory.get(i);
			std::string name = "%c";
			name.append(object->name);
			name.append("%c");
			if( i == cursor ) {
				con.print(2, y++, name.c_str(), TCOD_COLCTRL_2, TCOD_COLCTRL_STOP);
			} else {
				con.print(2, y++, name.c_str(), TCOD_COLCTRL_3, TCOD_COLCTRL_STOP);
			}
		}

		// blit the inventory console on the root console
		TCODConsole::blit(&con, 0, 0, INVENTORY_WIDTH, INVENTORY_HEIGHT, TCODConsole::root, engine.windowWidth/2 - INVENTORY_WIDTH/2, engine.windowHeight/2 - INVENTORY_HEIGHT/2);
		TCODConsole::flush();

	}
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
	con.print(2, y++, "%s", "Use      : u (From Inventory)");
	con.print(2, y++, "%s", "Equip    : e (From Inventory)");
	con.print(2, y++, "%s", "Wield    : w (From Inventory)");
	con.print(2, y++, "%s", "Quit     : Close Window");
	con.print(2, y++, "%s", "Help     : ?");

	// blit the help console on the root console
	TCODConsole::blit(&con, 0, 0, HELP_WIDTH,HELP_HEIGHT, TCODConsole::root, engine.windowWidth/2 - HELP_WIDTH/2, engine.windowHeight/2 - HELP_HEIGHT/2);
	TCODConsole::flush();

	// wait for a key press
	TCOD_key_t key;
	TCODSystem::waitForEvent(TCOD_EVENT_KEY_PRESS, &key, NULL, true);
}
