#include <iostream>
#include <iomanip>
#include <stdio.h>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

using namespace std;

#include "main.h"
#include "item.h"
#include "unit.h"
#include "board.h"
#include "merchant.h"

#ifdef VISUAL_STUDIO
#include <windows.h>
#include <conio.h>
#else
extern int _kbhit(void);
#endif

//------------------------------------------------------------------------------
// class Merchant
//------------------------------------------------------------------------------

Merchant::Merchant() : Unit(DEFAULT_MERCHANT_SHAPE, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0) {
    itemsToSell = new Backpack<Item *>(DEFAULT_MERCHANT_ITEMS_TO_SELL_MAX_NUM_ITEMS);
    itemsBought = new Backpack<Item *>(DEFAULT_MERCHANT_ITEMS_BOUGHT_MAX_NUM_ITEMS);
	initItemsToSell();
}

Merchant::Merchant(char shape, int row, int col) : Unit(shape, row, col, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0) {
    itemsToSell = new Backpack<Item *>(DEFAULT_MERCHANT_ITEMS_TO_SELL_MAX_NUM_ITEMS);
    itemsBought = new Backpack<Item *>(DEFAULT_MERCHANT_ITEMS_BOUGHT_MAX_NUM_ITEMS);

	initItemsToSell();
}

void Merchant::initItemsToSell() {
	for (int i = 0; i < DEFAULT_MERCHANT_ITEMS_TO_SELL_MAX_NUM_ITEMS; i++) {
		if (rand() % 2 == 0) {
			addItemToSell(createRandomArmor());
		}
		else {
			addItemToSell(createRandomWeapon());
		}
	}
}

Merchant::~Merchant() {
	// delete all the "Item *" in itemsToSell using iteration.
	itemsToSell->resetIterator();
	
	while (itemsToSell->hasMore()) {
		Item *currentItem = itemsToSell->next();
		if (currentItem != NULL) {
			delete currentItem;
		}
	}
	
    delete itemsToSell;
	
	// delete all the "Item *" in itemsBought using iteration.
	itemsBought->resetIterator();
	
	while (itemsBought->hasMore()) {
		Item *currentItem = itemsBought->next();
		if (currentItem != NULL) {
			delete currentItem;
		}
	}
	
    delete itemsBought;
}

string Merchant::getUnitID() {
    return string(UNIT_ID_MERCHANT);
}

void Merchant::move(int dir) {
    // N/A
}

bool Merchant::isMovable() {
    return false; // merchant등 안 움직이는 애들만 false를 return하게 한다.
}

bool Merchant::isMerchant() {
    return true;
}

// buy & sell
void Merchant::interact(Unit *otherUnit) {
    if (otherUnit == NULL) {
        return;
    }

    while (true) {
        cout << "(B)uy or (S)ell ('q' to exit): ";
        char command;
        while (!_kbhit()) {}
#ifdef VISUAL_STUDIO
        command = _getch();
#else
        command = getchar();
#endif
        if (command == 'b') {
            sell(otherUnit);
        }
        else if (command == 's') {
            buy(otherUnit);
        }
        else if (command == 'q') {
            break;
        }
    }
    cout << "Merchant: Have a good one!! ^^" << endl;
}

void Merchant::buy(Unit *otherUnit) {
    while (true) {
        // show the user the hero's item list.
        otherUnit->printItemsToSell();
        
        cout << "You have " << otherUnit->getGold() << " gold" << endl;
        cout << "Enter a number to sell (-1 to exit) : ";
        int itemNum = -1;
        cin >> itemNum;
        
        if (itemNum == -1) { // exit
            break;
        }

        Item *item = otherUnit->getItemsToSell(itemNum);
        if (item == NULL) {
            cout << "The item number is invalid or not available!!" << endl;
        }
        // buy the item from the hero.
        else {
            itemsBought->insert(item);
            otherUnit->incGold(item->getPrice());
            cout << "You received " << item->getPrice() << " gold!!" << endl;
        }
    }
    // skip enter code
#ifdef VISUAL_STUDIO
    _getch();
#else
    getchar();
#endif
}

void Merchant::sell(Unit *otherUnit) {
    while (true) {
        // show the unit the menu(itemToSell)
        cout << ">>>>>>>>>>>>>>>>>>>> Items <<<<<<<<<<<<<<<<<<<<" << endl;
        cout << endl;
        for (int i = 0; i < itemsToSell->size(); i++) {
            cout << setw(MAX_LEN_ITEM_INDEX) << i+1 << ": [ ";
            itemsToSell->getItem(i)->printFullSpec();
            cout << " ]" << endl;
        }
        cout << endl;
        cout << ">>>>> Items(the merchant bought from you) <<<<<" << endl;
        cout << endl;
        for (int i = 0; i < itemsBought->size(); i++) {
            cout << setw(MAX_LEN_ITEM_INDEX) << i+101 << ": [ ";
            itemsBought->getItem(i)->printFullSpec();
            cout << " ]" << endl;
        }
        cout << endl;
        cout << "You have " << otherUnit->getGold() << " gold" << endl;
        cout << "Enter a number to purchase (-1 to exit) : ";
        int index = -1;
        cin >> index;
        
        if (index == -1) { // exit
            break;
        }
        
        // sell from itemsToSell
        if (index >= 1 && index <= itemsToSell->size()) {
            
            Item *itemToCopy = itemsToSell->getItem(index-1);
            
            // check if the unit has enough gold to buy the item.
            if (otherUnit->getGold() < itemToCopy->getPrice()) {
                cout << "You don't have enough gold to buy the item!!" << endl;
            }
            // check if the unit can receive an item.
            else if (!otherUnit->canReceiveAnItem()) {
                cout << "You don't have enough room to receive the item!!" << endl;
            }
            // sell!
            else {
                // deep copy
                Item *itemClone = createItemByID(itemToCopy->getItemID(), itemToCopy);
                otherUnit->receiveAnItem(itemClone);
                // pay!
                otherUnit->decGold(itemToCopy->getPrice());
                cout << "Thank you!! Check your inventory!!" << endl;
            }
        }
        // sell from itemsBought
        else if (index >= 101 && index <= itemsBought->size()+100) {
            Item *itemToSell = itemsBought->getItem(index-101);
            
            // check if the unit has enough gold to buy the item.
            if (otherUnit->getGold() < itemToSell->getPrice()) {
                cout << "You don't have enough gold to buy the item!!" << endl;
            }
            // check if the unit can receive an item.
            else if (!otherUnit->canReceiveAnItem()) {
                cout << "You don't have enough room to receive the item!!" << endl;
            }
            // sell!
            else {
                // DO NOT deep copy
                itemsBought->removeItem(index-101);
                otherUnit->receiveAnItem(itemToSell);
                // pay!
                otherUnit->decGold(itemToSell->getPrice());
                cout << "Thank you!! Check your inventory!!" << endl;
            }
        }
    }
    // skip enter code
#ifdef VISUAL_STUDIO
    _getch();
#else
    getchar();
#endif
}

bool Merchant::addItemToSell(Item *item) { // class Merchant only and used by Board::initUnits()
    return itemsToSell->insert(item);
}

void Merchant::save(ostream &out) {
    Unit::save(out);
    
    out << "#---------------------------------------- class Merchant" << endl;
    
    out << "#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ itemsToSell begin" << endl;
    itemsToSell->save(out);
    out << "#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ itemsToSell end" << endl;
    
    out << "#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ itemsBought begin" << endl;
    itemsBought->save(out);
    out << "#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ itemsBought end" << endl;
}

void Merchant::load(istream &in) {
    Unit::load(in);
    
    char buf[MAX_LEN_BUF];
    
    //out << "#---------------------------------------- class Merchant" << endl;
    in.getline(buf, MAX_LEN_BUF); // skip a line
    
    //out << "#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ itemsToSell begin" << endl;
    in.getline(buf, MAX_LEN_BUF); // skip a line
    //itemsToSell->save(out);
    itemsToSell = new Backpack<Item *>();
    itemsToSell->load(in);
    //out << "#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ itemsToSell end" << endl;
    in.getline(buf, MAX_LEN_BUF); // skip a line
    
    //out << "#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ itemsBought begin" << endl;
    in.getline(buf, MAX_LEN_BUF); // skip a line
    //itemsBought->save(out);
    itemsBought = new Backpack<Item *>();
    itemsBought->load(in);
    //out << "#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ itemsBought end" << endl;
    in.getline(buf, MAX_LEN_BUF); // skip a line
}
