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
  walkableTiles.clear();
  usedWalkableTiles.clear();

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
        walkableTiles.push_back(offset);
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
  int nWalkable = 0;
  float fWalkable = 0.0f;

  engine.level++;

  while( fWalkable < 0.5f ) {
    // Prepare a new cave map
    prepareMap();

    // Pick the starting location of the player
    int offset = rng->getInt(0, walkableTiles.size() - 1);
    int key = walkableTiles[offset];
    px = key % width;
    py = floor(key/width);
    usedWalkableTiles.push_back(offset);
    walkableTiles.erase(walkableTiles.begin()+offset);

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
    //std::cout << "fWalkable = " << fWalkable << "\n";
  }

  // Assign the location of the tunnel, ensuring they are sufficiently far apart
  int offset = rng->getInt(0, walkableTiles.size() - 1);
  int key = walkableTiles[offset];
  dx = key % width;
  dy = floor(key/width);
  int dp2 = pow(dx - px, 2) + pow(dy - py, 2);
  while ( dp2 < 8100 ) {
    offset = rng->getInt(0, walkableTiles.size() - 1);
    key = walkableTiles[offset];
    dx = key % width;
    dy = floor(key/width);
    dp2 = pow(dx - px, 2) + pow(dy - py, 2);
  }
  usedWalkableTiles.push_back(offset);
  walkableTiles.erase(walkableTiles.begin()+offset);

  // Add items, creatures and equipment depending on depth level
  int l = engine.level;
  int nitems = 8 - l/2, ncreatures = 24 + (l*l)/2, nequip = 2 - l/2;
  if( nitems < 0 ) nitems = 0;
  if( nequip < 0 ) nequip = 0;
  for(int i = 0; i < nitems + ncreatures + nequip; i++) {
    int offset = rng->getInt(0, walkableTiles.size() - 1);
    int key = walkableTiles[offset];
    int qx = key % width;
    int qy = floor(key/width);
    int qp2 = pow(qx - px, 2) + pow(qy - py, 2);
    int qd2 = pow(qx - dx, 2) + pow(qy - dy, 2);
    while( ( qp2 < 225 ) || ( qd2 < 225 ) ) {
      offset = rng->getInt(0, walkableTiles.size() - 1);
      key = walkableTiles[offset];
      qx = key % width;
      qy = floor(key/width);
      qp2 = pow(qx - px, 2) + pow(qy - py, 2);
      qd2 = pow(qx - dx, 2) + pow(qy - dy, 2);
    }
    
    usedWalkableTiles.push_back(offset);
    walkableTiles.erase(walkableTiles.begin()+offset);

    if( i < nitems ) {
      addItem(qx, qy);
    } else if( i < nitems + ncreatures ) {
      addCreature(qx, qy);
    } else {
      addEquipment(qx, qy);
    }
  }
}

void Map::addCreature(int x, int y) {
  TCODRandom *rng = TCODRandom::getInstance();
  float dice = rng->getFloat(0.0, 100.0);
  float l = static_cast<float>(engine.level), lstar = 5.0;
  float pOrc = 10.0, pGoblin = 10.0;
  float pTroll = 10.0, pSkeleton = 10.0;
  float pGolem = 10.0, pSmurf = 10.0;
  float pDemon = 10.0, pSlime = 10.0;
  float pFlayer = 10.0; //, pEyeball = 10.0;

  // Create level progression
  if( l <= lstar ) {
    pOrc = 50.0 - 50.0*l/lstar;
    pGoblin = 50.0 - 50.0*l/lstar;
    pTroll = 50.0*l/lstar;
    pSkeleton = 50.0*l/lstar;
    pGolem = 0.0;
    pSmurf = 0.0;
    pDemon = 0.0;
    pSlime = 0.0;
    pFlayer = 0.0;
    //pEyeball = 0.0;
  } else if( l > lstar && l <= 2.0*lstar ) {
    pOrc = 0.0;
    pGoblin = 0.0;
    pTroll = 50.0 - 50.0*(l - lstar)/lstar;
    pSkeleton = 50.0 - 50.0*(l - lstar)/lstar;
    pGolem = 50.0*(l - lstar)/lstar;
    pSmurf = 50.0*(l - lstar)/lstar;
    pDemon = 0.0;
    pSlime = 0.0;
    pFlayer = 0.0;
    //pEyeball = 0.0;
  } else if( l > 2.0*lstar && l <= 3.0*lstar ) {
    pOrc = 0.0;
    pGoblin = 0.0;
    pTroll = 0.0;
    pSkeleton = 0.0;
    pGolem = 50.0 - 50.0*(l - 2.0*lstar)/lstar;
    pSmurf = 50.0 - 50.0*(l - 2.0*lstar)/lstar;
    pDemon = 50.0*(l - 2.0*lstar)/lstar;
    pSlime = 50.0*(l - 2.0*lstar)/lstar;
    pFlayer = 0.0;
    //pEyeball = 0.0;
  } else if( l > 3.0*lstar && l <= 4.0*lstar ) {
    pOrc = 0.0;
    pGoblin = 0.0;
    pTroll = 0.0;
    pSkeleton = 0.0;
    pGolem = 0.0;
    pSmurf = 0.0;
    pDemon = 50.0 - 50.0*(l - 3.0*lstar)/lstar;
    pSlime = 50.0 - 50.0*(l - 3.0*lstar)/lstar;
    pFlayer = 50*(l - 3.0*lstar)/lstar;
    //pEyeball = 50.0*(l - 3.0*lstar)/lstar;
  } else {
    pOrc = 10.0;
    pGoblin = 10.0;
    pTroll = 10.0;
    pSkeleton = 10.0;
    pGolem = 10.0;
    pSmurf = 10.0;
    pDemon = 10.0;
    pSlime = 10.0;
    pFlayer = 10.0;
    //pEyeball = 10.0;
  }

  // Create creatures
  if ( dice < pOrc ) {
    // Create an Orc
    Object *object = new Object(x, y, CHAR_ORC_PEON, "Orc", TCODColor::white);
    object->entity = new CreatureEntity(10, 4, 0, "orc carcass");
    object->entity->ai = new MonsterAi();
    engine.objects.push(object);
  } else if ( dice < pOrc + pGoblin ) {
    // Create an Ogre
    Object *object = new Object(x, y, CHAR_GOBLIN_WARRIOR, "Goblin", TCODColor::white);
    object->entity = new CreatureEntity(12, 4, 1, "goblin entrials");
    object->entity->ai = new MonsterAi();
    engine.objects.push(object);
  } else if ( dice < pOrc + pGoblin + pTroll ) {
    // Create an Ogre
    Object *object = new Object(x, y, CHAR_ORGE_WARRIOR_GREEN, "Troll", TCODColor::white);
    object->entity = new CreatureEntity(14, 5, 2, "troll carcass");
    object->entity->ai = new MonsterAi();
    engine.objects.push(object);
  } else if ( dice < pOrc + pGoblin + pTroll + pSkeleton ) {
    // Create an Ogre
    Object *object = new Object(x, y, CHAR_SKELETON_WARRIOR, "Skeleton", TCODColor::white);
    object->entity = new CreatureEntity(16, 5, 1, "pile of bones");
    object->entity->ai = new MonsterAi();
    engine.objects.push(object);
  } else if ( dice < pOrc + pGoblin + pTroll + pSkeleton + pGolem ) {
    // Create an Ogre
    Object *object = new Object(x, y, CHAR_GOLEM_BROWN, "Golem", TCODColor::white);
    object->entity = new CreatureEntity(18, 6, 3, "mound of clay");
    object->entity->ai = new MonsterAi();
    engine.objects.push(object);
  } else if ( dice < pOrc + pGoblin + pTroll + pSkeleton + pGolem + pSmurf ) {
    // Create an Ogre
    Object *object = new Object(x, y, CHAR_UNDEAD_DWARF_AXEBEARER, "Undead Smurf", TCODColor::white);
    object->entity = new CreatureEntity(20, 6, 1, "smurf entrails");
    object->entity->ai = new MonsterAi();
    engine.objects.push(object);
  } else if ( dice < pOrc + pGoblin + pTroll + pSkeleton + pGolem + pSmurf + pDemon ) {
    // Create an Ogre
    Object *object = new Object(x, y, CHAR_DEMON_HERO, "Demon", TCODColor::white);
    object->entity = new CreatureEntity(24, 8, 3, "demon carcass");
    object->entity->ai = new MonsterAi();
    engine.objects.push(object);
  } else if ( dice < pOrc + pGoblin + pTroll + pSkeleton + pGolem + pSmurf + pDemon + pSlime ) {
    // Create an Ogre
    Object *object = new Object(x, y, CHAR_SLIME_GREEN, "Slime", TCODColor::white);
    object->entity = new CreatureEntity(30, 6, 4, "slime puddle");
    object->entity->ai = new MonsterAi();
    engine.objects.push(object);
  } else if ( dice < pOrc + pGoblin + pTroll + pSkeleton + pGolem + pSmurf + pDemon + pSlime + pFlayer ) {
    // Create an Ogre
    Object *object = new Object(x, y, CHAR_FLAYER_MAGE, "Flayer", TCODColor::white);
    object->entity = new CreatureEntity(36, 7, 4, "flayer entrials");
    object->entity->ai = new MonsterAi();
    engine.objects.push(object);
  } else {
    // Create an Ogre
    Object *object = new Object(x, y, CHAR_EYEBALL, "Floating Eyeball", TCODColor::white);
    object->entity = new CreatureEntity(48, 8, 5, "mound of goo");
    object->entity->ai = new MonsterAi();
    engine.objects.push(object);
  }
}

void Map::addItem(int x, int y) {
  TCODRandom *rng = TCODRandom::getInstance();
  int dice = rng->getInt(0, 100);
  if ( dice < 80 ) {
    // create a health potion
    Object *object = new Object(x,y,CHAR_POTION_RED,"health potion", TCODColor::white);
    object->blocks = false;
    object->item = new Potion(Potion::HEAL, 10);
    engine.objects.push(object);
  } else if ( dice < 80+10 ) {
    // create an unknown potion
    int sym = rng->getInt(CHAR_POTION_YELLOW, CHAR_POTION_BLUE);
    int value = rng->getInt(-3, 3);
    if( value == 0 ) value = -1;
    Object *object = new Object(x,y,sym,"unknown potion", TCODColor::white);
    object->blocks = false;
    object->item = new Potion(Potion::UNKNOWN, value);
    engine.objects.push(object);
  } else if ( dice < 80+10+5 ) {
    // create an attack potion 
    Object *object = new Object(x,y,CHAR_POTION_GREEN,"attack potion", TCODColor::white);
    object->blocks = false;
    object->item = new Potion(Potion::ATK, 1);
    engine.objects.push(object);
  } else {
    // create a defense potion
    Object *object = new Object(x,y,CHAR_POTION_BLUE,"defense potion", TCODColor::white);
    object->blocks = false;
    object->item = new Potion(Potion::DEF, 1);
    engine.objects.push(object);
  }
}

void Map::addEquipment(int x, int y) {
    TCODRandom *rng = TCODRandom::getInstance();
    int dice = rng->getInt(0, 100);
    int dice2 = rng->getInt(0, 100);
    if ( dice < 25 ) {
        // create a sword
        int sym = rng->getInt(CHAR_SWORD_BASIC, CHAR_SWORD_GOLD);
        int value = rng->getInt(1, 5);
        Object *object = new Object(x,y,sym,"sword", TCODColor::white);
        object->blocks = false;
        if( dice2 < 25 ) {
            object->item = new Equipment(Equipment::CURSED, 0, -value, 0);
        } else {
            object->item = new Equipment(Equipment::SWORD, 0, value, 0);
        }
        engine.objects.push(object);
    } else if ( dice < 25+25 ) {
        // create a shield
        int sym = rng->getInt(CHAR_SHIELD_BROWN, CHAR_SHIELD_GOLD);
        int def = rng->getInt(1, 5);
        Object *object = new Object(x,y,sym,"shield", TCODColor::white);
        object->blocks = false;
        if( dice2 < 25 ) {
            object->item = new Equipment(Equipment::CURSED, 0, 0, -def);
        } else {
            object->item = new Equipment(Equipment::SHEILD, 0, 0, def);
        }
        engine.objects.push(object);
    } else if ( dice < 25+25+25 ) {
        // create a ring
        int sym = rng->getInt(CHAR_RING_RED, CHAR_RING_BLUE);
        int atk = rng->getInt(1, 5);
        int def = rng->getInt(1, 5);
        Object *object = new Object(x,y,sym,"ring", TCODColor::white);
        object->blocks = false;
        if( dice2 < 25 ) {
            object->item = new Equipment(Equipment::CURSED, 0, -atk, -def);
        } else {
           object->item = new Equipment(Equipment::RING, 0, atk, def);
        }
        engine.objects.push(object);
    } else {
        // create a corpse
        int def = rng->getInt(-2, 1);
        Object *object = new Object(x,y,CHAR_SKULL,"carcass", TCODColor::lighterRed);
        object->blocks = false;
        object->item = new Equipment(Equipment::CORPSE, 0, 0, def);
        engine.objects.push(object); 
    }
}

void Map::updateGoals() {
    const int dx[8] = {-1, 0, 1, -1, 1, -1, 0, 1};
    const int dy[8] = {-1, -1, -1, 0, 0, 1, 1, 1};
    const double dcoef = 1.0/8.0, lambda = 1.0;

    int offset = engine.player->x + width*engine.player->y;
    tiles[offset].goalsPrev[0] = 0.75;
    offset = engine.stairs->x + width*engine.stairs->y;
    tiles[offset].goalsPrev[1] = 0.25;

    for(int x = 1; x < width - 2; x++) {
        for(int y = 1; y < height - 2; y++) {
            offset = x + width*y;
            if( map->isWalkable(x, y) ) {
                double sdiff[2] = {0.0, 0.0};
                for(int z = 0; z < 8; z++) {
                    int doffset = x + dx[z] + width*(y + dy[z]);
                    sdiff[0] += tiles[doffset].goalsPrev[0] - tiles[offset].goalsPrev[0];
                    sdiff[1] += tiles[doffset].goalsPrev[1] - tiles[offset].goalsPrev[1];
                }
                tiles[offset].goals[0] = lambda*(tiles[offset].goalsPrev[0] + dcoef*sdiff[0]);
                tiles[offset].goals[1] = lambda*(tiles[offset].goalsPrev[1] + dcoef*sdiff[1]);
            } else {
                tiles[offset].goals[0] = 0.0;
                tiles[offset].goals[1] = 0.0;
            }
        }
    }

    for(int x = 1; x < width - 2; x++) {
        for(int y = 1; y < height - 2; y++) {
            offset = x + width*y;
            tiles[offset].goalsPrev[0] = tiles[offset].goals[0];
            tiles[offset].goalsPrev[1] = tiles[offset].goals[1];
        }
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
    TCODRandom *rng = TCODRandom::getInstance();
    float ti = rng->getFloat(-0.15f, 0.15f);

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
