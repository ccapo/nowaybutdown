class Item {
public :
	bool usable;
	Item() {}
	virtual ~Item() {}
	bool grab(Object *owner, Object *object);
	void drop(Object *owner, Object *object);
	virtual bool use(Object *owner, Object *object);
};

class Healer : public Item {
public :
	float amount; // how many hp

	Healer(float amount);
	bool use(Object *owner, Object *object);
};

class LightningBolt : public Item {
public :
	float range, damage;
	LightningBolt(float range, float damage);
	bool use(Object *owner, Object *object);
};

class Confuser : public Item {
public :
	int nTurns;
	float range;
	Confuser(int nTurns, float range);
	bool use(Object *owner, Object *object);	
};

class Fireball : public LightningBolt {
public :
	Fireball(float range, float damage);
	bool use(Object *owner, Object *object);		
};

