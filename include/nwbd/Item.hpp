class Item {
public :
  int type; // Item type
  int amount; // Strength of Item
  int hp; // Amount of hp
  int atk; // Amount of atk
  int def; // Amount of def
  Item(): type(0), amount(0), hp(0), atk(0), def(0) {}
  Item(int type, int amount): type(type), amount(amount), hp(0), atk(0), def(0) {}
  Item(int type, int hp, int atk, int def): type(type), amount(0), hp(hp), atk(atk), def(def) {}
  virtual ~Item() {}
  bool grab(Object *owner, Object *object);
  virtual void drop(Object *owner, Object *object);
  virtual bool use(Object *owner, Object *object);
  virtual bool equip(Object *owner, Object *object, int key);
};

// Potion item
class Potion: public Item {
  public:
  enum {
    HEAL = 1,
    ATK = 2,
    DEF = 3,
    UNKNOWN = 4
  };

  Potion(int type, int amount): Item(type, amount) {}
  ~Potion() {}
  bool use(Object *owner, Object *object);
};

// Equipment item
class Equipment: public Item {
  public:
  enum {
    CORPSE = 11,
    SWORD = 12,
    SHEILD = 13,
    RING = 14,
    CURSED = 15
  };

  Equipment(int type, int hp, int atk, int def): Item(type, hp, atk, def) {}
  ~Equipment() {}
  void drop(Object *owner, Object *object);
  bool equip(Object *owner, Object *object, int key);
};