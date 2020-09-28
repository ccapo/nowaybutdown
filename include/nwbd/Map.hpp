#define NGOALS 2 // Only two goals for now

struct Tile {
    bool explored;        // Has the player already seen this tile ?
    TCODColor colour;     // Tile colour
    double goals[NGOALS];   // Goal values
    double goalsPrev[NGOALS]; // Previous Goal values
    Tile() : explored(false), colour(TCODColor::darkestGrey), goals{0.0, 0.0}, goalsPrev{0.0, 0.0} {}
};
 
class Map {
public :
  int width,height;
  int display_x, display_y;
  TCODColor darkWall;
  TCODColor lightWall;
  TCODColor darkGround;
  TCODColor lightGround;
 
  Map(int width, int height);
  ~Map();
  bool isWall(int x, int y) const;
  bool isNotWalkable(int x, int y) const;
  bool isInFov(int x, int y) const;
  bool isExplored(int x, int y) const;
  bool canWalk(int x, int y) const;
  void computeFov();
  void moveDisplay(int x, int y);
  void updateGoals();
  void render() const;
  void generateMap(int &upx, int &upy, int &downx, int &downy); // Generates a map
  double getGoal(int x, int y, int index) const { return tiles[x + width*y].goals[index]; }
protected :
  Tile *tiles;
  TCODMap *map;
  std::vector<int> walkableTiles;
  std::vector<int> usedWalkableTiles;

  void floodFill(int x, int y, TCODColor fill); // Fill pixels that aren't the fill colour
  void generateHM(TCODHeightMap *hmap);         // Generates a height map
  void prepareMap();                            // Prepares a map

  void addCreature(int x, int y);
  void addItem(int x, int y);
  void addEquipment(int x, int y);
};
