class Ai {
public :

	Ai() {}
	virtual ~Ai() {}
	virtual void update(Actor *owner)=0;
};

class MonsterAi : public Ai {
public :
	MonsterAi();
	void update(Actor *owner);
protected :
	int moveCount;

	void moveOrAttack(Actor *owner, int targetx, int targety);
};

class ConfusedMonsterAi : public Ai {
public :
	ConfusedMonsterAi(int nbTurns, Ai *oldAi);
	void update(Actor *owner);
protected :
	int nbTurns;
	Ai *oldAi;
};

class PlayerAi : public Ai {
public :
	void update(Actor *owner);

protected :
	bool moveOrAttack(Actor *owner, int targetx, int targety);
	void handleActionKey(Actor *owner, int ascii);
	Actor *choseFromInventory(Actor *owner);
};
