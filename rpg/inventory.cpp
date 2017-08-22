#include <iostream>
#include <string>

using namespace std;

#include "main.h"
#include "item.h"
#include "inventory.h" 

Inventory::Inventory() {
    initInventory(DEFAULT_INVENTORY_MAX_NUM_ITEMS);
}

void Inventory::initInventory(int maxNumItems) {
    this->maxNumItems = maxNumItems;
    numItems = 0;
    
    slots = new Item *[maxNumItems];
    
    for (int i = 0; i < maxNumItems; i++) {
        slots[i] = NULL;
    }
}

Inventory::~Inventory() {
    destroyInventory();
}

void Inventory::destroyInventory() {
    if (slots != NULL) {
        for (int i = 0; i < maxNumItems; i++) {
            if (slots[i] != NULL) {
                delete slots[i];
            }
        }
        
        delete [] slots;
        slots = NULL;
    }
}

void Inventory::print() {
    cout << "[ ";
    for (int i = 0; i < maxNumItems; i++) {
        cout << i+1 << "(";
        if (slots[i] == NULL) {
            cout << " ";
        }
        else {
            slots[i]->print();
        }
        cout << ") ";
    }
    cout << "]";
}

int Inventory::size() {
    return numItems;
}

int Inventory::capacity() {
    return maxNumItems;
}

bool Inventory::insert(Item *item) {
    if (numItems >= maxNumItems) {
        return false;
    }
    
    if (item == NULL) {
        return false;
    }
    
    for (int i = 0; i < maxNumItems; i++) {
        if (slots[i] == NULL) {
            slots[i] = item;
            numItems++;
            break;
        }
    }
    
    return true;
}

Item *Inventory::getItem(int index) {
    if (index < 0 || index >= maxNumItems) {
        return NULL;
    }
    return slots[index];
}

bool Inventory::setItem(int index, Item *item) {
    if (index < 0 || index >= maxNumItems) {
        return false;
    }

    if (slots[index] != NULL) {
        return false;
    }
    
    slots[index] = item;
    numItems++;
    
    return true;
}

Item *Inventory::removeItem(int index) {
    if (index < 0 || index >= maxNumItems) {
        return NULL;
    }
    
    if (slots[index] == NULL) {
        return NULL;
    }
    
    Item *itemToReturn = slots[index];
    slots[index] = NULL;
    numItems--;
    
    return itemToReturn;
}

bool Inventory::useItem(int index, Unit *unit) {
    if (index < 0 || index >= maxNumItems) {
        return false;
    }
    
    if (slots[index] == NULL) {
        return false;
    }
    
    if (!slots[index]->use(unit)) { // failed to use the item.
        return false;
    }
    
    // succeeded to use the item.

    // check if the item has been disposed
    if (slots[index]->hasBeenDisposed()) {
        delete slots[index];
        slots[index] = NULL;
        numItems--;
    }

    // check if the item has been equipped
    else if (slots[index]->hasBeenEquipped()) {
        slots[index] = NULL;
        numItems--;
    }

    return true;
}

bool Inventory::isFull() {
    return numItems >= maxNumItems;
}

bool Inventory::hasKey() {
    for (int i = 0; i < maxNumItems; i++) {
        if (slots[i] != NULL && slots[i]->isKeyType()) {
            return true;
        }
    }
    return false;
}

void Inventory::save(ostream &out) {
    out << "#---------------------------------------- class Inventory" << endl;
    
    out << "# maxNumItems" << endl;
    out << maxNumItems << endl;
    out << "# numItems" << endl;
    out << numItems << endl;
    
    out << "#================================================== slots[] begin" << endl;
    for (int i = 0; i < maxNumItems; i++) {
        out << "#================================================== slots[" << i << "]" << endl;
        if (slots[i] == NULL) {
            out << TOMB_STONE_NON_EXIST << endl;
        }
        else {
            out << TOMB_STONE_EXIST << endl;
            
            out << "# Item class type" << endl;
            out << slots[i]->getItemID() << endl;
            
            slots[i]->save(out);
        }

    }
    out << "#================================================== slots[] end" << endl;
}

void Inventory::load(istream &in) {
    char buf[MAX_LEN_BUF];

    destroyInventory(); // clear this object.

    //out << "#---------------------------------------- class Inventory" << endl;
    in.getline(buf, MAX_LEN_BUF); // skip a line
    
    //out << "# maxNumItems" << endl;
    in.getline(buf, MAX_LEN_BUF); // skip a line
    //out << maxNumItems << endl;
    in >> maxNumItems;
    in.get(); // skip a char(new line = '\n')
    
    initInventory(maxNumItems); // create new array to hold items.
    
    //out << "# numItems" << endl;
    in.getline(buf, MAX_LEN_BUF); // skip a line
    //out << numItems << endl;
    in >> numItems;
    in.get(); // skip a char(new line = '\n')

    //out << "#================================================== slots[] begin" << endl;
    in.getline(buf, MAX_LEN_BUF); // skip a line
    for (int i = 0; i < maxNumItems; i++) {
        //out << "#================================================== slots[" << i << "]" << endl;
        in.getline(buf, MAX_LEN_BUF); // skip a line

        //out << TOMB_STONE_EXIST << endl;
        string tombStone = "";
        in >> tombStone;
        in.get(); // skip a char(new line = '\n')
        
        if (tombStone == TOMB_STONE_EXIST) {
            //out << "# Item class type" << endl;
            in.getline(buf, MAX_LEN_BUF); // skip a line

            //out << slots[i]->getItemID() << endl;
            string itemID = "";
            in >> itemID;
            in.get(); // skip a char(new line = '\n')
            
            slots[i] = createItemByID(itemID); // create an object using factory.
            
            //slots[i]->save(out);
            slots[i]->load(in);
        }
        else {
            slots[i] = NULL;
        }
        
    }
    //out << "#================================================== slots[] end" << endl;
    in.getline(buf, MAX_LEN_BUF); // skip a line
}
