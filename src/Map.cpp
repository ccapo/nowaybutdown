#include "Main.hpp"

static const int ROOM_MAX_SIZE = 12;
static const int ROOM_MIN_SIZE = 6;
static const int MAX_ROOM_MONSTERS = 3;
static const int MAX_ROOM_ITEMS = 2;

class BspListener : public ITCODBspCallback {
private :
    Map &map; // a map to dig
    int roomNum; // room number
    int lastx,lasty; // center of the last room
public :
    BspListener(Map &map) : map(map), roomNum(0) {}
    bool visitNode(TCODBsp *node, void *userData) {
    	if ( node->isLeaf() ) {    
    		int x,y,w,h;
			// dig a room
			TCODRandom *rng=TCODRandom::getInstance();
			w=rng->getInt(ROOM_MIN_SIZE, node->w-2);
			h=rng->getInt(ROOM_MIN_SIZE, node->h-2);
			x=rng->getInt(node->x+1, node->x+node->w-w-1);
			y=rng->getInt(node->y+1, node->y+node->h-h-1);
			map.createRoom(roomNum == 0, x, y, x+w-1, y+h-1);
			if ( roomNum != 0 ) {
			    // dig a corridor from last room
			    map.dig(lastx,lasty,x+w/2,lasty);
			    map.dig(x+w/2,lasty,x+w/2,y+h/2);
			}
            lastx=x+w/2;
            lasty=y+h/2;
            roomNum++;
        }
        return true;
    }
};

Map::Map(int width, int height) : width(width),height(height),display_x(0),display_y(0) {
    tiles=new Tile[width*height];
    map=new TCODMap(width,height);
    TCODBsp bsp(0,0,width,height);
    bsp.splitRecursive(NULL,8,ROOM_MAX_SIZE,ROOM_MAX_SIZE,1.5f,1.5f);
    BspListener listener(*this);
    bsp.traverseInvertedLevelOrder(&listener,NULL);
}

Map::~Map() {
    delete [] tiles;
    delete map;
}

void Map::dig(int x1, int y1, int x2, int y2) {
    if ( x2 < x1 ) {
        int tmp=x2;
        x2=x1;
        x1=tmp;
    }
    if ( y2 < y1 ) {
        int tmp=y2;
        y2=y1;
        y1=tmp;
    }
    for (int tilex=x1; tilex <= x2; tilex++) {
        for (int tiley=y1; tiley <= y2; tiley++) {
            map->setProperties(tilex,tiley,true,true);
        }
    }
}

void Map::addMonster(int x, int y) {
	TCODRandom *rng=TCODRandom::getInstance();
    if ( rng->getInt(0,100) < 80 ) {
        // create an orc
        Object *orc = new Object(x, y, 256, "Orc", TCODColor::white);
        orc->entity = new CreatureEntity(10, 3, 0, "dead orc");
        orc->entity->ai = new MonsterAi();
        engine.objects.push(orc);
    } else {
        // create a troll
        Object *troll = new Object(x, y, 257, "Troll", TCODColor::white);
        troll->entity = new CreatureEntity(16, 4, 1, "troll carcass");
        troll->entity->ai = new MonsterAi();
        engine.objects.push(troll);
    }
}

void Map::addItem(int x, int y) {
	TCODRandom *rng=TCODRandom::getInstance();
	int dice = rng->getInt(0,100);
	if ( dice < 70 ) {
		// create a health potion
		Object *healthPotion=new Object(x,y,259,"health potion",
			TCODColor::white);
		healthPotion->blocks=false;
		healthPotion->item=new Healer(4);
		engine.objects.push(healthPotion);
	} else if ( dice < 70+10 ) {
		// create a scroll of lightning bolt 
		Object *scrollOfLightningBolt=new Object(x,y,'#',"scroll of lightning bolt",
			TCODColor::lightYellow);
		scrollOfLightningBolt->blocks=false;
		scrollOfLightningBolt->item=new LightningBolt(5,20);
		engine.objects.push(scrollOfLightningBolt);
	} else if ( dice < 70+10+10 ) {
		// create a scroll of fireball
		Object *scrollOfFireball=new Object(x,y,'#',"scroll of fireball",
			TCODColor::lightYellow);
		scrollOfFireball->blocks=false;
		scrollOfFireball->item=new Fireball(3,12);
		engine.objects.push(scrollOfFireball);
	} else {
		// create a scroll of confusion
		Object *scrollOfConfusion=new Object(x,y,'#',"scroll of confusion",
			TCODColor::lightYellow);
		scrollOfConfusion->blocks=false;
		scrollOfConfusion->item=new Confuser(10,8);
		engine.objects.push(scrollOfConfusion);
	}
}

void Map::createRoom(bool first, int x1, int y1, int x2, int y2) {
    dig (x1,y1,x2,y2);
    if ( first ) {
        // put the player in the first room
        engine.player->x=(x1+x2)/2;
        engine.player->y=(y1+y2)/2;
    } else {
		TCODRandom *rng=TCODRandom::getInstance();
		// add monsters
		int nbMonsters=rng->getInt(0,MAX_ROOM_MONSTERS);
		while (nbMonsters > 0) {
		    int x=rng->getInt(x1,x2);
		    int y=rng->getInt(y1,y2);
    		if ( canWalk(x,y) ) {
				addMonster(x,y);
			}
		    nbMonsters--;
		}
		// add items
		int nbItems=rng->getInt(0,MAX_ROOM_ITEMS);
		while (nbItems > 0) {
		    int x=rng->getInt(x1,x2);
		    int y=rng->getInt(y1,y2);
    		if ( canWalk(x,y) ) {
				addItem(x,y);
			}
		    nbItems--;
		}
    }
}

bool Map::isWall(int x, int y) const {
    return !map->isWalkable(x,y);
}

bool Map::canWalk(int x, int y) const {
    if (isWall(x,y)) {
        // this is a wall
        return false;
    }
    for (Object **iterator=engine.objects.begin();
        iterator!=engine.objects.end();iterator++) {
        Object *object=*iterator;
        if ( object->blocks && object->x == x && object->y == y ) {
            // there is a blocking object here. cannot walk
            return false;
        }
    }
    return true;
}
 
bool Map::isExplored(int x, int y) const {
    return tiles[x+y*width].explored;
}

bool Map::isInFov(int x, int y) const {
	if ( x < 0 || x >= width || y < 0 || y >= height ) {
		return false;
	}
    if ( map->isInFov(x,y) ) {
        tiles[x+y*width].explored=true;
        return true;
    }
    return false;
}
 
void Map::computeFov() {
    map->computeFov(engine.player->x,engine.player->y,engine.fovRadius);
}

void Map::moveDisplay(int x, int y)
{
    // New display coordinates (top-left corner of the screen relative to the map)
    // Coordinates so that the target is at the center of the screen
    int cx = x - engine.displayWidth/2;
    int cy = y - engine.displayHeight/2;

    // Make sure the DISPLAY doesn't see outside the map
    if(cx < 0) cx = 0;
    if(cy < 0) cy = 0;
    if(cx > width - engine.displayWidth - 1) cx = width - engine.displayWidth - 1;
    if(cy > height - engine.displayHeight - 1) cy = height - engine.displayHeight - 1;

    display_x = cx; display_y = cy;
}

void Map::render() const {
    static const TCODColor darkWall = TCODColor::darkestGrey;
    static const TCODColor lightWall = TCODColor(130,110,50);
    static const TCODColor darkGround = TCODColor::darkerGrey;
    static const TCODColor lightGround = TCODColor(200,180,50);
    int posx = display_x, posy = display_y;

    // Torch intensity variation
    TCODRandom *rng=TCODRandom::getInstance();
    float ti = rng->getFloat(-0.15f,0.15f); 

	for (int x=posx; x < engine.displayWidth + posx + 1; x++) {
	    for (int y=posy; y < engine.displayHeight + posy + 1; y++) {
            // Cell distance to torch (squared)
            float dx = static_cast<float>(x - engine.player->x);
            float dy = static_cast<float>(y - engine.player->y);
            float dr2 = dx*dx + dy*dy;

	        if ( isInFov(x,y) ) {

                // Torch flickering FX
                TCODColor base = isWall(x,y) ? darkWall : darkGround;
                TCODColor light = isWall(x,y) ? lightWall : lightGround;

                // l = 1.0 at player position, 0.0 at a radius of TORCH_RADIUS cells
                float coef1 = 1.0f/(1.0f + dr2/20.0f);
                float coef2 = coef1 - 1.0f/(1.0f + engine.fovRadius*engine.fovRadius);
                float l = coef2/(1.0f - 1.0f/(1.0f + engine.fovRadius*engine.fovRadius)) + ti;
                l = CLAMP(0.0f, 1.0f, l);

                // Interpolate the colour
                TCODColor final = engine.player->entity->isDead() ? base : TCODColor::lerp(base, light, l);

	            TCODConsole::root->setCharBackground(x - posx, y - posy, final );
	        } else if ( isExplored(x,y) ) {
	            TCODConsole::root->setCharBackground(x - posx, y - posy, isWall(x,y) ? darkWall : darkGround );
	        }
   	    }
	}
}
