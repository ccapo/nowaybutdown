#include <math.h>
#include "Main.hpp"

Object::Object(int x, int y, int ch, const char *name, const TCODColor &col) :
    x(x), y(y), ch(ch), col(col), name(name), blocks(true), wielding(false), wearing(false),
    entity(NULL), item(NULL), container(NULL) {}
 
Object::~Object() {
	if ( entity ) delete entity;
	if ( item ) delete item;
	if ( container ) delete container;
}

void Object::render() const {
	int cx = x - engine.map->display_x, cy = y - engine.map->display_y;
    TCODConsole::root->setChar(cx, cy, ch);
    TCODConsole::root->setCharForeground(cx,  cy, col);
}

void Object::update() {
	if ( entity && entity->ai ) entity->ai->update(this);
}

float Object::getDistance(int cx, int cy) const {
	int dx = x - cx;
	int dy = y - cy;
	return sqrtf(dx*dx + dy*dy);
}
