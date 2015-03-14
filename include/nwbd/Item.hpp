class Item {
public :
	Item() {}
	virtual ~Item() {}
	bool grab(Object *owner, Object *object);
	void drop(Object *owner, Object *object);
	virtual bool use(Object *owner, Object *object);
	virtual bool equip(Object *owner, Object *object);
	virtual bool wield(Object *owner, Object *object);
};

// Potion item
class Potion: public Item {
	public:
	enum {
		HEAL,
		ATK,
		DEF,
		UNKNOWN,
		NPOTION
	};
	int type; // Potion type
	int amount; // Strength of potion

	Potion(int type, int amount): type(type), amount(amount) {}
	~Potion() {}
	bool use(Object *owner, Object *object);
};

// Spell item
class Spell: public Item {
	public:
	enum {
		FIRE,
		ICE,
		WIND,
		LIGHTNING,
		NSPELL
	};
	int type; // Spell type
	int amount; // Strength of spell

	Spell(int type, int amount): type(type), amount(amount) {}
	//bool use(Object *owner, Object *object);
};

// Equipment item
class Equipment: public Item {
	public:
	enum {
		STICK,
		BONE,
		SWORD,
		SHEILD,
		ARMOUR,
		RING,
		CURSED,
		NEQUIP
	};
	int type; // Equip type
	int hp; // Amount of hp
	int atk; // Amount of atk
	int def; // Amount of def

	Equipment(int type, int hp, int atk, int def): type(type), hp(hp), atk(atk), def(def) {}
	~Equipment() {}
	//bool use(Object *owner, Object *object);
	bool equip(Object *owner, Object *object);
	bool wield(Object *owner, Object *object);
};