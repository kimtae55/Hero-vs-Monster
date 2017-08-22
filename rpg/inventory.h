#ifndef INVENTORY_H
#define INVENTORY_H

class Item;
class Unit;

class Inventory {
private:
    Item **slots;
    int maxNumItems;
    int numItems;
    
    void initInventory(int maxNumItems); // used by Inventory()/load()
    void destroyInventory(); // used by Inventory()/load()
public:
    Inventory();
    
    virtual ~Inventory();
    
    void print();
    
    int size();
    int capacity();
    
    bool insert(Item *item);
    
    Item *getItem(int index);
    bool setItem(int index, Item *item);
    
    Item *removeItem(int index); // index begins from 0
    bool useItem(int index, Unit *unit);
    
    bool isFull();
    
    bool hasKey();
    
    virtual void save(ostream &out);
    virtual void load(istream &in);
};

#endif
