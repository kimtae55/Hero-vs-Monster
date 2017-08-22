#ifndef ITEM_H
#define ITEM_H

class Unit;

//==============================================================================
// 1st Generation
//==============================================================================

class Item {
protected:
    char shape;
    string name;
    int price;
    
    bool disposed;
    bool equipped;
    
public:
    Item();
    Item(Item &other);
    Item(char shape, string name, int price);
    
    virtual ~Item();
    
    void print();
    virtual void printFullSpec();
	
    virtual string getItemID() = 0;
    
    char getShape();
	string getName();
    int getPrice();

    bool hasBeenDisposed();
    bool hasBeenEquipped();
    
    virtual bool use(Unit *unit) = 0;
    
    virtual bool isConsumable();
    virtual bool isEquippable();
    virtual bool isKeyType();
    
    virtual void save(ostream &out);
    virtual void load(istream &in);
};

Item *createItemByID(string itemID);
Item *createItemByID(string itemID, Item *itemToCopy);

ostream &operator<<(ostream &out, Item *item);
istream &operator>>(istream &in, Item *&item);

//==============================================================================
// 2nd Generation
//==============================================================================

class ConsumableItem : public Item {
private:
public:
    ConsumableItem();
    ConsumableItem(ConsumableItem &other);
    ConsumableItem(char shape, string name, int price);
    
    virtual ~ConsumableItem();

    bool isConsumable();

    virtual void save(ostream &out);
    virtual void load(istream &in);
};

class EquippableItem : public Item {
private:
public:
    EquippableItem();
    EquippableItem(EquippableItem &other);
    EquippableItem(char shape, string name, int price);

    virtual ~EquippableItem();
    
    bool isEquippable();

    virtual bool isArmor();
    virtual bool isWeapon();
    
    virtual void save(ostream &out);
    virtual void load(istream &in);
};

//==============================================================================
// 3rd Generation
//==============================================================================

class Potion : public ConsumableItem {
private:
    int hp;
    int mp;
public:
    Potion();
    Potion(Potion &other);
    Potion(char shape, string name, int price, int hp, int mp);

    virtual ~Potion();

    virtual void printFullSpec();
    
    virtual string getItemID();

    int getHp();
    int getMp();

    virtual bool use(Unit *unit);
    
    virtual void save(ostream &out);
    virtual void load(istream &in);
};

class Key : public ConsumableItem {
private:
    int durability;
public:
    Key();
    Key(Key &other);
    Key(char shape, string name, int price, int durability);
    
    virtual ~Key();

    virtual void printFullSpec();

    virtual string getItemID();

    int getDurability();
    
    virtual bool use(Unit *unit);
    
    virtual bool isKeyType();
    
    virtual void save(ostream &out);
    virtual void load(istream &in);
};


class Armor : public EquippableItem {
private:
    int def;
    int durability;
    int bodyPartID;
public:
    Armor();
    Armor(Armor &other);
    Armor(char shape, string name, int price, int def, int durability, int bodyPartID);
    
    virtual ~Armor();
    
    virtual void printFullSpec();

    virtual string getItemID();

    int getDef();
    int getDurability();
    int getBodyPartID();
    
    virtual bool use(Unit *unit);
    
    bool isArmor();
    
    virtual void save(ostream &out);
    virtual void load(istream &in);
};

Armor *createRandomArmor();
Armor *createRandomArmorByShape(char shapeToCreate);

class Weapon : public EquippableItem {
private:
    int atk;
    int durability;
    int numHands;
public:
    Weapon();
    Weapon(Weapon &other);
    Weapon(char shape, string name, int price, int atk, int durability, int numHands);
    
    virtual ~Weapon();
    
    virtual void printFullSpec();

    virtual string getItemID();

    int getAtk();
    int getDurability();
    int getNumHands();

    virtual bool use(Unit *unit);

    bool isWeapon();
    
    virtual void save(ostream &out);
    virtual void load(istream &in);
};

Weapon *createRandomWeapon();
Weapon *createRandomWeaponByShape(char shapeToCreate);

#endif
