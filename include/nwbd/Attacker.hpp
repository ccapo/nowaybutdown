class Attacker {
public :
	float basePower; // Base hit points given
	float power; // hit points given

	Attacker(float power);
	void attack(Actor *owner, Actor *target);
	void wield(Actor *owner, Actor *wielder);
};
