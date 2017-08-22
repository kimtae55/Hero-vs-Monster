#include <iostream>
#include <string>

using namespace std;

#include "main.h"
#include "item.h"
//#include "backpack.h"

template <class T>
Backpack<T>::Backpack() {
    initBackpack(DEFAULT_BACKPACK_MAX_NUM_ITEMS);
}

template <class T>
Backpack<T>::Backpack(int maxNumItems) {
    initBackpack(maxNumItems);
}

template <class T>
void Backpack<T>::initBackpack(int maxNumItems) {
    this->maxNumItems = maxNumItems;
    numItems = 0;
    
    items = new T [maxNumItems];
    
    for (int i = 0; i < maxNumItems; i++) {
        items[i] = NULL;
    }
	
	currentIndexForIterator = 0;
}

template <class T>
Backpack<T>::~Backpack() {
    destroyBackpack();
}

template <class T>
void Backpack<T>::destroyBackpack() {
    if (items != NULL) {
        //for (int i = 0; i < numItems; i++) {
        //    delete items[i];
        //}
        
        delete [] items;
        items = NULL;
    }
	
	currentIndexForIterator = 0;
}

template <class T>
bool Backpack<T>::insert(T item) { // append, push
    if (numItems >= maxNumItems) { // is full?
        return false;
    }
    
    items[numItems] = item;
    numItems++;
    
    return true;
}

template <class T>
T Backpack<T>::getItem(int index) {
    if (index < 0 || index >= maxNumItems) {
        return NULL;
    }
    return items[index];
}

template <class T>
T Backpack<T>::removeItem(int index) {
    if (index < 0 || index >= maxNumItems) {
        return NULL;
    }
    
    if (items[index] == NULL) {
        return NULL;
    }
    
    T itemToReturn = items[index];
    
    // items[index+1..numItems-1] will be shifted to left by 1
    for (int i = index+1; i <= numItems-1; i++) {
        items[i-1] = items[i];
    }
    numItems--;
    
    return itemToReturn;
}

template <class T>
bool Backpack<T>::isFull() {
    return numItems >= maxNumItems;
}

template <class T>
int Backpack<T>::size() {
    return numItems;
}

template <class T>
void Backpack<T>::save(ostream &out) {
    out << "#---------------------------------------- class Backpack" << endl;
    
    out << "# maxNumItems" << endl;
    out << maxNumItems << endl;
    out << "# numItems" << endl;
    out << numItems << endl;
    
    out << "#================================================== items[] begin" << endl;
    for (int i = 0; i < maxNumItems; i++) {
        out << "#================================================== items[" << i << "]" << endl;
        if (items[i] == NULL) {
            out << TOMB_STONE_NON_EXIST << endl;
        }
        else {
            out << TOMB_STONE_EXIST << endl;
            
            //out << "# Item class type" << endl;
            //out << items[i]->getItemID() << endl;
            //items[i]->save(out);
			
			out << items[i];
        }

    }
    out << "#================================================== items[] end" << endl;
}

template <class T>
void Backpack<T>::load(istream &in) {
    char buf[MAX_LEN_BUF];

    destroyBackpack(); // clear this object.

    //out << "#---------------------------------------- class Backpack" << endl;
    in.getline(buf, MAX_LEN_BUF); // skip a line
    
    //out << "# maxNumItems" << endl;
    in.getline(buf, MAX_LEN_BUF); // skip a line
    //out << maxNumItems << endl;
    in >> maxNumItems;
    in.get(); // skip a char(new line = '\n')
    
    initBackpack(maxNumItems); // create new array to hold items.
    
    //out << "# numItems" << endl;
    in.getline(buf, MAX_LEN_BUF); // skip a line
    //out << numItems << endl;
    in >> numItems;
    in.get(); // skip a char(new line = '\n')

    //out << "#================================================== items[] begin" << endl;
    in.getline(buf, MAX_LEN_BUF); // skip a line
    for (int i = 0; i < maxNumItems; i++) {
        //out << "#================================================== items[" << i << "]" << endl;
        in.getline(buf, MAX_LEN_BUF); // skip a line

        //out << TOMB_STONE_EXIST << endl;
        string tombStone = "";
        in >> tombStone;
        in.get(); // skip a char(new line = '\n')
        
        if (tombStone == TOMB_STONE_EXIST) {
            //out << "# Item class type" << endl;
            in.getline(buf, MAX_LEN_BUF); // skip a line

            //string itemID = "";
            //in >> itemID;
            //in.get(); // skip a char(new line = '\n')
            //items[i] = createItemByID(itemID); // create an object using factory.
            //items[i]->load(in);
			
			in >> items[i];
        }
        else {
            items[i] = NULL;
        }
        
    }
    //out << "#================================================== items[] end" << endl;
    in.getline(buf, MAX_LEN_BUF); // skip a line
}

//------------------------------------------------------------------------------
// Iterator interface
//------------------------------------------------------------------------------

template <class T>
bool Backpack<T>::hasMore() {
	return currentIndexForIterator < numItems;
}

template <class T>
T Backpack<T>::next() {
	return items[currentIndexForIterator++];
}

template <class T>
void Backpack<T>::resetIterator() {
	currentIndexForIterator = 0;
}
