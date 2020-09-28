class Container {
public :
  int size; // maximum number of objects. 0=unlimited
  TCODList<Object *> inventory;   

  Container(int size);
  ~Container();
  bool add(Object *object);
  void remove(Object *object);
};
