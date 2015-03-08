class Item {
public :
	bool pick(Actor *owner, Actor *wearer);
	void drop(Actor *owner, Actor *wearer);
	virtual bool use(Actor *owner, Actor *wearer);
};

class Healer : public Item {
public :
	float amount; // how many hp

	Healer(float amount);
	bool use(Actor *owner, Actor *wearer);
};

class LightningBolt : public Item {
public :
	float range,damage;
	LightningBolt(float range, float damage);
	bool use(Actor *owner, Actor *wearer);
};

class Confuser : public Item {
public :
	int nbTurns;
	float range;
	Confuser(int nbTurns, float range);
	bool use(Actor *owner, Actor *wearer);	
};

class Fireball : public LightningBolt {
public :
	Fireball(float range, float damage);
	bool use(Actor *owner, Actor *wearer);		
};

