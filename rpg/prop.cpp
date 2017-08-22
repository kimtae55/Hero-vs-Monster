#include <iostream>
#include <string>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

using namespace std;

#include "main.h"
#include "item.h"
#include "unit.h"
#include "board.h"
#include "prop.h"

#include "sim.h"
extern Sim *sim; // main.cpp에서 global variable로 만들어진 sim을 쓰겠다는 얘기.

//==============================================================================
// 1st Generation
//==============================================================================

//------------------------------------------------------------------------------
// class Prop
//------------------------------------------------------------------------------

Prop::Prop() {
    shape = DEFAULT_PROP_SHAPE;
    disposed = false;
    row = -1;
    col = -1;
}

Prop::Prop(char shape) {
    this->shape = shape;
    disposed = false;
    row = -1;
    col = -1;
}

Prop::~Prop() {
}

void Prop::print() {
    cout << shape;
}

char Prop::getShape() {
	return shape;
}

int Prop::getRow() {
    return row;
}

void Prop::setRow(int row) {
    this->row = row;
}

int Prop::getCol() {
    return col;
}

void Prop::setCol(int col) {
    this->col = col;
}

bool Prop::isClimbable() {
	return false;
}

bool Prop::isExit() {
	return false;
}

bool Prop::isFunctionable() {
    return false;
}

bool Prop::interact(Unit *otherUnit) {
    // N/A
    return false;
}

bool Prop::hasBeenDisposed() {
    return disposed;
}

void Prop::save(ostream &out) {
    out << "#---------------------------------------- class Prop" << endl;
    
    out << "# shape" << endl;
    out << shape << endl;
    out << "# disposed" << endl;
    out << disposed << endl;
    out << "# row" << endl;
    out << row << endl;
    out << "# col" << endl;
    out << col << endl;
}

void Prop::load(istream &in) {
    char buf[MAX_LEN_BUF];
    
    //out << "#---------------------------------------- class Prop" << endl;
    in.getline(buf, MAX_LEN_BUF); // skip a line
    
    //out << "# shape" << endl;
    in.getline(buf, MAX_LEN_BUF); // skip a line
    //out << shape << endl;
    in >> shape;
    in.get(); // skip a char(new line = '\n')
    
    //out << "# disposed" << endl;
    in.getline(buf, MAX_LEN_BUF); // skip a line
    //out << disposed << endl;
    in >> disposed;
    in.get(); // skip a char(new line = '\n')
    
    //out << "# row" << endl;
    in.getline(buf, MAX_LEN_BUF); // skip a line
    //out << row << endl;
    in >> row;
    in.get(); // skip a char(new line = '\n')
    
    //out << "# col" << endl;
    in.getline(buf, MAX_LEN_BUF); // skip a line
    //out << col << endl;
    in >> col;
    in.get(); // skip a char(new line = '\n')
}

Prop *createPropByID(string propID) {
    if (propID == PROP_ID_TREE) {
        return new Tree();
    }
    else if (propID == PROP_ID_LOCKED_CHEST) {
        return new LockedChest();
    }
	else if (propID == PROP_ID_WALL) {
		return new Wall();
	}
	else if (propID == PROP_ID_PORTAL) {
		return new Portal();
	}
	else if (propID == PROP_ID_FOUNTAIN) {
		return new Fountain();
	}
	else if (propID == PROP_ID_EXIT) {
		return new Exit();
	}
    else {
        cout << "error: no such prop exist! propID = \"" << propID << "\"" << endl;
        exit(1);
    }
    
    return NULL;
}

//==============================================================================
// 2nd Generation
//==============================================================================

//------------------------------------------------------------------------------
// class NonFunctionableProp
//------------------------------------------------------------------------------

NonFunctionableProp::NonFunctionableProp() : Prop(DEFAULT_NON_FUNCTIONAL_PROP_SHAPE) {
}

NonFunctionableProp::NonFunctionableProp(char shape) : Prop(shape) {
}

NonFunctionableProp::~NonFunctionableProp() {
}

void NonFunctionableProp::save(ostream &out) {
    Prop::save(out);

    out << "#---------------------------------------- class NonFunctionableProp" << endl;
}

void NonFunctionableProp::load(istream &in) {
    Prop::load(in);
    
    char buf[MAX_LEN_BUF];
    
    //out << "#---------------------------------------- class NonFunctionableProp" << endl;
    in.getline(buf, MAX_LEN_BUF); // skip a line
}

//------------------------------------------------------------------------------
// class FunctionableProp
//------------------------------------------------------------------------------

FunctionableProp::FunctionableProp() : Prop(DEFAULT_FUNCTIONAL_PROP_SHAPE) {
}

FunctionableProp::FunctionableProp(char shape) : Prop(shape) {
}

FunctionableProp::~FunctionableProp() {
}

bool FunctionableProp::isFunctionable() {
    return true;
}

void FunctionableProp::save(ostream &out) {
    Prop::save(out);
    
    out << "#---------------------------------------- class FunctionableProp" << endl;
}

void FunctionableProp::load(istream &in) {
    Prop::load(in);

    char buf[MAX_LEN_BUF];
    
    //out << "#---------------------------------------- class FunctionableProp" << endl;
    in.getline(buf, MAX_LEN_BUF); // skip a line
}

//==============================================================================
// 3rd Generation
//==============================================================================

//------------------------------------------------------------------------------
// class Tree
//------------------------------------------------------------------------------

Tree::Tree() : NonFunctionableProp(DEFAULT_TREE_SHAPE) {
}

Tree::Tree(char shape) : NonFunctionableProp(shape) {
}

Tree::~Tree() {
}

string Tree::getPropID() {
    return string(PROP_ID_TREE);
}

void Tree::save(ostream &out) {
    NonFunctionableProp::save(out);
    
    out << "#---------------------------------------- class Tree" << endl;
}

void Tree::load(istream &in) {
    NonFunctionableProp::load(in);
    
    char buf[MAX_LEN_BUF];
    
    //out << "#---------------------------------------- class Tree" << endl;
    in.getline(buf, MAX_LEN_BUF); // skip a line
}

//------------------------------------------------------------------------------
// class LockedChest
//------------------------------------------------------------------------------

LockedChest::LockedChest() : FunctionableProp(DEFAULT_LOCKED_CHEST_SHAPE) {
	if (rand() % 2 == 0) {
		item = createRandomArmor();
	}
	else {
		item = createRandomWeapon();
	}
	
	item->printFullSpec();
}

LockedChest::LockedChest(Item *item) : FunctionableProp(DEFAULT_LOCKED_CHEST_SHAPE) {
    this->item = item;
}

LockedChest::LockedChest(char shape, Item *item) : FunctionableProp(shape) {
    this->item = item;
}

LockedChest::~LockedChest() {
    if (item != NULL) {
        delete item;
    }
}

string LockedChest::getPropID() {
    return string(PROP_ID_LOCKED_CHEST);
}

bool LockedChest::interact(Unit *otherUnit) {
    if (otherUnit->canOpenLockedChest()) {
        sim->board->setItem(row, col, item);
        item = NULL;
        disposed = true;
        return true;
    }
    return false;
}

void LockedChest::save(ostream &out) {
    FunctionableProp::save(out);
    
    out << "#---------------------------------------- class LockedChest" << endl;
    
    out << "# Item" << endl;
    if (item == NULL) {
        out << TOMB_STONE_NON_EXIST << endl;
    }
    else {
        out << TOMB_STONE_EXIST << endl;
        
        out << "# Item class type" << endl;
        out << item->getItemID() << endl;
        
        item->save(out);
    }
}

void LockedChest::load(istream &in) {
    FunctionableProp::load(in);
    
    char buf[MAX_LEN_BUF];
    
    //out << "#---------------------------------------- class LockedChest" << endl;
    in.getline(buf, MAX_LEN_BUF); // skip a line
    
    //out << "# Item" << endl;
    in.getline(buf, MAX_LEN_BUF); // skip a line

    //out << TOMB_STONE_EXIST << endl;
    string tombStone = "";
    in >> tombStone;
    in.get(); // skip a char(new line = '\n')
    
    if (tombStone == TOMB_STONE_EXIST) {
        //out << "# Item class type" << endl;
        in.getline(buf, MAX_LEN_BUF); // skip a line

        //out << item->getItemID() << endl;
        string itemID = "";
        in >> itemID;
        in.get(); // skip a char(new line = '\n')

        item = createItemByID(itemID); // create an object using factory.
        
        //item->save(out);
        item->load(in);
    }
}

//------------------------------------------------------------------------------
// class Wall
//------------------------------------------------------------------------------

Wall::Wall() : NonFunctionableProp(WALL_TYPE_JOINT_SHAPE) {
	wallType = WALL_TYPE_JOINT;
}

Wall::Wall(int wallType) : NonFunctionableProp() {
	this->wallType = wallType;
	if (wallType == WALL_TYPE_VERTICAL) {
		shape = WALL_TYPE_VERTICAL_SHAPE;
	}
	if (wallType == WALL_TYPE_HORIZONTAL) {
		shape = WALL_TYPE_HORIZONTAL_SHAPE;
	}
	if (wallType == WALL_TYPE_JOINT) {
		shape = WALL_TYPE_JOINT_SHAPE;
	}
}

Wall::~Wall() {
}

string Wall::getPropID() {
	return PROP_ID_WALL;
}

int Wall::getWallType() {
	return wallType;
}

void Wall::save(ostream &out) {
	NonFunctionableProp::save(out);
	
	out << "#---------------------------------------- class Wall" << endl;
	
	out << "# wallType" << endl;
	out << wallType << endl;
}

void Wall::load(istream &in) {
	NonFunctionableProp::load(in);
	
	char buf[MAX_LEN_BUF];
	
	//out << "#---------------------------------------- class Wall" << endl;
	in.getline(buf, MAX_LEN_BUF); // skip a line
	
	//out << "# wallType" << endl;
	in.getline(buf, MAX_LEN_BUF); // skip a line
	//out << wallType << endl;
	in >> wallType;
	in.get(); // skip a char(new line = '\n')
}

//------------------------------------------------------------------------------
// class Fountain
//------------------------------------------------------------------------------

Fountain::Fountain() : FunctionableProp(DEFAULT_FOUNTAIN_SHAPE) {
	hp = DEFAULT_FOUNTAIN_HP;
	mp = DEFAULT_FOUNTAIN_MP;
}

Fountain::Fountain(int hp, int mp) : FunctionableProp(DEFAULT_FOUNTAIN_SHAPE) {
	this->hp = hp;
	this->mp = mp;
}

Fountain::Fountain(char shape, int hp, int mp) : FunctionableProp(shape) {
	this->hp = hp;
	this->mp = mp;
}

Fountain::~Fountain() {
}

string Fountain::getPropID() {
	return string(PROP_ID_FOUNTAIN);
}

bool Fountain::interact(Unit *otherUnit) {
	if (otherUnit == NULL) {
		return false;
	}
	
	bool resultIncHp = otherUnit->incHp(hp);
	bool resultIncMp = otherUnit->incMp(hp);
	
	if (resultIncHp || resultIncMp) {
		return true;
	}
	// this fountain MUST NOT be empty when the unit's hp and mp are full.
	else {
		return false;
	}
}

void Fountain::save(ostream &out) {
	FunctionableProp::save(out);
	
	out << "#---------------------------------------- class Fountain" << endl;
	
	out << "# hp" << endl;
	out << hp << endl;
	out << "# mp" << endl;
	out << mp << endl;
}

void Fountain::load(istream &in) {
	FunctionableProp::load(in);
	
	char buf[MAX_LEN_BUF];
	
	//out << "#---------------------------------------- class Fountain" << endl;
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
// class Portal
//------------------------------------------------------------------------------

Portal::Portal() : FunctionableProp(DEFAULT_PORTAL_SHAPE) {
}

Portal::Portal(char shape) : FunctionableProp(shape) {
}

Portal::~Portal() {
}

bool Portal::isClimbable() {
	return true;
}

string Portal::getPropID() {
	return string(PROP_ID_PORTAL);
}

bool Portal::interact(Unit *otherUnit) {
	// create a Merchant object
	while (true) {
		int randRow = rand() % sim->board->getRowSize();
		int randCol = rand() % sim->board->getColSize();
		
		if (sim->board->getUnit(randRow, randCol) == NULL &&
			sim->board->getProp(randRow, randCol) == NULL &&
			sim->board->getItem(randRow, randCol) == NULL) {
			sim->board->setUnit(otherUnit->getRow(), otherUnit->getCol(), NULL); // remove hero from the original location.
			sim->board->setUnit(randRow, randCol, otherUnit); // hero has been placed at the (new) random location.
			break;
		}
	}
	return true;
}

void Portal::save(ostream &out) {
	FunctionableProp::save(out);
	
	out << "#---------------------------------------- class Portal" << endl;
}

void Portal::load(istream &in) {
	FunctionableProp::load(in);
	
	char buf[MAX_LEN_BUF];
	
	//out << "#---------------------------------------- class Portal" << endl;
	in.getline(buf, MAX_LEN_BUF); // skip a line
}

//------------------------------------------------------------------------------
// class Exit
//------------------------------------------------------------------------------

Exit::Exit() : NonFunctionableProp(DEFAULT_EXIT_SHAPE) {
	
}

Exit::Exit(char shape) : NonFunctionableProp(shape) {
}

Exit::~Exit() {
}

bool Exit::isClimbable() {
	return true;
}

bool Exit::isExit() {
	return true;
}

string Exit::getPropID() {
	return string(PROP_ID_EXIT);
}

void Exit::save(ostream &out) {
	NonFunctionableProp::save(out);
	
	out << "#---------------------------------------- class Exit" << endl;
}

void Exit::load(istream &in) {
	NonFunctionableProp::load(in);
	
	char buf[MAX_LEN_BUF];
	
	//out << "#---------------------------------------- class Exit" << endl;
	in.getline(buf, MAX_LEN_BUF); // skip a line
}
