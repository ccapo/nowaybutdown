#include <math.h>
#include "Main.hpp"

Engine::Engine(int windowWidth, int windowHeight) : gameStatus(STARTUP),fovRadius(8),
	windowWidth(windowWidth),windowHeight(windowHeight),displayWidth(windowWidth),displayHeight(windowHeight - 9),
	mapWidth(2*windowWidth),mapHeight(2*(windowHeight - 9)) {
	TCODConsole::setCustomFont("data/fonts/arial8x8-ext.png", TCOD_FONT_LAYOUT_TCOD | TCOD_FONT_TYPE_GREYSCALE, 32, 14);
    TCODConsole::initRoot(windowWidth,windowHeight,"NoWayButDown v0.1.0",false);
    TCODConsole::mapAsciiCodeToFont(256, 9, 10); // orc
    TCODConsole::mapAsciiCodeToFont(257, 0, 10); // troll
    TCODConsole::mapAsciiCodeToFont(258, 1, 9); // player
	TCODConsole::mapAsciiCodeToFont(259, 10, 8); // health potion
	TCODConsole::mapAsciiCodeToFont(260, 18, 10); // corpse
    player = new Actor(10,10,'@',"player",TCODColor::white);
    player->destructible=new PlayerDestructible(30,2,"your corpse");
    player->attacker=new Attacker(5);
    player->ai = new PlayerAi();
    player->container = new Container(26);
    actors.push(player);
    map = new Map(mapWidth,mapHeight);
    gui = new Gui();
    gui->message(TCODColor::red, "You decide to venture inside the cave" );
	gui->message(TCODColor::red, "Only to have the opening collapse behind you!" );
	gui->message(TCODColor::red, "Now what?!");
}

Engine::~Engine() {
    actors.clearAndDelete();
    delete map;
    delete gui;
}

void Engine::update() {
	if ( gameStatus == STARTUP ) map->computeFov();
   	gameStatus=IDLE;
    TCODSystem::checkForEvent(TCOD_EVENT_KEY_PRESS|TCOD_EVENT_MOUSE,&lastKey,&mouse);
    player->update();
    if ( gameStatus == NEW_TURN ) {
	    for (Actor **iterator=actors.begin(); iterator != actors.end();
	        iterator++) {
	        Actor *actor=*iterator;
	        if ( actor != player ) {
	            actor->update();
	        }
	    }
	}
}

void Engine::render() {
	TCODConsole::root->clear();
	// draw the map
	map->moveDisplay(player->x, player->y);
	map->render();
	// draw the actors
	for (Actor **iterator=actors.begin();
	    iterator != actors.end(); iterator++) {
		Actor *actor=*iterator;
		if ( actor != player && map->isInFov(actor->x,actor->y) ) {
	        actor->render();
	    }
	}
	player->render();
	// show the player's stats
	gui->render();
}

void Engine::sendToBack(Actor *actor) {
	actors.remove(actor);
	actors.insertBefore(actor,0);
}

Actor *Engine::getActor(int x, int y) const {
	for (Actor **iterator=actors.begin();
	    iterator != actors.end(); iterator++) {
		Actor *actor=*iterator;
		if ( actor->x == x && actor->y ==y && actor->destructible
			&& ! actor->destructible->isDead()) {
			return actor;
		}
	}
	return NULL;
}

Actor *Engine::getClosestMonster(int x, int y, float range) const {
	Actor *closest=NULL;
	float bestDistance=1E6f;
	for (Actor **iterator=actors.begin();
	    iterator != actors.end(); iterator++) {
		Actor *actor=*iterator;
		if ( actor != player && actor->destructible 
			&& !actor->destructible->isDead() ) {
			float distance=actor->getDistance(x,y);
			if ( distance < bestDistance && ( distance <= range || range == 0.0f ) ) {
				bestDistance=distance;
				closest=actor;
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
