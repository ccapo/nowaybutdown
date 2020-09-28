class Ai;

class Entity {
public:
  int baseHpMax; // Base maximum health points
  int hpMax; // maximum health points
  int hp; // current health points
  int baseAtk; // Base hit points given
  int atk; // hit points given
  int baseDef; // Base hit points deflected
  int def; // hit points deflected
  const char *corpseName; // the object's name once dead/destroyed
  Ai *ai; // Entity's AI
  Object *wielded; // Wielded object
  Object *worn; // Worn object

  Entity(int hpMax, int atk, int def, const char *corpseName);
  virtual ~Entity();
  inline bool isDead() { return hp <= 0; }
  void attack(Object *owner, Object *target);
  int damage(Object *owner, int amount);
  int heal(int amount);
  virtual void die(Object *owner);
};

class CreatureEntity: public Entity {
public:
  CreatureEntity(int hpMax, int atk, int def, const char *corpseName);
  void die(Object *owner);
};

class PlayerEntity: public Entity {
public:
  PlayerEntity(int hpMax, int atk, int def, const char *corpseName);
  void die(Object *owner);
};
