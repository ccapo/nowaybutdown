#include <math.h>
#include "Main.hpp"

Engine::Engine(int windowWidth, int windowHeight) : gameStatus(STARTUP),fovRadius(12),
	windowWidth(windowWidth),windowHeight(windowHeight),displayWidth(windowWidth),displayHeight(windowHeight - 9),
	mapWidth(2*windowWidth),mapHeight(2*(windowHeight - 9)), level(0), restart(false) {
	TCODConsole::setCustomFont("data/fonts/arial16x16-ext.png", TCOD_FONT_LAYOUT_TCOD | TCOD_FONT_TYPE_GREYSCALE, 32, 14);
    TCODConsole::initRoot(windowWidth,windowHeight,"NoWayButDown v0.3.0",false);
	int fpsMax = 24, initialDelay = 100, interval = 1000/MAX(10,fpsMax);
    TCODSystem::setFps(fpsMax);
	TCODConsole::setKeyboardRepeat(initialDelay, interval);
	TCODMouse::showCursor(true);

	// Assign extra ascii keys
	int x = 0, y = 8;
	TCODConsole::mapAsciiCodeToFont(CHAR_STAIRS_UP, x++, y);
	TCODConsole::mapAsciiCodeToFont(CHAR_STAIRS_DOWN, x++, y);

	TCODConsole::mapAsciiCodeToFont(CHAR_HOLE, x++, y);

	TCODConsole::mapAsciiCodeToFont(CHAR_WATER_01, x++, y);
	TCODConsole::mapAsciiCodeToFont(CHAR_WATER_02, x++, y);

	TCODConsole::mapAsciiCodeToFont(CHAR_LAVA_01, x++, y);
	TCODConsole::mapAsciiCodeToFont(CHAR_LAVA_02, x++, y);

	TCODConsole::mapAsciiCodeToFont(CHAR_CHEST_OPEN, x++, y);
	TCODConsole::mapAsciiCodeToFont(CHAR_CHEST_CLOSED, x++, y);

	TCODConsole::mapAsciiCodeToFont(CHAR_POTION_YELLOW, x++, y);
	TCODConsole::mapAsciiCodeToFont(CHAR_POTION_RED, x++, y);
	TCODConsole::mapAsciiCodeToFont(CHAR_POTION_GREEN, x++, y);
	TCODConsole::mapAsciiCodeToFont(CHAR_POTION_BLUE, x++, y);

	TCODConsole::mapAsciiCodeToFont(CHAR_KEY, x++, y);

	TCODConsole::mapAsciiCodeToFont(CHAR_RING_RED, x++, y);
	TCODConsole::mapAsciiCodeToFont(CHAR_RING_GREEN, x++, y);
	TCODConsole::mapAsciiCodeToFont(CHAR_RING_BLUE, x++, y);
	TCODConsole::mapAsciiCodeToFont(CHAR_RING_RED_BIG, x++, y);
	TCODConsole::mapAsciiCodeToFont(CHAR_RING_GREEN_BIG, x++, y);
	TCODConsole::mapAsciiCodeToFont(CHAR_RING_BLUE_BIG, x++, y);

	TCODConsole::mapAsciiCodeToFont(CHAR_SHIELD_BROWN, x++, y);
	TCODConsole::mapAsciiCodeToFont(CHAR_SHIELD_GREY, x++, y);
	TCODConsole::mapAsciiCodeToFont(CHAR_SHIELD_GOLD, x++, y);

	TCODConsole::mapAsciiCodeToFont(CHAR_SWORD_BASIC, x++, y);
	TCODConsole::mapAsciiCodeToFont(CHAR_SWORD_STEEL, x++, y);
	TCODConsole::mapAsciiCodeToFont(CHAR_SWORD_GOLD, x++, y);

	TCODConsole::mapAsciiCodeToFont(CHAR_ARMOUR_BROWN, x++, y);
	TCODConsole::mapAsciiCodeToFont(CHAR_ARMOUR_YELLOW, x++, y);
	TCODConsole::mapAsciiCodeToFont(CHAR_ARMOUR_RED, x++, y);
	TCODConsole::mapAsciiCodeToFont(CHAR_ARMOUR_GREEN, x++, y);
	TCODConsole::mapAsciiCodeToFont(CHAR_ARMOUR_BLUE, x++, y);
	TCODConsole::mapAsciiCodeToFont(CHAR_ARMOUR_MITHRIL, x++, y);
	x = 0; y++;

	TCODConsole::mapAsciiCodeToFont(CHAR_CHARGEBAR, x++, y);

	TCODConsole::mapAsciiCodeToFont(CHAR_PLAYER_RIGHT, x++, y);
	TCODConsole::mapAsciiCodeToFont(CHAR_PLAYER_DOWN, x++, y);
	TCODConsole::mapAsciiCodeToFont(CHAR_PLAYER_LEFT, x++, y);
	TCODConsole::mapAsciiCodeToFont(CHAR_PLAYER_UP, x++, y);
	TCODConsole::mapAsciiCodeToFont(CHAR_GUARDIAN, x++, y);
	TCODConsole::mapAsciiCodeToFont(CHAR_KEEPER, x++, y);
	TCODConsole::mapAsciiCodeToFont(CHAR_PERSON_MALE, x++, y);
	TCODConsole::mapAsciiCodeToFont(CHAR_PERSON_FEMALE, x++, y);

	TCODConsole::mapAsciiCodeToFont(CHAR_GUARD, x++, y);

	TCODConsole::mapAsciiCodeToFont(CHAR_WARLOCK_PURPLE, x++, y);

	TCODConsole::mapAsciiCodeToFont(CHAR_NECROMANCER_APPENTICE, x++, y);
	TCODConsole::mapAsciiCodeToFont(CHAR_NECROMANCER_MASTER, x++, y);

	TCODConsole::mapAsciiCodeToFont(CHAR_DARKELF_ARCHER, x++, y);
	TCODConsole::mapAsciiCodeToFont(CHAR_DARKELF_WARRIOR, x++, y);
	TCODConsole::mapAsciiCodeToFont(CHAR_DARKELF_MAGE, x++, y);

	TCODConsole::mapAsciiCodeToFont(CHAR_DWARF_WARRIOR, x++, y);
	TCODConsole::mapAsciiCodeToFont(CHAR_DWARF_AXEBEARER, x++, y);
	TCODConsole::mapAsciiCodeToFont(CHAR_DWARF_MAGE, x++, y);
	TCODConsole::mapAsciiCodeToFont(CHAR_DWARF_HERO, x++, y);

	TCODConsole::mapAsciiCodeToFont(CHAR_UNDEAD_DWARF_WARRIOR, x++, y);
	TCODConsole::mapAsciiCodeToFont(CHAR_UNDEAD_DWARF_AXEBEARER, x++, y);
	TCODConsole::mapAsciiCodeToFont(CHAR_UNDEAD_DWARF_MAGE, x++, y);
	TCODConsole::mapAsciiCodeToFont(CHAR_UNDEAD_DWARF_HERO, x++, y);

	TCODConsole::mapAsciiCodeToFont(CHAR_GOBLIN_PEON, x++, y);
	TCODConsole::mapAsciiCodeToFont(CHAR_GOBLIN_WARRIOR, x++, y);
	TCODConsole::mapAsciiCodeToFont(CHAR_GOBLIN_MAGE, x++, y);

	TCODConsole::mapAsciiCodeToFont(CHAR_IMP_BLUE, x++, y);
	TCODConsole::mapAsciiCodeToFont(CHAR_IMP_RED, x++, y);
	x = 0; y++;

	TCODConsole::mapAsciiCodeToFont(CHAR_ORGE_PEON_GREEN, x++, y);
	TCODConsole::mapAsciiCodeToFont(CHAR_ORGE_WARRIOR_GREEN, x++, y);

	TCODConsole::mapAsciiCodeToFont(CHAR_ORGE_PEON_RED, x++, y);
	TCODConsole::mapAsciiCodeToFont(CHAR_ORGE_WARRIOR_RED, x++, y);

	TCODConsole::mapAsciiCodeToFont(CHAR_SKELETON_PEON, x++, y);
	TCODConsole::mapAsciiCodeToFont(CHAR_SKELETON_WARRIOR, x++, y);
	TCODConsole::mapAsciiCodeToFont(CHAR_SKELETON_HERO, x++, y);
	TCODConsole::mapAsciiCodeToFont(CHAR_SKELETON_MAGE, x++, y);

	TCODConsole::mapAsciiCodeToFont(CHAR_SPRITE, x++, y);

	TCODConsole::mapAsciiCodeToFont(CHAR_ORC_PEON, x++, y);
	TCODConsole::mapAsciiCodeToFont(CHAR_ORC_WARRIOR, x++, y);
	TCODConsole::mapAsciiCodeToFont(CHAR_ORC_HERO, x++, y);
	TCODConsole::mapAsciiCodeToFont(CHAR_ORC_MAGE, x++, y);

	TCODConsole::mapAsciiCodeToFont(CHAR_DEMON_PEON, x++, y);
	TCODConsole::mapAsciiCodeToFont(CHAR_DEMON_HERO, x++, y);
	TCODConsole::mapAsciiCodeToFont(CHAR_DEMON_MAGE, x++, y);

	TCODConsole::mapAsciiCodeToFont(CHAR_FLAYER_WARRIOR, x++, y);
	TCODConsole::mapAsciiCodeToFont(CHAR_FLAYER_MAGE, x++, y);

	TCODConsole::mapAsciiCodeToFont(CHAR_SKULL, x++, y);

	TCODConsole::mapAsciiCodeToFont(CHAR_GOLEM_GREY, x++, y);
	TCODConsole::mapAsciiCodeToFont(CHAR_GOLEM_BROWN, x++, y);
	TCODConsole::mapAsciiCodeToFont(CHAR_GOLEM_RED, x++, y);

	TCODConsole::mapAsciiCodeToFont(CHAR_SLIME_BROWN, x++, y);
	TCODConsole::mapAsciiCodeToFont(CHAR_SLIME_GREEN, x++, y);

	TCODConsole::mapAsciiCodeToFont(CHAR_EYEBALL, x++, y);

	TCODConsole::mapAsciiCodeToFont(CHAR_VERMIN_BROWN, x++, y);

	TCODConsole::mapAsciiCodeToFont(CHAR_SNAKE_GREEN, x++, y);

	TCODConsole::mapAsciiCodeToFont(CHAR_RUBBLE_PILE, x++, y);
		x = 0; y++;

	TCODConsole::mapAsciiCodeToFont(CHAR_SCORPIAN_YELLOW, x++, y);
	TCODConsole::mapAsciiCodeToFont(CHAR_SCORPIAN_BLACK, x++, y);

	TCODConsole::mapAsciiCodeToFont(CHAR_SPIDER_BLACK, x++, y);
	TCODConsole::mapAsciiCodeToFont(CHAR_SPIDER_RED, x++, y);
	TCODConsole::mapAsciiCodeToFont(CHAR_SPIDER_GREEN, x++, y);

	TCODConsole::mapAsciiCodeToFont(CHAR_PYTHON_RED, x++, y);
	TCODConsole::mapAsciiCodeToFont(CHAR_PYTHON_BROWN, x++, y);
	TCODConsole::mapAsciiCodeToFont(CHAR_PYTHON_YELLOW, x++, y);

	TCODConsole::mapAsciiCodeToFont(CHAR_BAT_BROWN, x++, y);
	TCODConsole::mapAsciiCodeToFont(CHAR_BAT_BLUE, x++, y);

	// Environment Tiles
	TCODConsole::mapAsciiCodeToFont(CHAR_TREE_A, x++, y);
	TCODConsole::mapAsciiCodeToFont(CHAR_TREE_B, x++, y);
	TCODConsole::mapAsciiCodeToFont(CHAR_TREE_C, x++, y);
	TCODConsole::mapAsciiCodeToFont(CHAR_TREE_D, x++, y);
	TCODConsole::mapAsciiCodeToFont(CHAR_TREE_E, x++, y);
	TCODConsole::mapAsciiCodeToFont(CHAR_TREE_F, x++, y);
	TCODConsole::mapAsciiCodeToFont(CHAR_TREE_G, x++, y);

	TCODConsole::mapAsciiCodeToFont(CHAR_SHRUB_A, x++, y);
	TCODConsole::mapAsciiCodeToFont(CHAR_SHRUB_B, x++, y);
	TCODConsole::mapAsciiCodeToFont(CHAR_SHRUB_C, x++, y);
	TCODConsole::mapAsciiCodeToFont(CHAR_SHRUB_D, x++, y);
	TCODConsole::mapAsciiCodeToFont(CHAR_MUSHROOM, x++, y);
	TCODConsole::mapAsciiCodeToFont(CHAR_FLOWERS_WHITE, x++, y);
	TCODConsole::mapAsciiCodeToFont(CHAR_FLOWERS_BLUE, x++, y);

	TCODConsole::mapAsciiCodeToFont(CHAR_TEMPLE, x++, y);
	TCODConsole::mapAsciiCodeToFont(CHAR_TOWN, x++, y);
	TCODConsole::mapAsciiCodeToFont(CHAR_CAVE, x++, y);

	TCODConsole::mapAsciiCodeToFont(CHAR_BED, x++, y);
	TCODConsole::mapAsciiCodeToFont(CHAR_TABLE, x++, y);
	TCODConsole::mapAsciiCodeToFont(CHAR_BOOKCASE, x++, y);
	TCODConsole::mapAsciiCodeToFont(CHAR_CHAIR_RIGHT, x++, y);
	TCODConsole::mapAsciiCodeToFont(CHAR_CHAIR_LEFT, x++, y);
	x = 0; y++;

	// Minor Bosses (Upper Portion)
	TCODConsole::mapAsciiCodeToFont(CHAR_DEMONLORD_WHITE_UL, x++, y);
	TCODConsole::mapAsciiCodeToFont(CHAR_DEMONLORD_WHITE_UR, x++, y);

	TCODConsole::mapAsciiCodeToFont(CHAR_DEMONLORD_RED_UL, x++, y);
	TCODConsole::mapAsciiCodeToFont(CHAR_DEMONLORD_RED_UR, x++, y);

	TCODConsole::mapAsciiCodeToFont(CHAR_CYCLOPS_UL, x++, y);
	TCODConsole::mapAsciiCodeToFont(CHAR_CYCLOPS_UR, x++, y);

	TCODConsole::mapAsciiCodeToFont(CHAR_DRAGON_LARGE_RED_UL, x++, y);
	TCODConsole::mapAsciiCodeToFont(CHAR_DRAGON_LARGE_RED_UR, x++, y);

	TCODConsole::mapAsciiCodeToFont(CHAR_DRAGON_LARGE_YELLOW_UL, x++, y);
	TCODConsole::mapAsciiCodeToFont(CHAR_DRAGON_LARGE_YELLOW_UR, x++, y);

	TCODConsole::mapAsciiCodeToFont(CHAR_DRAGON_LARGE_GREEN_UL, x++, y);
	TCODConsole::mapAsciiCodeToFont(CHAR_DRAGON_LARGE_GREEN_UR, x++, y);

	TCODConsole::mapAsciiCodeToFont(CHAR_DRAGON_LARGE_BLUE_UL, x++, y);
	TCODConsole::mapAsciiCodeToFont(CHAR_DRAGON_LARGE_BLUE_UR, x++, y);

	TCODConsole::mapAsciiCodeToFont(CHAR_DRAGON_LARGE_GREY_UL, x++, y);
	TCODConsole::mapAsciiCodeToFont(CHAR_DRAGON_LARGE_GREY_UR, x++, y);

	TCODConsole::mapAsciiCodeToFont(CHAR_MINOTAUR_UL, x++, y);
	TCODConsole::mapAsciiCodeToFont(CHAR_MINOTAUR_UR, x++, y);

	TCODConsole::mapAsciiCodeToFont(CHAR_LIZARD_GIANT_UL, x++, y);
	TCODConsole::mapAsciiCodeToFont(CHAR_LIZARD_GIANT_UR, x++, y);

	TCODConsole::mapAsciiCodeToFont(CHAR_MEDUSA_UL, x++, y);
	TCODConsole::mapAsciiCodeToFont(CHAR_MEDUSA_UR, x++, y);

	TCODConsole::mapAsciiCodeToFont(CHAR_FLYING_BRAIN_UL, x++, y);
	TCODConsole::mapAsciiCodeToFont(CHAR_FLYING_BRAIN_UR, x++, y);

	TCODConsole::mapAsciiCodeToFont(CHAR_SLIMELORD_UL, x++, y);
	TCODConsole::mapAsciiCodeToFont(CHAR_SLIMELORD_UR, x++, y);

	TCODConsole::mapAsciiCodeToFont(CHAR_BEHOLDER_UL, x++, y);
	TCODConsole::mapAsciiCodeToFont(CHAR_BEHOLDER_UR, x++, y);

	TCODConsole::mapAsciiCodeToFont(CHAR_BEHEMOTH_UL, x++, y);
	TCODConsole::mapAsciiCodeToFont(CHAR_BEHEMOTH_UR, x++, y);

	// Final Boss (Upper Portion)
	TCODConsole::mapAsciiCodeToFont(CHAR_FINAL_BOSS_UL, x++, y);
	TCODConsole::mapAsciiCodeToFont(CHAR_FINAL_BOSS_UR, x++, y);
	x = 0; y++;

	// Minor Bosses (Lower Portion)
	TCODConsole::mapAsciiCodeToFont(CHAR_DEMONLORD_WHITE_LL, x++, y);
	TCODConsole::mapAsciiCodeToFont(CHAR_DEMONLORD_WHITE_LR, x++, y);

	TCODConsole::mapAsciiCodeToFont(CHAR_DEMONLORD_RED_LL, x++, y);
	TCODConsole::mapAsciiCodeToFont(CHAR_DEMONLORD_RED_LR, x++, y);

	TCODConsole::mapAsciiCodeToFont(CHAR_CYCLOPS_LL, x++, y);
	TCODConsole::mapAsciiCodeToFont(CHAR_CYCLOPS_LR, x++, y);

	TCODConsole::mapAsciiCodeToFont(CHAR_DRAGON_LARGE_RED_LL, x++, y);
	TCODConsole::mapAsciiCodeToFont(CHAR_DRAGON_LARGE_RED_LR, x++, y);

	TCODConsole::mapAsciiCodeToFont(CHAR_DRAGON_LARGE_YELLOW_LL, x++, y);
	TCODConsole::mapAsciiCodeToFont(CHAR_DRAGON_LARGE_YELLOW_LR, x++, y);

	TCODConsole::mapAsciiCodeToFont(CHAR_DRAGON_LARGE_GREEN_LL, x++, y);
	TCODConsole::mapAsciiCodeToFont(CHAR_DRAGON_LARGE_GREEN_LR, x++, y);

	TCODConsole::mapAsciiCodeToFont(CHAR_DRAGON_LARGE_BLUE_LL, x++, y);
	TCODConsole::mapAsciiCodeToFont(CHAR_DRAGON_LARGE_BLUE_LR, x++, y);

	TCODConsole::mapAsciiCodeToFont(CHAR_DRAGON_LARGE_GREY_LL, x++, y);
	TCODConsole::mapAsciiCodeToFont(CHAR_DRAGON_LARGE_GREY_LR, x++, y);

	TCODConsole::mapAsciiCodeToFont(CHAR_MINOTAUR_LL, x++, y);
	TCODConsole::mapAsciiCodeToFont(CHAR_MINOTAUR_LR, x++, y);

	TCODConsole::mapAsciiCodeToFont(CHAR_LIZARD_GIANT_LL, x++, y);
	TCODConsole::mapAsciiCodeToFont(CHAR_LIZARD_GIANT_LR, x++, y);

	TCODConsole::mapAsciiCodeToFont(CHAR_MEDUSA_LL, x++, y);
	TCODConsole::mapAsciiCodeToFont(CHAR_MEDUSA_LR, x++, y);

	TCODConsole::mapAsciiCodeToFont(CHAR_FLYING_BRAIN_LL, x++, y);
	TCODConsole::mapAsciiCodeToFont(CHAR_FLYING_BRAIN_LR, x++, y);

	TCODConsole::mapAsciiCodeToFont(CHAR_SLIMELORD_LL, x++, y);
	TCODConsole::mapAsciiCodeToFont(CHAR_SLIMELORD_LR, x++, y);

	TCODConsole::mapAsciiCodeToFont(CHAR_BEHOLDER_LL, x++, y);
	TCODConsole::mapAsciiCodeToFont(CHAR_BEHOLDER_LR, x++, y);

	TCODConsole::mapAsciiCodeToFont(CHAR_BEHEMOTH_LL, x++, y);
	TCODConsole::mapAsciiCodeToFont(CHAR_BEHEMOTH_LR, x++, y);

	// Final Boss (Lower Portion)
	TCODConsole::mapAsciiCodeToFont(CHAR_FINAL_BOSS_LL, x++, y);
	TCODConsole::mapAsciiCodeToFont(CHAR_FINAL_BOSS_LR, x++, y);

    player = new Object(0, 0, '@', "Player", TCODColor::white);
    player->entity = new PlayerEntity(30, 5, 2, "your corpse");
    player->entity->ai = new PlayerAi();
    player->container = new Container(12);

	stairs = new Object(0, 0, CHAR_STAIRS_DOWN, "stairs down", TCODColor::white);
	stairs->blocks = false;
    stairs->entity = new Entity(0, 0, 0, "a stairs down");

	Object *object = new Object(0,0,CHAR_SWORD_BASIC,"sword", TCODColor::white);
	object->blocks = false;
	object->entity = new Entity(0, 2, 2, "a sword");
	object->item = new Equipment(Equipment::SWORD, 0, 2, 2);

    map = new Map(mapWidth, mapHeight);

	int px, py, dx, dy;
	map->generateMap(px, py, dx, dy);
	std::cout << "px, py = " << px << ", " << py << std::endl;
	std::cout << "dx, dy = " << dx << ", " << dy << std::endl;
	player->x = px; 
	player->y = py;
	stairs->x = dx;
	stairs->y = dy;
	object->x = px+1;object->y = py;

    objects.push(player);
	objects.push(stairs);
	objects.push(object);

    gui = new Gui();
    gui->message(TCODColor::red, "You decide to venture inside the cave" );
	gui->message(TCODColor::red, "Only to have the opening collapse behind you!" );
	gui->message(TCODColor::red, "Now what?!");
}

Engine::~Engine() {
    objects.clearAndDelete();
    delete map;
    delete gui;
}

bool Engine::update() {
	static const int DIALOG_WIDTH = 40;
	static const int DIALOG_HEIGHT = 20;
	static TCODConsole con(DIALOG_WIDTH, DIALOG_HEIGHT);

	if ( gameStatus == STARTUP ) map->computeFov();

   	gameStatus = IDLE;
    TCODSystem::checkForEvent(TCOD_EVENT_KEY_PRESS|TCOD_EVENT_MOUSE,&lastKey,&mouse);
    player->update();

	switch( gameStatus ) {
    	case NEW_TURN: {
	    	for (Object **iterator=objects.begin(); iterator != objects.end(); iterator++) {
	        	Object *object = *iterator;
	        	if ( object != player ) object->update();
	        }
			break;
	    }
		case VICTORY: {
			//gui->message(TCODColor::green, "You Won!!!\nThis extremely impressive since I didn't code an ending!");
			break;
		}
		case DEFEAT: {

			con.setAlignment(TCOD_CENTER);
			con.setBackgroundFlag(TCOD_BKGND_SET);
			con.setDefaultBackground(TCODColor::black);
			con.setDefaultForeground(TCODColor::white);
			con.clear();

			// display the inventory frame
			con.setDefaultForeground(TCODColor(200,180,50));
			con.printFrame(0, 0, DIALOG_WIDTH, DIALOG_HEIGHT, true, TCOD_BKGND_DEFAULT, restart ? "Defeat" : "Defeat?");

			con.setDefaultForeground(TCODColor::white);
			if( !restart ) {
				int y = DIALOG_HEIGHT/2 - 3;
				con.print(DIALOG_WIDTH/2, y++, "%s", "You Died");
				y++;
				con.print(DIALOG_WIDTH/2, y++, "%s", "Would you like to be continue?");
				y++;
				con.print(DIALOG_WIDTH/2, y++, "%s", "(Y)es or (N)o");
			} else {
				int y = DIALOG_HEIGHT/2 - 1;
				con.print(DIALOG_WIDTH/2, y++, "%s", "You Died.");
			}

			// blit the help console on the root console
			TCODConsole::blit(&con, 0, 0, DIALOG_WIDTH, DIALOG_HEIGHT, TCODConsole::root, engine.windowWidth/2 - DIALOG_WIDTH/2, engine.windowHeight/2 - DIALOG_HEIGHT/2);
			TCODConsole::flush();

			// wait for a key press
			TCOD_key_t key;
			TCODSystem::waitForEvent(TCOD_EVENT_KEY_PRESS, &key, NULL, true);
			if( restart && key.vk == TCODK_ENTER ) return false;
			if( key.c == 'y' || key.c == 'Y' ) {
				delete player->entity;
				player->entity = new PlayerEntity(15, 10, 1, "your corpse");
				player->entity->ai = new PlayerAi();
				player->name = "Re-animated Corpse";
				player->ch = '@';
				player->col = TCODColor::desaturatedGreen;
				player->blocks = true;
				gameStatus = STARTUP;
				restart = true;
			} else if( key.c == 'n' || key.c == 'N' ) {
				return false;
			}

			break;
		}
		default: break;
	}
	return true;
}

void Engine::render() {
	TCODConsole::root->clear();
	// draw the map
	map->moveDisplay(player->x, player->y);
	map->render();
	// draw the objects
	for (Object **iterator=objects.begin();
	    iterator != objects.end(); iterator++) {
		Object *object=*iterator;
		if ( object != player && map->isInFov(object->x,object->y) ) {
	        object->render();
	    }
	}
	player->render();
	// show the player's stats
	gui->render();
}

void Engine::sendToBack(Object *object) {
	objects.remove(object);
	objects.insertBefore(object,0);
}

Object *Engine::getObject(int x, int y) const {
	for (Object **iterator=objects.begin();
	    iterator != objects.end(); iterator++) {
		Object *object=*iterator;
		if ( object->x == x && object->y ==y && object->entity
			&& ! object->entity->isDead()) {
			return object;
		}
	}
	return NULL;
}

Object *Engine::getClosestMonster(int x, int y, float range) const {
	Object *closest=NULL;
	float bestDistance=1E6f;
	for (Object **iterator=objects.begin();
	    iterator != objects.end(); iterator++) {
		Object *object=*iterator;
		if ( object != player && object->entity 
			&& !object->entity->isDead() ) {
			float distance=object->getDistance(x,y);
			if ( distance < bestDistance && ( distance <= range || range == 0.0f ) ) {
				bestDistance=distance;
				closest=object;
			}
		}
	}
	return closest;
}

bool Engine::pickATile(int *x, int *y, float maxRange) {
	while ( !TCODConsole::isWindowClosed() ) {
		render();
		// highlight the possible range
		for (int cx=0; cx < map->width; cx++) {
			for (int cy=0; cy < map->height; cy++) {
				if ( map->isInFov(cx,cy)
					&& ( maxRange == 0 || player->getDistance(cx,cy) <= maxRange) ) {
					TCODColor col=TCODConsole::root->getCharBackground(cx,cy);
					col = col * 1.2f;
					TCODConsole::root->setCharBackground(cx,cy,col);
				}
			}
		}
		TCODSystem::checkForEvent(TCOD_EVENT_KEY_PRESS|TCOD_EVENT_MOUSE,&lastKey,&mouse);
		if ( map->isInFov(mouse.cx,mouse.cy)
			&& ( maxRange == 0 || player->getDistance(mouse.cx,mouse.cy) <= maxRange )) {
			TCODConsole::root->setCharBackground(mouse.cx,mouse.cy,TCODColor::white);
			if ( mouse.lbutton_pressed ) {
				*x=mouse.cx;
				*y=mouse.cy;
				return true;
			}
		} 
		if (mouse.rbutton_pressed || lastKey.vk != TCODK_NONE) {
			return false;
		}
		TCODConsole::flush();
	}
	return false;
}
