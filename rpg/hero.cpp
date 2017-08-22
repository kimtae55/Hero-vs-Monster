#include <iostream>
#include <iomanip>

using namespace std;

#include "main.h"
#include "prop.h"
#include "item.h"
#include "unit.h"
#include "board.h"
#include "inventory.h"
#include "paperdoll.h"
#include "hero.h"

#include "sim.h"
extern Sim *sim; // main.cpp에서 global variable로 만들어진 sim을 쓰겠다는 얘기.

Hero::Hero() : Unit(DEFAULT_HERO_SHAPE, -1, -1, DEFAULT_HERO_MAX_HP, DEFAULT_HERO_HP,
                    DEFAULT_HERO_MAX_MP, DEFAULT_HERO_MP, DEFAULT_HERO_GOLD,
					DEFAULT_HERO_MAX_EXP, DEFAULT_HERO_EXP,
                    DEFAULT_HERO_ATK, DEFAULT_HERO_DEF, DEFAULT_HERO_LEVEL) {
    slots = new Inventory();
	backpack = new Backpack<Item *>();
    paperdoll = new Paperdoll();
	escaped = false;
}

Hero::~Hero() {
	destroyHero();
}

void Hero::destroyHero() {
	delete slots;
	slots = NULL;
	
	// delete all the "Item *" in backpack using iteration.
	backpack->resetIterator();
	while (backpack->hasMore()) {
		Item *currentItem = backpack->next();
		if (currentItem != NULL) {
			delete currentItem;
		}
	}
	delete backpack;
	backpack = NULL;
	
	delete paperdoll;
	paperdoll = NULL;
}

string Hero::getUnitID() {
    return string(UNIT_ID_HERO);
}

void Hero::printStat() {
    Unit::printStat();
    cout << endl;
    slots->print();
	cout << endl;
	
	// print all the "Item *" in backpack using iteration.
	cout << "[ ";
	backpack->resetIterator();
	int i = 0;
	while (backpack->hasMore()) {
		Item *currentItem = backpack->next();
		cout << i+1 << "(";
		if (currentItem != NULL) {
			currentItem->print();
		}
		else {
			cout << " ";
		}
		cout << ") ";
		i++;
	}
	cout << "]" << endl;
	
    paperdoll->print();
    cout << endl;
}

void Hero::printItemsToSell() {
    // show the unit the menu(slots)
    cout << ">>>>>>>>>>>>>>>>>>>> Items in slots <<<<<<<<<<<<<<<<<<<<" << endl;
    cout << endl;
    for (int i = 0; i < slots->capacity(); i++) {
        Item *currentItem = slots->getItem(i);
        if (currentItem != NULL) {
            cout << setw(3) << i+1 << ": [ ";
            currentItem->printFullSpec();
            cout << " ]" << endl;
        }
        else {
            cout << setw(3) << i+1 << ": [ None ]" << endl;
        }
    }
    cout << endl;
	
	// show the unit the menu(backpack)
	cout << ">>>>>>>>>>>>>>>>>>>> Items in backpack <<<<<<<<<<<<<<<<<<<<" << endl;
	cout << endl;
	for (int i = 0; i < backpack->size(); i++) {
		Item *currentItem = backpack->getItem(i);
		if (currentItem != NULL) {
			cout << setw(3) << i+101 << ": [ ";
			currentItem->printFullSpec();
			cout << " ]" << endl;
		}
		else {
			cout << setw(3) << i+101 << ": [ None ]" << endl;
		}
	}
	cout << endl;

}

Item *Hero::getItemsToSell(int itemNum) { // itemNum begins from 1
	if (itemNum >= 1 && itemNum <= slots->capacity()) { // index of slots
		return slots->removeItem(itemNum-1);;
	}
	else if (itemNum >= 101 && itemNum < 101 + backpack->size()) { // index of backpack
		return backpack->removeItem(itemNum-101);
	}
	else {
		return NULL;
	}
}

void Hero::move(int dir) {
    if (freezed) {
        return;
    }
    
    int prevRow = row;
    int prevCol = col;

    bool moved = false;
    
    // decide the next position.
    int nextRow = -1;
    int nextCol = -1;

    if (dir == DIR_NORTH) {
        nextRow = row-1;
        nextCol = col;
    }
    else if (dir == DIR_EAST) {
        nextRow = row;
        nextCol = col+1;
    }
    else if (dir == DIR_SOUTH) {
        nextRow = row+1;
        nextCol = col;
    }
    else if (dir == DIR_WEST) {
        nextRow = row;
        nextCol = col-1;
    }
    
    // move the hero when the next position is empty(no unit && no prop).
    if (sim->board->validate(nextRow, nextCol) &&
       sim->board->getUnit(nextRow, nextCol) == NULL &&
        (sim->board->getProp(nextRow, nextCol) == NULL ||sim->board->getProp(nextRow, nextCol)->isClimbable())) {
       sim->board->setUnit(prevRow, prevCol, NULL);
       sim->board->setUnit(nextRow, nextCol, this);
        // now row and col have new coord.
        moved = true;
    }
    
    // pick up the item under my feet.
    if (moved) {
        if (sim->board->getItem(row, col) != NULL) {
            if (slots->insert(sim->board->getItem(row, col))) {
               sim->board->setItem(row, col, NULL);
            }
        }
    }

    // unfreeze all the monsters around my previous position when I moved.
    if (moved) {
        for (int i = prevRow-1; i <= prevRow+1; i++) {
            for (int j = prevCol-1; j <= prevCol+1; j++) {
                Unit *curUnit =sim->board->getUnit(i, j);
                if (curUnit != NULL && curUnit->isMonster()) {
                    curUnit->unfreeze();
                }
            }
        }
    }

    // freeze all the monsters around me when I moved.
    if (moved) {
        for (int i = row-1; i <= row+1; i++) {
            for (int j = col-1; j <= col+1; j++) {
                Unit *curUnit =sim->board->getUnit(i, j);
                if (curUnit != NULL && curUnit->isMonster()) {
                    curUnit->freeze();
                }
            }
        }
    }
    
    // interact when the monster has been engaged.
    if (!moved) {
        Unit *otherUnit =sim->board->getUnit(nextRow, nextCol);
        if (otherUnit != NULL) {
            unitEngaged = otherUnit;

            // "interact": Monster는 battle, Merchant는 buyAndSell, Neutral은 talk, ...
			if (!otherUnit->isDead()) {
				otherUnit->interact(this);
			}
        }
    }
    
    // interact when a FunctionableProp has been detected.
	Prop *propToInteract =sim->board->getProp(nextRow, nextCol);
	if (propToInteract != NULL && propToInteract->isFunctionable()) {
		// "interact": Monster는 battle, Merchant는 buyAndSell, Neutral은 talk, ...
		if (propToInteract->interact(this)) {
			if (propToInteract->hasBeenDisposed()) {
				sim->board->setProp(nextRow, nextCol, NULL);
				delete propToInteract;
			}
		}
	}

	// check wheter the new hero's location has an Exit object or not.
	if (moved) {
		Prop *propToCheck =sim->board->getProp(nextRow, nextCol);
		if (propToCheck != NULL && propToCheck->isExit()) {
			escaped = true;
		}
	}
}

bool Hero::isHero() {
    return true;
}

void Hero::useItem(int indexSlot) {
    slots->useItem(indexSlot, this);
}

bool Hero::equip(Item *item) {
    return paperdoll->equip(item, this);
}

void Hero::unequip(int bodyPartID) {
    // DO NOT unequip when the inventory is full.
    if (slots->isFull()) {
        cout << "error: Your inventory is full!!!" << endl;
        return;
    }
    
    //cout << "Hero::unequip(): bodyPartID=" << bodyPartID << endl;
    
    // unequip when the inventory is not full.
    Item *itemUnequipped = paperdoll->unequip(bodyPartID, this);
    
    // when the item has been unequipped successfully.
    if (itemUnequipped != NULL) {
        //cout << "Hero::unequip(): if (itemUnequipped != NULL)" << endl;
        slots->insert(itemUnequipped);
        //cout << "Hero::unequip(): slots->insert(itemUnequipped); done" << endl;
    }
}

bool Hero::canOpenLockedChest() {
    return slots->hasKey();
}

bool Hero::canReceiveAnItem() {
    // check if slots can get more items.
    if (!slots->isFull()) {
        return true;
    }
    
    // check if backpack can get more items.
    //if (!backpack->isFull()) {
    //    return true;
    //}
    
    return false;
}

// pre-condition: canReceiveAnItem() == true
void Hero::receiveAnItem(Item *item) {
    // check if slots can get more items.
    if (!slots->isFull()) {
        // insert the item.
        slots->insert(item);
    }
    
    // check if backpack can get more items.
    //else { // if (!backpack->isFull()) {
    //    // insert the item.
    //}
}

bool Hero::hasEscaped() {
	return escaped;
}

void Hero::moveItemSlotsToBackpack(int indexSlot) {
	if (indexSlot < 0 || indexSlot >= slots->capacity()) {
		cout << "error: invalid index!" << endl;
	}
	
	if (backpack->isFull()) {
		cout << "error: backpack is full!" << endl;
		return;
	}
	
	Item *itemToMove = slots->removeItem(indexSlot);
	
	if (itemToMove != NULL) {
		backpack->insert(itemToMove);
	}
	else {
		cout << "error: the slot is empty!" << endl;
	}
}

void Hero::moveItemBackpackToSlots(int indexBackpack) {
	if (indexBackpack < 0 || indexBackpack >= backpack->size()) {
		cout << "error: invalid index!" << endl;
	}

	if (slots->isFull()) {
		cout << "error: slots is full!" << endl;
		return;
	}
	
	Item *itemToMove = backpack->removeItem(indexBackpack);
	
	if (itemToMove != NULL) {
		slots->insert(itemToMove);
	}
	else {
		cout << "error: the backpack slot is empty!" << endl;
	}
}

void Hero::save(ostream &out) {
    Unit::save(out);
    
    out << "#---------------------------------------- class Hero" << endl;
    
    out << "#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ slots begin" << endl;
    slots->save(out);
    out << "#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ slots end" << endl;

	out << "#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ backpack begin" << endl;
	backpack->save(out);
	out << "#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ backpack end" << endl;

    out << "#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ paperdoll begin" << endl;
    paperdoll->save(out);
    out << "#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ paperdoll end" << endl;
	
	out << "# escaped" << endl;
	out << escaped << endl;
}

void Hero::load(istream &in) {
    Unit::load(in);
	
	destroyHero();
	
    char buf[MAX_LEN_BUF];
    
    //out << "#---------------------------------------- class Hero" << endl;
    in.getline(buf, MAX_LEN_BUF); // skip a line
    
    //out << "#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ slots begin" << endl;
    in.getline(buf, MAX_LEN_BUF); // skip a line
    //slots->save(out);
    slots = new Inventory();
    slots->load(in);
    //out << "#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ slots end" << endl;
    in.getline(buf, MAX_LEN_BUF); // skip a line
    
	//out << "#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ backpack begin" << endl;
	in.getline(buf, MAX_LEN_BUF); // skip a line
	//backpack->save(out);
	backpack = new Backpack<Item *>();
	backpack->load(in);
	//out << "#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ backpack end" << endl;
	in.getline(buf, MAX_LEN_BUF); // skip a line
	
    //out << "#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ paperdoll begin" << endl;
    in.getline(buf, MAX_LEN_BUF); // skip a line
    //paperdoll->save(out);
    paperdoll = new Paperdoll();
    paperdoll->load(in);
    //out << "#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ paperdoll end" << endl;
    in.getline(buf, MAX_LEN_BUF); // skip a line
	
	//out << "# escaped" << endl;
	in.getline(buf, MAX_LEN_BUF); // skip a line
	//out << escaped << endl;
	in >> escaped;
	in.get(); // skip a char(new line = '\n')
}
