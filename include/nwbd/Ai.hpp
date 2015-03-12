class Ai {
public :

	Ai() {}
	virtual ~Ai() {}
	virtual void update(Object *owner) = 0;
};

class MonsterAi : public Ai {
public :
	MonsterAi();
	void update(Object *owner);
protected :
	int moveCount;

	void moveOrAttack(Object *owner, int targetx, int targety);
};

class ConfusedMonsterAi : public Ai {
public :
	ConfusedMonsterAi(int nbTurns, Ai *prevAi);
	void update(Object *owner);
protected :
	int nTurns;
	Ai *prevAi;
};

class PlayerAi : public Ai {
public :
	void update(Object *owner);

protected :
	bool moveOrAttack(Object *owner, int targetx, int targety);
	void handleActionKey(Object *owner, int ascii, int &dx, int &dy);
	void choseFromInventory(Object *owner);
	void helpScreen();
};
