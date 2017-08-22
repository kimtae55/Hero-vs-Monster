#include <iostream>
#include <iomanip>
#include <string>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

using namespace std;

#include "main.h"
#include "unit.h"
#include "item.h"

//==============================================================================
// 1st Generation
//==============================================================================

//------------------------------------------------------------------------------
// class Item
//------------------------------------------------------------------------------

Item::Item() {
    shape = DEFAULT_ITEM_SHAPE;
    name = DEFAULT_ITEM_NAME;
    price = DEFAULT_ITEM_PRICE;

    disposed = false;
    equipped = false;
}

Item::Item(Item &other) {
    shape = other.shape;
    name = other.name;
    price = other.price;
    
    disposed = other.disposed;
    equipped = other.equipped;
}

Item::Item(char shape, string name, int price) {
    this->shape = shape;
    this->name = name;
    this->price = price;

    disposed = false;
    equipped = false;
}

Item::~Item() {
}

void Item::print() {
    cout << shape;
}

void Item::printFullSpec() {
    cout << "(" << shape << ") " << setw(MAX_LEN_ITEM_NAME) << left << name << right << " : Price " << setw(MAX_LEN_ITEM_PRICE) << price << " gold";
}

char Item::getShape() {
    return shape;
}

string Item::getName() {
	return name;
}

int Item::getPrice() {
    return price;
}

bool Item::hasBeenDisposed() {
    return disposed;
}

bool Item::hasBeenEquipped() {
    return equipped;
}

bool Item::isConsumable() {
    return false;
}

bool Item::isEquippable() {
    return false;
}

bool Item::isKeyType() {
    return false;
}

void Item::save(ostream &out) {
    out << "#---------------------------------------- class Item" << endl;
    
    out << "# shape" << endl;
    out << shape << endl;
    out << "# name" << endl;
    out << name << endl;
    out << "# price" << endl;
    out << price << endl;
    out << "# disposed" << endl;
    out << disposed << endl;
    out << "# equipped" << endl;
    out << equipped << endl;
}

void Item::load(istream &in) {
    char buf[MAX_LEN_BUF];
    
    //out << "#---------------------------------------- class Item" << endl;
    in.getline(buf, MAX_LEN_BUF); // skip a line

    //out << "# shape" << endl;
    in.getline(buf, MAX_LEN_BUF); // skip a line
    //out << shape << endl;
    in >> shape;
    in.get(); // skip a char(new line = '\n')

    //out << "# name" << endl;
    in.getline(buf, MAX_LEN_BUF); // skip a line
    //out << name << endl;
    in.getline(buf, MAX_LEN_BUF); // skip a line
    name = buf;
    
    //out << "# price" << endl;
    in.getline(buf, MAX_LEN_BUF); // skip a line
    //out << price << endl;
    in >> price;
    in.get(); // skip a char(new line = '\n')
    
    //out << "# disposed" << endl;
    in.getline(buf, MAX_LEN_BUF); // skip a line
    //out << disposed << endl;
    in >> disposed;
    in.get(); // skip a char(new line = '\n')  
    
    //out << "# equipped" << endl;
    in.getline(buf, MAX_LEN_BUF); // skip a line
    //out << equipped << endl;
    in >> equipped;
    in.get(); // skip a char(new line = '\n')  
}

Item *createItemByID(string itemID) {
    return createItemByID(itemID, NULL);
}

Item *createItemByID(string itemID, Item *itemToCopy) {
    if (itemID == ITEM_ID_WEAPON) {
        return (itemToCopy == NULL) ? new Weapon(): new Weapon((Weapon &)*itemToCopy);
    }
    else if (itemID == ITEM_ID_ARMOR) {
        return (itemToCopy == NULL) ? new Armor(): new Armor((Armor &)*itemToCopy);
    }
    else if (itemID == ITEM_ID_KEY) {
        return (itemToCopy == NULL) ? new Key(): new Key((Key &)*itemToCopy);
    }
    else if (itemID == ITEM_ID_POTION) {
        return (itemToCopy == NULL) ? new Potion(): new Potion((Potion &)*itemToCopy);
    }
    else {
        cout << "error: no such item exist! itemID = \"" << itemID << "\"" << endl;
        exit(1);
    }
    
    return NULL;
}

ostream &operator<<(ostream &out, Item *item) {
	out << "# Item class type" << endl;
	out << item->getItemID() << endl;
	item->save(out);
	return out;
}

istream &operator>>(istream &in, Item *&item) {
	string itemID = "";
	in >> itemID;
	in.get(); // skip a char(new line = '\n')
	item = createItemByID(itemID); // create an object using factory.
	item->load(in);
	return in;
}

//==============================================================================
// 2nd Generation
//==============================================================================

ConsumableItem::ConsumableItem() : Item() {
}

ConsumableItem::ConsumableItem(ConsumableItem &other) : Item(other) {
}

ConsumableItem::ConsumableItem(char shape, string name, int price) : Item(shape, name, price){
}

ConsumableItem::~ConsumableItem() {
}

bool ConsumableItem::isConsumable() {
    return true;
}

void ConsumableItem::save(ostream &out) {
    Item::save(out);
    
    out << "#---------------------------------------- class ConsumableItem" << endl;
}

void ConsumableItem::load(istream &in) {
    Item::load(in);
    
    char buf[MAX_LEN_BUF];
    
    //out << "#---------------------------------------- class ConsumableItem" << endl;
    in.getline(buf, MAX_LEN_BUF); // skip a line
}

EquippableItem::EquippableItem() : Item() {
}

EquippableItem::EquippableItem(EquippableItem &other) : Item(other) {
}

EquippableItem::EquippableItem(char shape, string name, int price) : Item(shape, name, price) {
}

EquippableItem::~EquippableItem() {
}

bool EquippableItem::isEquippable() {
    return true;
}

bool EquippableItem::isArmor() {
    return false;
}

bool EquippableItem::isWeapon() {
    return false;
}

void EquippableItem::save(ostream &out) {
    Item::save(out);
    
    out << "#---------------------------------------- class EquippableItem" << endl;
}

void EquippableItem::load(istream &in) {
    Item::load(in);
    
    char buf[MAX_LEN_BUF];
    
    //out << "#---------------------------------------- class EquippableItem" << endl;
    in.getline(buf, MAX_LEN_BUF); // skip a line
}

//==============================================================================
// 3rd Generation
//==============================================================================

//------------------------------------------------------------------------------
// class Potion
//------------------------------------------------------------------------------

Potion::Potion() : ConsumableItem(DEFAULT_POTION_SHAPE, DEFAULT_POTION_NAME, DEFAULT_POTION_PRICE) {
    hp = DEFAULT_POTION_HP;
    mp = DEFAULT_POTION_MP;
}

Potion::Potion(Potion &other) : ConsumableItem(other) {
    hp = other.hp;
    mp = other.mp;
}

Potion::Potion(char shape, string name, int price, int hp, int mp) : ConsumableItem(shape, name, price) {
    this->hp = hp;
    this->mp = mp;
}

Potion::~Potion() {
}

void Potion::printFullSpec() {
    Item::printFullSpec();
    cout << " | HP " << hp << " | MP " << mp; 
}

string Potion::getItemID() {
    return string(ITEM_ID_POTION);
}

int Potion::getHp() {
    return hp;
}

int Potion::getMp() {
    return mp;
}

bool Potion::use(Unit *unit) {
    if (unit == NULL) {
        return false;
    }
	
	bool resultIncHp = unit->incHp(hp);
	bool resultIncMp = unit->incMp(hp);
	
    if (resultIncHp || resultIncMp) {
        disposed = true;
        
        return true;
    }
    // this potion MUST NOT be disposed when the unit's hp and mp are full.
    else {
        return false;
    }
}

void Potion::save(ostream &out) {
    ConsumableItem::save(out);
    
    out << "#---------------------------------------- class Potion" << endl;
    
    out << "# hp" << endl;
    out << hp << endl;
    out << "# mp" << endl;
    out << mp << endl;
}

void Potion::load(istream &in) {
    ConsumableItem::load(in);
    
    char buf[MAX_LEN_BUF];  
    
    //out << "#---------------------------------------- class Potion" << endl;
    in.getline(buf, MAX_LEN_BUF); // skip a line

    //out << "# hp" << endl;
    in.getline(buf, MAX_LEN_BUF); // skip a line
    //out << hp << endl;
    in >> hp;
    in.get(); // skip a char(new line = '\n')  

    //out << "# mp" << endl;
    in.getline(buf, MAX_LEN_BUF); // skip a line
    //out << mp << endl;
    in >> mp;
    in.get(); // skip a char(new line = '\n')  
}

//------------------------------------------------------------------------------
// class Key
//------------------------------------------------------------------------------

Key::Key() : ConsumableItem(DEFAULT_KEY_SHAPE, DEFAULT_KEY_NAME, DEFAULT_KEY_PRICE) {
    durability = DEFAULT_KEY_DURABILITY;
}

Key::Key(Key &other) : ConsumableItem(other) {
    durability = other.durability;
}

Key::Key(char shape, string name, int price, int durability) : ConsumableItem(shape, name, price) {
    this->durability = durability;
}

Key::~Key() {
}

void Key::printFullSpec() {
    Item::printFullSpec();
    cout << " | Durability " << durability; 
}

string Key::getItemID() {
    return string(ITEM_ID_KEY);
}

int Key::getDurability() {
    return durability;
}

bool Key::use(Unit *unit) {
    // N/A
    return false;
}

bool Key::isKeyType() {
    return true;
}

void Key::save(ostream &out) {
    ConsumableItem::save(out);
    
    out << "#---------------------------------------- class Key" << endl;
    
    out << "# durability" << endl;
    out << durability << endl;
}

void Key::load(istream &in) {
    ConsumableItem::load(in);

    char buf[MAX_LEN_BUF];  
    
    //out << "#---------------------------------------- class Key" << endl;
    in.getline(buf, MAX_LEN_BUF); // skip a line

    //out << "# durability" << endl;
    in.getline(buf, MAX_LEN_BUF); // skip a line
    //out << durability << endl;
    in >> durability;
    in.get(); // skip a char(new line = '\n')  
}

//------------------------------------------------------------------------------
// class Armor
//------------------------------------------------------------------------------

Armor::Armor() : EquippableItem(DEFAULT_ARMOR_SHAPE, DEFAULT_ARMOR_NAME, DEFAULT_ARMOR_PRICE) {
    def = DEFAULT_ARMOR_DEF;
    durability = DEFAULT_ARMOR_DURABILITY;
    bodyPartID = -1;
}

Armor::Armor(Armor &other) : EquippableItem(other) {
    def = other.def;
    durability = other.durability;
    bodyPartID = other.bodyPartID;
}

Armor::Armor(char shape, string name, int price, int def, int durability, int bodyPartID) : EquippableItem(shape, name, price) {
    this->def = def;
    this->durability = durability;
    this->bodyPartID = bodyPartID;
}

Armor::~Armor() {
}

void Armor::printFullSpec() {
    Item::printFullSpec();
    cout << " | Def " << setw(MAX_LEN_ITEM_DEF) << def << " | Durability " << setw(MAX_LEN_ITEM_DURABILITY) << durability << " | ";
    if (bodyPartID == BODY_PART_ID_HEAD) {
        cout << BODY_PART_ID_HEAD_STRING;
    }
    else if (bodyPartID == BODY_PART_ID_CHEST) {
        cout << BODY_PART_ID_CHEST_STRING;
    }
    else if (bodyPartID == BODY_PART_ID_BACK) {
        cout << BODY_PART_ID_BACK_STRING;
    }
    else if (bodyPartID == BODY_PART_ID_HANDS) {
        cout << BODY_PART_ID_HANDS_STRING;
    }
    else if (bodyPartID == BODY_PART_ID_LEGS) {
        cout << BODY_PART_ID_LEGS_STRING;
    }
    else if (bodyPartID == BODY_PART_ID_FEET) {
        cout << BODY_PART_ID_FEET_STRING;
    }
    cout << " item";
}

string Armor::getItemID() {
    return string(ITEM_ID_ARMOR);
}

int Armor::getDef() {
    return def;
}

int Armor::getDurability() {
    return durability;
}

int Armor::getBodyPartID() {
    return bodyPartID;
}

bool Armor::use(Unit *unit) {
    if (unit->equip(this)) {
        equipped = true;
        return true;
    }
    return false;    
}

bool Armor::isArmor() {
    return true;
}

void Armor::save(ostream &out) {
    EquippableItem::save(out);
    
    out << "#---------------------------------------- class Armor" << endl;
    
    out << "# def" << endl;
    out << def << endl;
    out << "# durability" << endl;
    out << durability << endl;
    out << "# bodyPartID" << endl;
    out << bodyPartID << endl;
}

void Armor::load(istream &in) {
    EquippableItem::load(in);

    char buf[MAX_LEN_BUF];  
    
    //out << "#---------------------------------------- class Armor" << endl;
    in.getline(buf, MAX_LEN_BUF); // skip a line
    
    //out << "# def" << endl;
    in.getline(buf, MAX_LEN_BUF); // skip a line
    //out << def << endl;
    in >> def;
    in.get(); // skip a char(new line = '\n')  
    
    //out << "# durability" << endl;
    in.getline(buf, MAX_LEN_BUF); // skip a line
    //out << durability << endl;
    in >> durability;
    in.get(); // skip a char(new line = '\n')  
    
    //out << "# bodyPartID" << endl;
    in.getline(buf, MAX_LEN_BUF); // skip a line
    //out << bodyPartID << endl;
    in >> bodyPartID;
    in.get(); // skip a char(new line = '\n')  
}

/*
 #define BODY_PART_ID_HEAD 0
 #define BODY_PART_ID_CHEST 1
 #define BODY_PART_ID_BACK 2
 #define BODY_PART_ID_HANDS 3
 #define BODY_PART_ID_LEGS 4
 #define BODY_PART_ID_FEET 5
 #define BODY_PART_ID_LEFT_HAND 6
 #define BODY_PART_ID_RIGHT_HAND 7
 
 #define NUM_SHAPES_OF_ARMORS 6
 #define ARMOR_SHAPE_HEAD 'A'
 #define ARMOR_SHAPE_CHEST 'W'
 #define ARMOR_SHAPE_BACK 'Q'
 #define ARMOR_SHAPE_HANDS 'U'
 #define ARMOR_SHAPE_LEGS 'L'
 #define ARMOR_SHAPE_FEET 'F'
 */

Armor *createRandomArmor() {
	char shape[NUM_SHAPES_OF_ARMORS] = {'A', 'W', 'Q', 'U', 'L', 'F'};
	
	int indexRandShape = rand() % NUM_SHAPES_OF_ARMORS;
	return createRandomArmorByShape(shape[indexRandShape]);
}

#define NUM_RANDOM_ARMORS 6
Armor *createRandomArmorByShape(char shapeToCreate) {
	char shape[NUM_RANDOM_ARMORS] =			{ARMOR_SHAPE_HEAD,	ARMOR_SHAPE_CHEST,	ARMOR_SHAPE_BACK,		ARMOR_SHAPE_HANDS,		ARMOR_SHAPE_LEGS,		ARMOR_SHAPE_FEET};
	char name[NUM_RANDOM_ARMORS][80] =		{"Wizard\'s Hat",	"Zeus\' Chest",		"Lupin\'s Cape",		"Lupin\'s Gloves",		"Zeus\' Grieves",		"Poseidon\'s Boots"};
	int price[NUM_RANDOM_ARMORS] =			{10,				10000,				1000,					1000,					10000,					1000};
	int def[NUM_RANDOM_ARMORS] =			{1,					100,				50,						50,						100,					50};
	int durability[NUM_RANDOM_ARMORS] =		{100,				10000,				1000,					1000,					10000,					1000};
	int bodyPartID[NUM_RANDOM_ARMORS] =		{BODY_PART_ID_HEAD,	BODY_PART_ID_CHEST,	BODY_PART_ID_BACK,		BODY_PART_ID_HANDS,		BODY_PART_ID_LEGS,		BODY_PART_ID_FEET};
	
	int bodyPartIDToCreate = -1;

	if (shapeToCreate == ARMOR_SHAPE_HEAD) {
		bodyPartIDToCreate = BODY_PART_ID_HEAD;
	}
	else if (shapeToCreate == ARMOR_SHAPE_CHEST) {
		bodyPartIDToCreate = BODY_PART_ID_CHEST;
	}
	else if (shapeToCreate == ARMOR_SHAPE_BACK) {
		bodyPartIDToCreate = BODY_PART_ID_BACK;
	}
	else if (shapeToCreate == ARMOR_SHAPE_HANDS) {
		bodyPartIDToCreate = BODY_PART_ID_HANDS;
	}
	else if (shapeToCreate == ARMOR_SHAPE_LEGS) {
		bodyPartIDToCreate = BODY_PART_ID_LEGS;
	}
	else if (shapeToCreate == ARMOR_SHAPE_FEET) {
		bodyPartIDToCreate = BODY_PART_ID_FEET;
	}
	else {
		cout << "fatal error: No such armor shape \"" << shapeToCreate << "\" exist!!" << endl;
		exit(1);
	}

	int randIndex = -1;
	
	while (true) {
		randIndex = rand() % NUM_RANDOM_ARMORS;
		if (bodyPartID[randIndex] == bodyPartIDToCreate) {
			break;
		}
	}
	
	return new Armor(shape[randIndex],
						 name[randIndex],
						 price[randIndex],
						 def[randIndex],
						 durability[randIndex],
						 bodyPartID[randIndex]);
}

//------------------------------------------------------------------------------
// class Weapon
//------------------------------------------------------------------------------

Weapon::Weapon() : EquippableItem(DEFAULT_WEAPON_SHAPE, DEFAULT_WEAPON_NAME, DEFAULT_WEAPON_PRICE) {
    atk = DEFAULT_ARMOR_DEF;
    durability = DEFAULT_ARMOR_DURABILITY;
    numHands = 0;
}

Weapon::Weapon(Weapon &other) : EquippableItem(other) {
    atk = other.atk;
    durability = other.durability;
    numHands = other.numHands;
}

Weapon::Weapon(char shape, string name, int price, int atk, int durability, int numHands) : EquippableItem(shape, name, price) {
    this->atk = atk;
    this->durability = durability;
    this->numHands = numHands;
}

Weapon::~Weapon() {
}

void Weapon::printFullSpec() {
    Item::printFullSpec();
    cout << " | Atk " << setw(MAX_LEN_ITEM_ATK) << atk << " | Durability " << setw(MAX_LEN_ITEM_DURABILITY) << durability << " | " << numHands << " hand(s)";
}

string Weapon::getItemID() {
    return string(ITEM_ID_WEAPON);
}

int Weapon::getAtk() {
    return atk;
}

int Weapon::getDurability() {
    return durability;
}

int Weapon::getNumHands() {
    return numHands;
}

bool Weapon::use(Unit *unit) {
    if (unit->equip(this)) {
        equipped = true;
        return true;
    }
    return false;
}

bool Weapon::isWeapon() {
    return true;
}

void Weapon::save(ostream &out) {
    EquippableItem::save(out);
    
    out << "#---------------------------------------- class Weapon" << endl;
    
    out << "# atk" << endl;
    out << atk << endl;
    out << "# durability" << endl;
    out << durability << endl;
    out << "# numHands" << endl;
    out << numHands << endl;
}

void Weapon::load(istream &in) {
    EquippableItem::load(in);
    
    char buf[MAX_LEN_BUF];  
    
    //out << "#---------------------------------------- class Weapon" << endl;
    in.getline(buf, MAX_LEN_BUF); // skip a line
    
    //out << "# atk" << endl;
    in.getline(buf, MAX_LEN_BUF); // skip a line
    //out << atk << endl;
    in >> atk;
    in.get(); // skip a char(new line = '\n')  
    
    //out << "# durability" << endl;
    in.getline(buf, MAX_LEN_BUF); // skip a line
    //out << durability << endl;
    in >> durability;
    in.get(); // skip a char(new line = '\n')  
    
    //out << "# numHands" << endl;
    in.getline(buf, MAX_LEN_BUF); // skip a line
    //out << numHands << endl;
    in >> numHands;
    in.get(); // skip a char(new line = '\n')  
}

/*
 #define NUM_SHAPES_OF_WEAPONS 2
 #define WEAPON_SHAPE_ONE_HAND '!'
 #define WEAPON_SHAPE_TWO_HAND '?'

 #define WEAPON_ONE_HAND 1
 #define WEAPON_TWO_HAND 2
 */

Weapon *createRandomWeapon() {
	char shape[NUM_SHAPES_OF_WEAPONS] = {'!', '?'};
	
	int indexRandShape = rand() % NUM_SHAPES_OF_WEAPONS;
	return createRandomWeaponByShape(shape[indexRandShape]);
}

#define NUM_RANDOM_WEAPONS 3
Weapon *createRandomWeaponByShape(char shapeToCreate) {
	char shape[NUM_RANDOM_ARMORS] =			{WEAPON_SHAPE_ONE_HAND,	WEAPON_SHAPE_ONE_HAND,	WEAPON_SHAPE_TWO_HAND};
	char name[NUM_RANDOM_ARMORS][80] =		{"Wizard\'s Wand",		"Poseidon\'s Sword",	"Zeus\'s Axe"};
	int price[NUM_RANDOM_ARMORS] =			{10,					1000,					10000};
	int atk[NUM_RANDOM_ARMORS] =			{10,					50,						100};
	int durability[NUM_RANDOM_ARMORS] =		{100,					500,					1000};
	int numHands[NUM_RANDOM_ARMORS] =		{WEAPON_ONE_HAND,		WEAPON_ONE_HAND,		WEAPON_TWO_HAND};

	int numHandsToCreate = -1;
	
	if (shapeToCreate == WEAPON_SHAPE_ONE_HAND) {
		numHandsToCreate = WEAPON_ONE_HAND;
	}
	else if (shapeToCreate == WEAPON_SHAPE_TWO_HAND) {
		numHandsToCreate = WEAPON_TWO_HAND;
	}
	else {
		cout << "fatal error: No such weapon shape \"" << shapeToCreate << "\" exist!!" << endl;
		exit(1);
	}
	
	int randIndex = -1;
	
	while (true) {
		randIndex = rand() % NUM_RANDOM_WEAPONS;
		if (numHands[randIndex] == numHandsToCreate) {
			break;
		}
	}
	
	return new Weapon(shape[randIndex],
					  name[randIndex],
					  price[randIndex],
					  atk[randIndex],
					  durability[randIndex],
					  numHands[randIndex]);
}


