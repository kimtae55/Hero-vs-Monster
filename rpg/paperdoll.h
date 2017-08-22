#ifndef PAPERDOLL_H
#define PAPERDOLL_H

class Armor;
class Weapon;

class Paperdoll {
private:
    Armor *head;
    Armor *chest;
    Armor *back;
    Armor *hands;
    Armor *legs;
    Armor *feet;
    
    Weapon *rightHand;
    Weapon *leftHand;
    
public:
    Paperdoll();
    
    virtual ~Paperdoll();
    
    void print();
    
    bool equip(Item *item, Unit *unit);
    Item *unequip(int bodyPartID, Unit *unit);
    
    virtual void save(ostream &out);
    virtual void load(istream &in);
};

#endif
