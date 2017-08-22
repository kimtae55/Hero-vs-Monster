#ifndef MERCHANT_H
#define MERCHANT_H

class Unit;
#include "backpack.h"

class Merchant : public Unit {
private:
    Backpack<Item *> *itemsToSell;
    Backpack<Item *> *itemsBought;
	
	void initItemsToSell();
	
    void sell(Unit *otherUnit);
    void buy(Unit *otherUnit);
public:
    Merchant();
    Merchant(char shape, int row, int col);

    virtual ~Merchant();
    
    virtual string getUnitID();
    
    bool isMerchant();

    virtual void move(int dir);
    virtual bool isMovable();
    
    virtual void interact(Unit *otherUnit); // buy & sell
    
    bool addItemToSell(Item *item); // class Merchant only and used by Board::initUnits()

    virtual void save(ostream &out);
    virtual void load(istream &in);
};

#endif
