class Object {
public :
    int x,y; // position on map
    int ch; // ascii code
    TCODColor col; // color
	const char *name; // the object's name
	bool blocks; // can we walk on this object?
	Entity *entity; // Something that can interact with its environment
	Item *item; // Something that can be used
	Container *container; // something that can contain objects
	 
	Object(int x, int y, int ch, const char *name, const TCODColor &col);
	virtual ~Object();
	void update();
    void render() const;
    float getDistance(int cx, int cy) const;
};
