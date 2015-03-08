#define NGOALS 2

struct Tile {
    bool explored;        // Has the player already seen this tile ?
    double goals[NGOALS]; // Goal values
    Tile() : explored(false), goals{0.0, 0.0} {}
};
 
class Map {
public :
    int width,height;
 
    Map(int width, int height);
    ~Map();
    bool isWall(int x, int y) const;
	bool isInFov(int x, int y) const;
    bool isExplored(int x, int y) const;
    bool canWalk(int x, int y) const;
    void computeFov();
    void render() const;
protected :
    Tile *tiles;
    TCODMap *map;
    friend class BspListener;
 
    void dig(int x1, int y1, int x2, int y2);
    void createRoom(bool first, int x1, int y1, int x2, int y2);
    void addMonster(int x, int y);
    void addItem(int x, int y);
};
