#include "Main.hpp"

bool Item::grab(Object *owner, Object *object) {
  if ( object->container && object->container->add(owner) ) {
    engine.objects.remove(owner);
    return true;
  }
  return false;
}

void Item::drop(Object *owner, Object *object) {
  if( object->container ) {
    object->container->remove(owner);
    engine.objects.push(owner);
    owner->x = object->x;
    owner->y = object->y;
    engine.gui->message(TCODColor::lightGrey,"%s drops a %s.", object->name,owner->name);
  }
}

bool Item::use(Object *owner, Object *object) {
  if ( object->container ) {
    if( owner->item->type <= Potion::UNKNOWN ) {
      object->container->remove(owner);
      delete owner;
      return true;
    } else {
      engine.gui->message(TCODColor::lightGrey, "You think about it for a moment...\n but you resist the urge to eat the %s", owner->name);
      return false;
    }
  }
  return false;
}

bool Item::equip(Object *owner, Object *object, int key) {
  if ( owner->item ) {
    return true;
  }
  return false;
}

bool Potion::use(Object *owner, Object *object) {
  if ( object->entity ) {
    switch( type ) {
      case HEAL: {
        int netAmount = object->entity->heal(amount);
        if( netAmount > 0 ) {
          engine.gui->message(TCODColor::lightGrey, "You consume a health potion\n which increased your HP by %d", netAmount);
          return Item::use(owner,object);
        }
        break;
      }
      case ATK: {
        object->entity->baseAtk += amount;
        object->entity->atk += amount;
        engine.gui->message(TCODColor::lightGrey, "You consume an attack potion\n which increased your ATK by %d", amount);
        return Item::use(owner,object);
        break;
      }
      case DEF: {
        object->entity->baseDef += amount;
        object->entity->def += amount;
        engine.gui->message(TCODColor::lightGrey, "You consume a defense potion\n which increased your DEF by %d", amount);
        return Item::use(owner,object);
        break;
      }
      case UNKNOWN: {
        TCODRandom *rng = TCODRandom::getInstance();
        int potion = rng->getInt(HEAL, DEF);
        switch( potion ) {
          case HEAL: {
            int netAmount = object->entity->heal(amount);
            if( netAmount != 0 ) {
              std::string msg;
              if( netAmount > 0 ) {
                msg = "You consume a health potion\n which increased your HP by %d";
              } else {
                msg = "You consume a poison potion\n which decreased your HP by %d";
              }
              engine.gui->message(TCODColor::lightGrey, msg.c_str(), abs(netAmount));
              return Item::use(owner,object);
            }
            break;
          }
          case ATK: {
            object->entity->baseAtk += amount;
            object->entity->atk += amount;
            if( object->entity->atk < 0 ) object->entity->atk = 0;
            if( amount != 0 ) {
              std::string msg;
              if( amount > 0 ) {
                msg = "You consume an attack potion\n which increased your ATK by %d";
              } else {
                msg = "You consume an attack potion\n which decreased your ATK by %d";
              }
              engine.gui->message(TCODColor::lightGrey, msg.c_str(), abs(amount));
              return Item::use(owner,object);
            }
            break;
          }
          case DEF: {
            object->entity->baseDef += amount;
            object->entity->def += amount;
            if( object->entity->def < 0 ) object->entity->def = 0;
            if( amount != 0 ) {
              std::string msg;
              if( amount > 0 ) {
                msg = "You consume an defense potion\n which increased your DEF by %d";
              } else {
                msg = "You consume an defense potion\n which decreased your DEF by %d";
              }
              engine.gui->message(TCODColor::lightGrey, msg.c_str(), abs(amount));
              return Item::use(owner,object);
            }
            break;
          }
          default: break;
        }
        break;
      }
      default: break;
    }
  }
  return false;
}

void Equipment::drop(Object *owner, Object *object) {
  if( owner->item ) {
    if( owner->item->type == Equipment::CURSED ) {
      engine.gui->message(TCODColor::lightRed, "You cannot remove the %s, it is cursed!", owner->name);
      return;
    }
    if( object->entity->worn == owner ) {
      object->entity->worn = NULL;
    } else {
      object->entity->wielded = NULL;
    }

    // Reset stats, and recompute based on what is worn and wielded
    object->entity->hpMax = object->entity->baseHpMax;
    object->entity->atk = object->entity->baseAtk;
    object->entity->def = object->entity->baseDef;
    if( object->entity->worn ) {
      object->entity->hpMax += object->entity->worn->item->hp;
      object->entity->atk += object->entity->worn->item->atk;
      object->entity->def += object->entity->worn->item->def;
    }
    if( object->entity->wielded ) {
      object->entity->hpMax += object->entity->wielded->item->hp;
      object->entity->atk += object->entity->wielded->item->atk;
      object->entity->def += object->entity->wielded->item->def;
    }
  }
  Item::drop(owner, object);
}

bool Equipment::equip(Object *owner, Object *object, int key) {
  if ( owner->item ) {
    if( object->entity->wielded != owner && key == 'w' ) {
      if( object->entity->worn == owner ) {
        if( owner->item->type == Equipment::CURSED) {
          engine.gui->message(TCODColor::lightRed, "You cannot remove the %s, it is cursed!", owner->name);
          return false;
        }
        object->entity->worn = NULL;
      }
      object->entity->wielded = owner;

      // Reset stats, and recompute based on what is worn and wielded
      object->entity->hpMax = object->entity->baseHpMax;
      object->entity->atk = object->entity->baseAtk;
      object->entity->def = object->entity->baseDef;
      if( object->entity->worn ) {
        object->entity->hpMax += object->entity->worn->item->hp;
        object->entity->atk += object->entity->worn->item->atk;
        object->entity->def += object->entity->worn->item->def;
      }
      if( object->entity->wielded ) {
        object->entity->hpMax += object->entity->wielded->item->hp;
        object->entity->atk += object->entity->wielded->item->atk;
        object->entity->def += object->entity->wielded->item->def;
      }

      if( owner->item->type == Equipment::CURSED) {
        engine.gui->message(TCODColor::lightRed, "You feel a malevolent energy as you wield the %s\n and you can't let go, it is cursed!", owner->name);
      } else {
        engine.gui->message(TCODColor::lightGrey, "You now wield the %s", owner->name);
      }

      return true;
    } else if( object->entity->worn != owner && key == 'e' ) {
      if( object->entity->wielded == owner ) {
        if( owner->item->type == Equipment::CURSED) {
          engine.gui->message(TCODColor::lightRed, "You cannot let go of the %s, it is cursed!", owner->name);
          return false;
        }
        object->entity->wielded = NULL;
      }
      object->entity->worn = owner;

      // Reset stats, and recompute based on what is worn and wielded
      object->entity->hpMax = object->entity->baseHpMax;
      object->entity->atk = object->entity->baseAtk;
      object->entity->def = object->entity->baseDef;
      if( object->entity->worn ) {
        object->entity->hpMax += object->entity->worn->item->hp;
        object->entity->atk += object->entity->worn->item->atk;
        object->entity->def += object->entity->worn->item->def;
      }
      if( object->entity->wielded ) {
        object->entity->hpMax += object->entity->wielded->item->hp;
        object->entity->atk += object->entity->wielded->item->atk;
        object->entity->def += object->entity->wielded->item->def;
      }

      if( owner->item->type == Equipment::CURSED) {
        engine.gui->message(TCODColor::lightRed, "You feel a malevolent energy as you put on the %s\n and you can't get it off, it is cursed!", owner->name);
      } else {
        engine.gui->message(TCODColor::lightGrey, "You now wear the %s", owner->name);
      }

      return true;
    } else {
      return false;
    }
  } else {
    return false;
  }
}