#include "Main.hpp"

Map::Map(int width, int height) : width(width),height(height),display_x(0),display_y(0) {
	darkWall = TCODColor::darkestGrey;
	lightWall = TCODColor(130,110,50);
	darkGround = TCODColor::darkerGrey;
	lightGround = TCODColor(200,180,50);

    tiles = new Tile[width*height];
    map = new TCODMap(width, height);
}

Map::~Map() {
    delete [] tiles;
    delete map;
}

// Builds the cave heightmap
void Map::generateHM(TCODHeightMap *hmap) {
	TCODRandom *rng = TCODRandom::getInstance();
	TCODNoise *terrain = new TCODNoise(2, rng);
	hmap->clear();
	//        mulx  muly  addx  addy  octaves  delta  scale
	hmap->addFbm(terrain,  8.0f,  8.0f,  0.0f,  0.0f,  8.0f,  0.0f,  1.0f);
	hmap->normalize(0.75, 0.95);
}

// Fill (over write) all pixels that are not the fill color
void Map::floodFill(int x, int y, TCODColor fill) {
  // Test tile colour
  int offset = x + width*y;
  if( tiles[offset].colour != fill ) {
    tiles[offset].colour = fill;
    map->setProperties(x, y, false, false);

    floodFill(x - 1, y    , fill);
    floodFill(x + 1, y    , fill);
    floodFill(x    , y - 1, fill);
    floodFill(x    , y + 1, fill);
  }
}

// Prepare a map
void Map::prepareMap() {
	TCODHeightMap *hmap = new TCODHeightMap(width, height);

	generateHM(hmap);

	for(int x = 0; x < 1; x++) {
		for(int y = 0; y < height; y++) {
			int offset = x + width*y;
			tiles[offset].colour = darkWall;
			map->setProperties(x, y, false, false);
		}
	}

	for(int x = width - 2; x < width; x++) {
		for(int y = 0; y < height; y++) {
			int offset = x + width*y;
			tiles[offset].colour = darkWall;
			map->setProperties(x, y, false, false);
		}
	}

	for(int y = 0; y < 1; y++) {
		for(int x = 0; x < width; x++) {
			int offset = x + width*y;
			tiles[offset].colour = darkWall;
			map->setProperties(x, y, false, false);
		}
	}

	for(int y = height - 2; y < height; y++) {
		for(int x = 0; x < width; x++) {
			int offset = x + width*y;
			tiles[offset].colour = darkWall;
			map->setProperties(x, y, false, false);
		}
	}

	for(int x = 1; x < width - 2; x++) {
		for(int y = 1; y < height - 2; y++) {
			int offset = x + width*y;
			float z = hmap->getValue(x, y);
			if( z >= 0.83 ) {
				float coef = (z - 0.75)/(0.95 - 0.75);
				tiles[offset].colour = TCODColor::lerp(darkWall, darkGround, coef);
				map->setProperties(x, y, true, true);
			} else {
				tiles[offset].colour = darkWall;
				map->setProperties(x, y, false, false);
			}
			tiles[offset].explored = false;
		}
	}
}

// Generate a map
void Map::generateMap(int &px, int &py, int &dx, int &dy) {
	TCODRandom *rng = TCODRandom::getInstance();
	TCODDijkstra *dijkstra;
	int nWalkable = 0, nIteration = 0;
	float fWalkable = 0.0f;

    engine.level++;

	while( fWalkable < 0.4f && nIteration < 10 ) {
		// Prepare a new cave map
		prepareMap();

		// Pick the starting location of the player
        px = rng->getInt(2, width - 3);
        py = rng->getInt(2, height - 3);
		while( isNotWalkable(px, py) ) {
			px = rng->getInt(2, width - 3);
			py = rng->getInt(2, height - 3);
        }

        dijkstra = new TCODDijkstra(map);  // allocate the path
        dijkstra->compute(px, py);    // calculate distance from (px, py) to all other nodes
        nWalkable = 0;

		for(int x = 1; x < width - 2; x++) {
			for(int y = 1; y < height - 2; y++) {
				if( !isWall(x, y) ) {
					if( !dijkstra->setPath(x, y) ) {
						floodFill(x, y, darkWall);
					} else {
						nWalkable++;
					}
				}
			}
		}
		fWalkable = static_cast<float>(nWalkable)/static_cast<float>(width*height);
		delete dijkstra;
        nIteration++;
        //std::cout << "Iteration, fWalkable = " << nIteration << ", " << fWalkable << "\n";
	}

	// Assign the location of the tunnel, ensuring they are sufficiently far apart
    nIteration = 0;
	dx = rng->getInt(2, width - 3);
	dy = rng->getInt(2, height - 3);
	int dp2 = pow(dx - px, 2) + pow(dy - py, 2);
	while( ( isNotWalkable(dx, dy) || ( dp2 < 8000 ) ) && nIteration < 10 )  {
		dx = rng->getInt(2, width - 3);
		dy = rng->getInt(2, height - 3);
		dp2 = pow(dx - px, 2) + pow(dy - py, 2);
        nIteration++;
	}

    int nitems = 12, ncreatures = 24, nequip = 4;
	for(int i = 0; i < nitems + ncreatures + nequip; i++) {
        nIteration = 0;
		int qx = rng->getInt(2, width - 3);
		int qy = rng->getInt(2, height - 3);
		int qp2 = pow(qx - px, 2) + pow(qy - py, 2);
		int qd2 = pow(qx - dx, 2) + pow(qy - dy, 2);
		while( ( isNotWalkable(qx, qy) || ( qp2 < 225 ) || ( qd2 < 225 ) ) && nIteration < 10 ) {
			qx = rng->getInt(2, width - 3);
			qy = rng->getInt(2, height - 3);
			qp2 = pow(qx - px, 2) + pow(qy - py, 2);
			qd2 = pow(qx - dx, 2) + pow(qy - dy, 2);
		}

		if( i < nitems ) {
			addItem(qx, qy);
		} else if( i >= nitems && i < nitems + ncreatures ) {
			addCreature(qx, qy);
		} else {
            addEquipment(qx, qy);
        }
	}
}

void Map::addCreature(int x, int y) {
	TCODRandom *rng = TCODRandom::getInstance();
    if ( rng->getInt(0,100) < 80 ) {
        // Create an Orc
        Object *object = new Object(x, y, CHAR_ORC_PEON, "Orc", TCODColor::white);
        object->entity = new CreatureEntity(10, 3, 0, "dead orc");
        object->entity->ai = new MonsterAi();
        engine.objects.push(object);
    } else {
        // Create an Ogre
        Object *object = new Object(x, y, CHAR_ORGE_PEON_GREEN, "Ogre", TCODColor::white);
        object->entity = new CreatureEntity(16, 4, 1, "ogre carcass");
        object->entity->ai = new MonsterAi();
        engine.objects.push(object);
    }
}

void Map::addItem(int x, int y) {
	TCODRandom *rng = TCODRandom::getInstance();
	int dice = rng->getInt(0, 100);
	if ( dice < 75 ) {
		// create a health potion
		Object *object = new Object(x,y,CHAR_POTION_RED,"health potion", TCODColor::white);
		object->blocks = false;
		object->item = new Potion(Potion::HEAL, 5);
		engine.objects.push(object);
    } else if ( dice < 75+10 ) {
        // create an unknown potion
        int sym = rng->getInt(CHAR_POTION_YELLOW, CHAR_POTION_BLUE);
        int value = rng->getInt(-3, 3);
        if( value == 0 ) value = -1;
        Object *object = new Object(x,y,sym,"unknown potion", TCODColor::white);
        object->blocks = false;
        object->item = new Potion(Potion::UNKNOWN, value);
        engine.objects.push(object);
    } else if ( dice < 75+10+5 ) {
		// create an attack potion 
		Object *object = new Object(x,y,CHAR_POTION_GREEN,"attack potion", TCODColor::white);
		object->blocks = false;
		object->item = new Potion(Potion::ATK, 1);
		engine.objects.push(object);
	} else if ( dice < 75+10+5+5 ) {
		// create a defense potion
		Object *object = new Object(x,y,CHAR_POTION_BLUE,"defense potion", TCODColor::white);
		object->blocks = false;
		object->item = new Potion(Potion::DEF, 1);
		engine.objects.push(object);
	} else {
		// create an unknown potion
        int sym = rng->getInt(CHAR_POTION_YELLOW, CHAR_POTION_BLUE);
        int value = rng->getInt(-3, 3);
        if( value == 0 ) value = -1;
		Object *object = new Object(x,y,sym,"unknown potion", TCODColor::white);
		object->blocks = false;
		object->item = new Potion(Potion::UNKNOWN, value);
		engine.objects.push(object);
	}
}

void Map::addEquipment(int x, int y) {
    TCODRandom *rng = TCODRandom::getInstance();
    int dice = rng->getInt(0, 100);
    if ( dice < 75 ) {
        // create a health potion
        Object *object = new Object(x,y,CHAR_POTION_RED,"health potion", TCODColor::white);
        object->blocks = false;
        object->item = new Potion(Potion::HEAL, 5);
        engine.objects.push(object);
    } else if ( dice < 75+10 ) {
        // create an unknown potion
        int sym = rng->getInt(CHAR_POTION_YELLOW, CHAR_POTION_BLUE);
        int value = rng->getInt(-3, 3);
        if( value == 0 ) value = -1;
        Object *object = new Object(x,y,sym,"unknown potion", TCODColor::white);
        object->blocks = false;
        object->item = new Potion(Potion::UNKNOWN, value);
        engine.objects.push(object);
    } else if ( dice < 75+10+5 ) {
        // create an attack potion 
        Object *object = new Object(x,y,CHAR_POTION_GREEN,"attack potion", TCODColor::white);
        object->blocks = false;
        object->item = new Potion(Potion::ATK, 1);
        engine.objects.push(object);
    } else if ( dice < 75+10+5+5 ) {
        // create a defense potion
        Object *object = new Object(x,y,CHAR_POTION_BLUE,"defense potion", TCODColor::white);
        object->blocks = false;
        object->item = new Potion(Potion::DEF, 1);
        engine.objects.push(object);
    } else {
        // create an unknown potion
        int sym = rng->getInt(CHAR_POTION_YELLOW, CHAR_POTION_BLUE);
        int value = rng->getInt(-3, 3);
        if( value == 0 ) value = -1;
        Object *object = new Object(x,y,sym,"unknown potion", TCODColor::white);
        object->blocks = false;
        object->item = new Potion(Potion::UNKNOWN, value);
        engine.objects.push(object);
    }
}

bool Map::isWall(int x, int y) const {
    return !map->isWalkable(x, y);
}

bool Map::isNotWalkable(int x, int y) const {

	bool result;

	result = isWall(x - 1, y - 1) || isWall(x + 0, y - 1) || isWall(x + 1, y - 1) || isWall(x - 1, y + 0) || isWall(x + 0, y + 0) || isWall(x + 1, y + 0) || isWall(x - 1, y + 1) || isWall(x + 0, y + 1) || isWall(x + 1, y + 1);

	return result;
}

bool Map::canWalk(int x, int y) const {
    if (isWall(x,y)) {
        // this is a wall
        return false;
    }
    for (Object **iterator=engine.objects.begin(); iterator!=engine.objects.end();iterator++) {
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
    int posx = display_x, posy = display_y;

    // Torch intensity variation
    TCODRandom *rng=TCODRandom::getInstance();
    float ti = rng->getFloat(-0.15f,0.15f);

	TCODConsole::root->setDefaultBackground(TCODColor::darkestGrey);

	for (int x=posx; x < engine.displayWidth + posx + 1; x++) {
	    for (int y=posy; y < engine.displayHeight + posy + 1; y++) {
            // Cell distance to torch (squared)
            float dx = static_cast<float>(x - engine.player->x);
            float dy = static_cast<float>(y - engine.player->y);
            float dr2 = dx*dx + dy*dy;
			int offset = x + width*y;

	        if ( isInFov(x,y) ) {

                // Torch flickering FX
                TCODColor base = tiles[offset].colour;
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
	            TCODConsole::root->setCharBackground(x - posx, y - posy, tiles[offset].colour);
	        }
   	    }
	}
}
