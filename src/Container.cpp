#include "Main.hpp"

Container::Container(int size) : size(size) {
}

Container::~Container() {
  inventory.clearAndDelete();
}

bool Container::add(Object *object) {
  if ( size > 0 && inventory.size() >= size ) {
    // inventory full
    return false;
  }
  inventory.push(object);
  return true;
}

void Container::remove(Object *object) {
  inventory.remove(object);
}
