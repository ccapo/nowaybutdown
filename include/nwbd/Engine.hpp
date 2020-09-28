class Engine {
public :
  enum GameStatus {
    STARTUP,
    IDLE,
    NEW_TURN,
    VICTORY,
    DEFEAT
  } gameStatus;
  TCOD_key_t lastKey;
  TCOD_mouse_t mouse;
    TCODList<Object *> objects;
    Object *player;
  Object *stairs;
    Map *map;
    int fovRadius;
    int windowWidth;
    int windowHeight;
    int displayWidth;
    int displayHeight;
    int mapWidth;
    int mapHeight;
    int level;
    Gui *gui;

    Engine(int windowWidth, int windowHeight);
    ~Engine();
    bool update();
    void render();
    void sendToBack(Object *object);
    Object *getObject(int x, int y) const;
    Object *getClosestMonster(int x, int y, float range) const;
    bool pickATile(int *x, int *y, float maxRange = 0.0f);
};
 
extern Engine engine;
