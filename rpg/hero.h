#ifndef HERO_H
#define HERO_H

class Unit;
class Inventory;
#include "backpack.h"
class Paperdoll;

class Hero : public Unit {
private:
    Inventory *slots;
	Backpack<Item *> *backpack;
    Paperdoll *paperdoll;
	bool escaped;
	
	void destroyHero();
public:
    Hero();
    
    virtual ~Hero();
	
    virtual string getUnitID();

    virtual void printStat();
    virtual void printItemsToSell();
    virtual Item *getItemsToSell(int itemNum); // itemNum begins from 1
    
    virtual void move(int dir);
    
    virtual bool isHero();
    
    virtual void useItem(int indexSlot);
    
    virtual bool equip(Item *item);
    virtual void unequip(int bodyPartID);
    
    virtual bool canOpenLockedChest();
    
    virtual bool canReceiveAnItem();
    // pre-condition: canReceiveAnItem() == true
    virtual void receiveAnItem(Item *item);

	bool hasEscaped();
	
	void moveItemSlotsToBackpack(int indexSlot);
	void moveItemBackpackToSlots(int indexBackpack);
	
    virtual void save(ostream &out);
    virtual void load(istream &in);
};


#endif
