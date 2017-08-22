#ifndef UNIT_H
#define UNIT_H

class Item;
class Board;

class Unit {
protected:
    char shape;
    
    int row;
    int col;
    
    int maxHp;
    int hp;
    int maxMp;
    int mp;
    int gold;
	int currentMaxExp;
    int exp;

    int atk;
    int def;

	int level;

	bool freezed;
    bool died;
    
    Unit *unitEngaged;
    
public:
    Unit();
    Unit(char shape);
    Unit(char shape, int row, int col, int maxHp, int hp, int maxMp, int mp,
		 int gold, int currentMaxExp, int exp, int atk, int def, int level);
    
    virtual ~Unit();
    
    virtual string getUnitID() = 0;
    
    void print();
    virtual void printStat();
    virtual void printItemsToSell();
    virtual Item *getItemsToSell(int itemNum);
	
	char getShape();
	
    int getRow();
    void setRow(int row);
    int getCol();
    void setCol(int col);

    int getMaxHp();
    void setMaxHp(int maxHp);

    int getHp();
    bool incHp(int hpToInc);
    void decHp(int hpToDec);

    int getMaxMp();
    void setMaxMp(int maxMp);

    int getMp();
    bool incMp(int mpToInc);
    void decMp(int mpToDec);

    int getGold();
    void incGold(int goldToInc);
    void decGold(int goldToDec);

    int getExp();
    void incExp(int expToInc);

    int getAtk();
    void incAtk(int atkToInc);
    void decAtk(int atkToDec);

    int getDef();
    void incDef(int defToInc);
    void decDef(int defToDec);

	int getLevel();
	void incLevel(int levelToInc);

    void freeze();
    void unfreeze();

    void revive();
    bool isDead();

    virtual void move(int dir) = 0;
    virtual bool isMovable();
    
    virtual void useItem(int indexSlot);
    
    virtual bool isMonster();
    virtual bool isBoss();
    virtual bool isHero();
    virtual bool isMerchant();
    
    virtual void interact(Unit *otherUnit);
    
    virtual bool equip(Item *item);
    virtual void unequip(int partNo);
    
    virtual bool canOpenLockedChest();

    virtual bool canReceiveAnItem();
    // pre-condition: canReceiveAnItem() == true
    virtual void receiveAnItem(Item *item);
    
    virtual void save(ostream &out);
    virtual void load(istream &in);
};

Unit *createUnitByID(string unitID);

#endif
