#include "object.h"

Object::Object(Floor* floor, u_char x, u_char y, std::string* name, std::string* description, u_int itemType, u_short color, Dice* hitBonus, Dice* damageBonus, Dice* dodgeBonus, Dice* defenseBonus, u_short weight, Dice* speedBonus, Dice* specialAttribute, Dice* value, bool isArtifact, u_char rarity) {
    this->floor = floor;
    this->x = x;
    this->y = y;
    this->name.assign(*name);
    this->description.assign(*description);
    this->itemType = itemType;
    this->color = color;
    this->hitBonus = hitBonus;
    this->damageBonus = damageBonus;
    this->dodgeBonus = dodgeBonus;
    this->defenseBonus = defenseBonus;
    this->weight = weight;
    this->speedBonus = speedBonus;
    this->specialAttribute = specialAttribute;
    this->value = value;
    this->isArtifact = isArtifact;
    this->rarity = rarity;
}

Object::~Object() {
    delete (this->hitBonus);
    delete (this->damageBonus);
    delete (this->dodgeBonus);
    delete (this->defenseBonus);
    delete (this->speedBonus);
    delete (this->specialAttribute);
    delete (this->value);
}

std::string Object::typeToString(u_int type) {
    switch (type) {
        case OBJECT_WEAPON:
            return OBJECT_WEAPON_KEYWORD;
        case OBJECT_OFFHAND:
            return OBJECT_OFFHAND_KEYWORD;
        case OBJECT_RANGED:
            return OBJECT_RANGED_KEYWORD;
        case OBJECT_ARMOR:
            return OBJECT_ARMOR_KEYWORD;
        case OBJECT_HELMET:
            return OBJECT_HELMET_KEYWORD;
        case OBJECT_CLOAK:
            return OBJECT_CLOAK_KEYWORD;
        case OBJECT_GLOVES:
            return OBJECT_GLOVES_KEYWORD;
        case OBJECT_BOOTS:
            return OBJECT_BOOTS_KEYWORD;
        case OBJECT_RING:
            return OBJECT_RING_KEYWORD;
        case OBJECT_AMULET:
            return OBJECT_AMULET_KEYWORD;
        case OBJECT_LIGHT:
            return OBJECT_LIGHT_KEYWORD;
        case OBJECT_SCROLL:
            return OBJECT_SCROLL_KEYWORD;
        case OBJECT_BOOK:
            return OBJECT_BOOK_KEYWORD;
        case OBJECT_FLASK:
            return OBJECT_FLASK_KEYWORD;
        case OBJECT_GOLD:
            return OBJECT_GOLD_KEYWORD;
        case OBJECT_AMMUNITION:
            return OBJECT_AMMUNITION_KEYWORD;
        case OBJECT_FOOD:
            return OBJECT_FOOD_KEYWORD;
        case OBJECT_WAND:
            return OBJECT_WAND_KEYWORD;
        case OBJECT_CONTAINER:
            return OBJECT_CONTAINER_KEYWORD;
        default:
            return "";
    }
}

u_char Object::getCharacter() {
    if (this->isWeapon()) {
        return WEAPON_CHARACTER;
    } else if (this->isOffHand()) {
        return OFFHAND_CHARACTER;
    } else if (this->isRanged()) {
        return RANGED_CHARACTER;
    } else if (this->isArmor()) {
        return ARMOR_CHARACTER;
    } else if (this->isHelmet()) {
        return HELMET_CHARACTER;
    } else if (this->isCloak()) {
        return CLOAK_CHARACTER;
    } else if (this->isGloves()) {
        return GLOVES_CHARACTER;
    } else if (this->isBoots()) {
        return BOOTS_CHARACTER;
    } else if (this->isRing()) {
        return RING_CHARACTER;
    } else if (this->isAmulet()) {
        return AMULET_CHARACTER;
    } else if (this->isLight()) {
        return LIGHT_CHARACTER;
    } else if (this->isScroll()) {
        return SCROLL_CHARACTER;
    } else if (this->isBook()) {
        return BOOK_CHARACTER;
    } else if (this->isFlask()) {
        return FLASK_CHARACTER;
    } else if (this->isGold()) {
        return GOLD_CHARACTER;
    } else if (this->isAmmunition()) {
        return AMMUNITION_CHARACTER;
    } else if (this->isFood()) {
        return FOOD_CHARACTER;
    } else if (this->isWand()) {
        return WAND_CHARACTER;
    } else if (this->isContainer()) {
        return CONTAINER_CHARACTER;
    } else {
        return UNKNOWN_CHARACTER;
    }
}

/** GETTERS **/
Floor* Object::getFloor() {
    return floor;
}

u_char Object::getX() {
    return x;
}

u_char Object::getY() {
    return y;
}

std::string Object::getName() {
    return name;
}

std::string Object::getDescription() {
    return description;
}

u_int Object::getItemType() {
    return itemType;
}

std::string Object::getTypeString() {
    switch (this->itemType) {
        case OBJECT_WEAPON:
            return OBJECT_WEAPON_KEYWORD;
        case OBJECT_OFFHAND:
            return OBJECT_OFFHAND_KEYWORD;
        case OBJECT_RANGED:
            return OBJECT_RANGED_KEYWORD;
        case OBJECT_ARMOR:
            return OBJECT_ARMOR_KEYWORD;
        case OBJECT_HELMET:
            return OBJECT_HELMET_KEYWORD;
        case OBJECT_CLOAK:
            return OBJECT_CLOAK_KEYWORD;
        case OBJECT_GLOVES:
            return OBJECT_GLOVES_KEYWORD;
        case OBJECT_BOOTS:
            return OBJECT_BOOTS_KEYWORD;
        case OBJECT_RING:
            return OBJECT_RING_KEYWORD;
        case OBJECT_AMULET:
            return OBJECT_AMULET_KEYWORD;
        case OBJECT_LIGHT:
            return OBJECT_LIGHT_KEYWORD;
        case OBJECT_SCROLL:
            return OBJECT_SCROLL_KEYWORD;
        case OBJECT_BOOK:
            return OBJECT_BOOK_KEYWORD;
        case OBJECT_FLASK:
            return OBJECT_FLASK_KEYWORD;
        case OBJECT_GOLD:
            return OBJECT_GOLD_KEYWORD;
        case OBJECT_AMMUNITION:
            return OBJECT_AMMUNITION_KEYWORD;
        case OBJECT_FOOD:
            return OBJECT_FOOD_KEYWORD;
        case OBJECT_WAND:
            return OBJECT_WAND_KEYWORD;
        case OBJECT_CONTAINER:
            return OBJECT_CONTAINER_KEYWORD;
        default:
            return "";
    }
}

u_short Object::getColor() {
    return this->color;
}

u_short Object::getHitBonus() {
    return this->hitBonus->roll();
}

u_short Object::getDamageBonus() {
    return this->damageBonus->roll();
}

u_short Object::getDodgeBonus() {
    return this->dodgeBonus->roll();
}

u_short Object::getDefenseBonus() {
    return this->defenseBonus->roll();
}

u_short Object::getWeight() {
    return this->weight;
}

short Object::getSpeedBonus() {
    return this->speedBonus->roll();
}

u_short Object::getSpecialAttribute() {
    return this->specialAttribute->roll();
}

u_short Object::getValue() {
    return this->value->roll();
}

bool Object::getIsArtifact() {
    return this->isArtifact;
}

u_char Object::getRarity() {
    return this->rarity;
}

Dice* Object::getHitDice() {
    return this->hitBonus;
}

Dice* Object::getDamageDice() {
    return this->damageBonus;
}

Dice* Object::getDodgeDice() {
    return this->dodgeBonus;
}

Dice* Object::getSpeedDice() {
    return this->speedBonus;
}

Dice* Object::getSpecialAttributeDice() {
    return this->specialAttribute;
}

Dice* Object::getValueDice() {
    return this->value;
}
/** GETTERS **/

/** SETTERS **/
/** SETTERS **/

/** TYPE CONVERSION **/
bool Object::isWeapon() {
    return this->itemType & OBJECT_WEAPON;
}
bool Object::isOffHand() {
    return this->itemType & OBJECT_OFFHAND;
}
bool Object::isRanged() {
    return this->itemType & OBJECT_RANGED;
}
bool Object::isArmor() {
    return this->itemType & OBJECT_ARMOR;
}
bool Object::isHelmet() {
    return this->itemType & OBJECT_HELMET;
}
bool Object::isCloak() {
    return this->itemType & OBJECT_CLOAK;
}
bool Object::isGloves() {
    return this->itemType & OBJECT_GLOVES;
}
bool Object::isBoots() {
    return this->itemType & OBJECT_BOOTS;
}
bool Object::isRing() {
    return this->itemType & OBJECT_RING;
}
bool Object::isAmulet() {
    return this->itemType & OBJECT_AMULET;
}
bool Object::isLight() {
    return this->itemType & OBJECT_LIGHT;
}
bool Object::isScroll() {
    return this->itemType & OBJECT_SCROLL;
}
bool Object::isBook() {
    return this->itemType & OBJECT_BOOK;
}
bool Object::isFlask() {
    return this->itemType & OBJECT_FLASK;
}
bool Object::isGold() {
    return this->itemType & OBJECT_GOLD;
}
bool Object::isAmmunition() {
    return this->itemType & OBJECT_AMMUNITION;
}
bool Object::isFood() {
    return this->itemType & OBJECT_FOOD;
}
bool Object::isWand() {
    return this->itemType & OBJECT_WAND;
}
bool Object::isContainer() {
    return this->itemType & OBJECT_CONTAINER;
}
/** TYPE CONVERSION **/