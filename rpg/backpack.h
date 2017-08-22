#ifndef BACKPACK_H
#define BACKPACK_H

class Item;
class Unit;

template <class T>
class Backpack {
private:
    T *items;
    int maxNumItems;
    int numItems;
	
	// Iterator interface
	int currentIndexForIterator;
    
    void initBackpack(int maxNumItems); // used by Backpack()/load()
    void destroyBackpack(); // used by Backpack()/load()
public:
    Backpack();
    Backpack(int maxNumItems);
    
    virtual ~Backpack();
    
    bool insert(T item);
    
    T getItem(int index);
    T removeItem(int index);
    
    bool isFull();
    
    int size();

    virtual void save(ostream &out);
    virtual void load(istream &in);
	
	// Iterator interface
	bool hasMore();
	T next();

	void resetIterator();
};

#include "backpack.cpp"

#endif
