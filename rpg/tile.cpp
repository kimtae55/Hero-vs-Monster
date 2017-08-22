#include <iostream>
#include <string>

using namespace std;

#include "main.h"
#include "unit.h"
#include "hero.h"
#include "monster.h"
#include "merchant.h"
#include "item.h"
#include "prop.h"
#include "tile.h"

Tile::Tile() {
    unit = NULL;
    item = NULL;
    prop = NULL;
}

void Tile::print() {
    if (unit != NULL) {
        unit->print();
    }
    else if (item != NULL) {
        item->print();
    }
    else if (prop != NULL) {
        prop->print();
    }
    else {
        cout << ".";
    }
}

char Tile::getShape() {
	if (unit != NULL) {
		return unit->getShape();
	}
	else if (item != NULL) {
		return item->getShape();
	}
	else if (prop != NULL) {
		return prop->getShape();
	}
	else {
		return '.';
	}
}

Unit *Tile::getUnit() {
    return unit;
}

void Tile::setUnit(Unit *unit) {
    this->unit = unit;
}

Item *Tile::getItem() {
    return item;
}

void Tile::setItem(Item *item) {
    this->item = item;
}

Prop *Tile::getProp() {
    return prop;
}

void Tile::setProp(Prop *prop) {
    this->prop = prop;
}

// read a char from in and create an appropriate object.
void Tile::loadLevel(istream &in) {
	char shape = in.get();
	cout << shape;

	if (shape == '.') {
		return;
	}
	//--------------------------------------------------------------------------
	// load units
	//--------------------------------------------------------------------------
	else if (shape == DEFAULT_HERO_SHAPE) {
		unit = new Hero();
	}
	else if (shape == DEFAULT_MONSTER_SHAPE) {
		unit = new Monster();
	}
	else if (shape == DEFAULT_BOSS_SHAPE) {
		unit = new Boss();
	}
	else if (shape == DEFAULT_MERCHANT_SHAPE) {
		unit = new Merchant();
	}
	//--------------------------------------------------------------------------
	// load props
	//--------------------------------------------------------------------------
	else if (shape == DEFAULT_TREE_SHAPE) {
		prop = new Tree();
	}
	else if (shape == DEFAULT_LOCKED_CHEST_SHAPE) {
		prop = new LockedChest();
	}
	else if (shape == WALL_TYPE_VERTICAL_SHAPE) {
		prop = new Wall(WALL_TYPE_VERTICAL);
	}
	else if (shape == WALL_TYPE_HORIZONTAL_SHAPE) {
		prop = new Wall(WALL_TYPE_HORIZONTAL);
	}
	else if (shape == WALL_TYPE_JOINT_SHAPE) {
		prop = new Wall(WALL_TYPE_JOINT);
	}
	else if (shape == DEFAULT_FOUNTAIN_SHAPE) {
		prop = new Fountain();
	}
	else if (shape == DEFAULT_PORTAL_SHAPE) {
		prop = new Portal();
	}
	else if (shape == DEFAULT_EXIT_SHAPE) {
		prop = new Exit();
	}
	//--------------------------------------------------------------------------
	// load items
	//--------------------------------------------------------------------------
	else if (shape == DEFAULT_POTION_SHAPE) {
		item = new Potion();
	}
	else if (shape == DEFAULT_KEY_SHAPE) {
		item = new Key();
	}
	else if (shape == ARMOR_SHAPE_HEAD ||
			 shape == ARMOR_SHAPE_CHEST ||
			 shape == ARMOR_SHAPE_BACK ||
			 shape == ARMOR_SHAPE_HANDS ||
			 shape == ARMOR_SHAPE_LEGS ||
			 shape == ARMOR_SHAPE_FEET) {
		item = createRandomArmorByShape(shape);
	}
	else if (shape == WEAPON_SHAPE_ONE_HAND ||
			 shape == WEAPON_SHAPE_TWO_HAND) {
		item = createRandomWeaponByShape(shape);
	}
}

void Tile::save(ostream &out) {
    out << "#---------------------------------------- class Tile" << endl; 
    
    //*********************************************************************
    //*** Unit will be saved by main()(hero) and class Board(mons/bosses).
    //*********************************************************************
    /*
    out << "#^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ unit begin" << endl;
    if (unit == NULL) {
        out << TOMB_STONE_NON_EXIST << endl;
    }
    else {
        out << TOMB_STONE_EXIST << endl;
        
        out << "# Unit class type" << endl;
        out << unit->getUnitID() << endl;

        unit->save(out);
    }
    out << "#^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ unit end" << endl;
    */
    
    out << "#^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ item begin" << endl;
    if (item == NULL) {
        out << TOMB_STONE_NON_EXIST << endl;
    }
    else {
        out << TOMB_STONE_EXIST << endl;
        
        out << "# Item class type" << endl;
        out << item->getItemID() << endl;

        item->save(out);
    }
    out << "#^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ item end" << endl;

    out << "#^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ prop begin" << endl;
    if (prop == NULL) {
        out << TOMB_STONE_NON_EXIST << endl;
    }
    else {
        out << TOMB_STONE_EXIST << endl;
        
        out << "# Prop class type" << endl;
        out << prop->getPropID() << endl;

        prop->save(out);
    }
    out << "#^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ prop end" << endl;
}

void Tile::load(istream &in) {
    char buf[MAX_LEN_BUF];
    
    //out << "#---------------------------------------- class Tile" << endl;
    in.getline(buf, MAX_LEN_BUF); // skip a line

    //*********************************************************************
    //*** Unit will be loaded by main()(hero) and class Board(mons/bosses).
    //*********************************************************************
    unit = NULL;
    /*
    //out << "#^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ unit begin" << endl;
    in.getline(buf, MAX_LEN_BUF); // skip a line
    //out << TOMB_STONE_EXIST << endl;
    string tombStone = "";
    in >> tombStone;
    in.get(); // skip a char(new line = '\n')
    
    if (tombStone == TOMB_STONE_EXIST) {
        //out << "# Unit class type" << endl;
        in.getline(buf, MAX_LEN_BUF); // skip a line
        //out << unit->getUnitID() << endl;
        string unitID = "";
        in >> unitID;
        in.get(); // skip a char(new line = '\n')
        
        unit = createUnitByID(unitID); // create an object using factory.
        
        //unit->save(out);
        unit->load(in);
    }
    else {
        unit = NULL;
    }
    //out << "#^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ unit end" << endl;
    in.getline(buf, MAX_LEN_BUF); // skip a line
    */
    
    //out << "#^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ item begin" << endl;
    in.getline(buf, MAX_LEN_BUF); // skip a line
    //out << TOMB_STONE_EXIST << endl;
    string tombStone = "";
    in >> tombStone;
    in.get(); // skip a char(new line = '\n')
    
    if (tombStone == TOMB_STONE_EXIST) {
        //out << "# Item class type" << endl;
        in.getline(buf, MAX_LEN_BUF); // skip a line
        //out << item->getUnitID() << endl;
        string itemID = "";
        in >> itemID;
        in.get(); // skip a char(new line = '\n')
        
        item = createItemByID(itemID); // create an object using factory.
        
        //item->save(out);
        item->load(in);
    }
    else {
        item = NULL;
    }
    //out << "#^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ item end" << endl;
    in.getline(buf, MAX_LEN_BUF); // skip a line
    
    //out << "#^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ prop begin" << endl;
    in.getline(buf, MAX_LEN_BUF); // skip a line
    //out << TOMB_STONE_EXIST << endl;
    tombStone = "";
    in >> tombStone;
    in.get(); // skip a char(new line = '\n')
    
    if (tombStone == TOMB_STONE_EXIST) {
        //out << "# Prop class type" << endl;
        in.getline(buf, MAX_LEN_BUF); // skip a line
        //out << prop->getPropID() << endl;
        string propID = "";
        in >> propID;
        in.get(); // skip a char(new line = '\n')
        
        prop= createPropByID(propID); // create an object using factory.
        
        //prop->save(out);
        prop->load(in);
    }
    else {
        prop = NULL;
    }
    //out << "#^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ prop end" << endl;
    in.getline(buf, MAX_LEN_BUF); // skip a line
}
