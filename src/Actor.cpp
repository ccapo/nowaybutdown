#include <math.h>
#include "Main.hpp"

Actor::Actor(int x, int y, int ch, const char *name, const TCODColor &col) :
    x(x), y(y), ch(ch), col(col), name(name), blocks(true), wielding(false), wearing(false),
    attacker(NULL), destructible(NULL), ai(NULL), item(NULL), container(NULL) {}
 
Actor::~Actor() {
	if ( attacker ) delete attacker;
	if ( destructible ) delete destructible;
	if ( ai ) delete ai;
	if ( item ) delete item;
	if ( container ) delete container;
}

void Actor::render() const {
	int cx = x - engine.map->display_x, cy = y - engine.map->display_y;
    TCODConsole::root->setChar(cx, cy, ch);
    TCODConsole::root->setCharForeground(cx,  cy, col);
}

void Actor::update() {
	if ( ai ) ai->update(this);
}

float Actor::getDistance(int cx, int cy) const {
	int dx = x - cx;
	int dy = y - cy;
	return sqrtf(dx*dx + dy*dy);
}