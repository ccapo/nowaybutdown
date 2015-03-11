#include <math.h>
#include "Main.hpp"

Engine::Engine(int windowWidth, int windowHeight) : gameStatus(STARTUP),fovRadius(8),
	windowWidth(windowWidth),windowHeight(windowHeight),displayWidth(windowWidth),displayHeight(windowHeight - 9),
	mapWidth(2*windowWidth),mapHeight(2*(windowHeight - 9)) {
	TCODConsole::setCustomFont("data/fonts/arial8x8-ext.png", TCOD_FONT_LAYOUT_TCOD | TCOD_FONT_TYPE_GREYSCALE, 32, 14);
    TCODConsole::initRoot(windowWidth,windowHeight,"NoWayButDown v0.2.0",false);
    TCODSystem::setFps(24);
    TCODConsole::mapAsciiCodeToFont(256, 9, 10); // orc
    TCODConsole::mapAsciiCodeToFont(257, 0, 10); // troll
    TCODConsole::mapAsciiCodeToFont(258, 1, 9); // player
	TCODConsole::mapAsciiCodeToFont(259, 10, 8); // health potion
	TCODConsole::mapAsciiCodeToFont(260, 18, 10); // corpse
    player = new Object(10,10,'@',"Player",TCODColor::white);
    player->entity = new Entity(30, 5, 2, "your corpse");
    player->entity->ai = new PlayerAi();
    player->container = new Container(26);
    objects.push(player);
    map = new Map(mapWidth,mapHeight);
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

void Engine::update() {
	if ( gameStatus == STARTUP ) map->computeFov();
   	gameStatus=IDLE;
    TCODSystem::checkForEvent(TCOD_EVENT_KEY_PRESS|TCOD_EVENT_MOUSE,&lastKey,&mouse);
    player->update();
    if ( gameStatus == NEW_TURN ) {
	    for (Object **iterator=objects.begin(); iterator != objects.end();
	        iterator++) {
	        Object *object=*iterator;
	        if ( object != player ) {
	            object->update();
	        }
	    }
	}
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
