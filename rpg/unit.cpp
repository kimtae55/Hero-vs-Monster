#include <iostream>
#include <string>
#include <stdlib.h>     /* srand, rand */

using namespace std;

#include "main.h"
#include "unit.h"
#include "monster.h"
#include "hero.h"
#include "merchant.h"

Unit::Unit() {
    shape = DEFAULT_UNIT_SHAPE;
    
    row = -1;
    col = -1;
    
    maxHp = DEFAULT_UNIT_MAX_HP;
    hp = DEFAULT_UNIT_HP;
    maxMp = DEFAULT_UNIT_MAX_MP;
    mp = DEFAULT_UNIT_MP;
    gold = DEFAULT_UNIT_GOLD;
	currentMaxExp = DEFAULT_UNIT_MAX_EXP;
    exp = DEFAULT_UNIT_EXP;
    
    atk = DEFAULT_UNIT_ATK;
    def = DEFAULT_UNIT_DEF;
	
	level = DEFAULT_UNIT_LEVEL;
	
    freezed = false;
    died = false;
    
    unitEngaged = NULL;
}

Unit::Unit(char shape) {
    this->shape = shape;
    
    row = -1;
    col = -1;
    
    maxHp = DEFAULT_UNIT_MAX_HP;
    hp = DEFAULT_UNIT_HP;
    maxMp = DEFAULT_UNIT_MAX_MP;
    mp = DEFAULT_UNIT_MP;
    gold = DEFAULT_UNIT_GOLD;
	currentMaxExp = DEFAULT_UNIT_MAX_EXP;
    exp = DEFAULT_UNIT_EXP;
    
    atk = DEFAULT_UNIT_ATK;
    def = DEFAULT_UNIT_DEF;
	
	level = DEFAULT_UNIT_LEVEL;

    freezed = false;
    died = false;

    unitEngaged = NULL;
}

Unit::Unit(char shape, int row, int col, int maxHp, int hp, int maxMp, int mp, int gold, int currentMaxExp, int exp, int atk, int def, int level) {
    this->shape = shape;
    
    this->row = row;
    this->col = col;

    this->maxHp = maxHp;
    this->hp = hp;
    this->maxMp = maxMp;
    this->mp = mp;
    this->gold = gold;
	this->currentMaxExp = currentMaxExp;
    this->exp = exp;
    
    this->atk = atk;
    this->def = def;

	this->level = level;

    freezed = false;
    died = false;

    unitEngaged = NULL;
}

Unit::~Unit() {
}

void Unit::print() {
    cout << shape;
}

char Unit::getShape() {
	return shape;
}

void Unit::printStat() {
    cout << shape << "(" << level << "): hp(" << hp << "/" << maxHp << ") mp(" << mp << "/" << maxMp
         << ") gold(" << gold << ") exp(" << exp << "/" << currentMaxExp << ") atk(" << atk << ") def(" << def << ")";
    if (unitEngaged != NULL) {
        cout << " vs ";
        cout << unitEngaged->shape << ": hp(" << unitEngaged->hp<< ") mp(" << unitEngaged->mp
             << ") gold(" << unitEngaged->gold << ") exp(" << unitEngaged->exp << ")";
        
        if (unitEngaged->isDead()) {
            unitEngaged = NULL;
        }
    }
}

void Unit::printItemsToSell() {
    // N/A
}

Item *Unit::getItemsToSell(int itemNum) {
    // N/A
    return NULL;
}

int Unit::getRow() {
    return row;
}

void Unit::setRow(int row) {
    this->row = row;
}

int Unit::getCol() {
    return col;
}

void Unit::setCol(int col) {
    this->col = col;
}

int Unit::getMaxHp() {
    return maxHp;
}

void Unit::setMaxHp(int maxHp) {
    this->maxHp = maxHp;
}

int Unit::getHp() {
    return hp;
}

bool Unit::incHp(int hpToInc) {
    // to avoid to use a potion when this unit's hp and mp are full.
    if (hp >= maxHp) {
        return false;
    }
    
    if (hp + hpToInc >= maxHp) {
        hp = maxHp;
    }
    else {
        hp += hpToInc;
    }
    return true;
}

void Unit::decHp(int hpToDec) { // hpToDec is moster's atk
	// finalDamage = hpToDec - (random value of 50% - 100% of def)
	int finalDamage = hpToDec - (def - (rand() % ((int)(def * 0.5))));
	if (finalDamage < 0) {
		finalDamage = 0;
	}
	
    if (hp - finalDamage <= 0) {
        hp = 0;
        died = true;
    }
    else {
        hp -= finalDamage;
    }
}

int Unit::getMaxMp() {
    return maxMp;
}

void Unit::setMaxMp(int maxMp) {
    this->maxMp = maxMp;
}

int Unit::getMp() {
    return mp;
}

bool Unit::incMp(int mpToInc) {
    // to avoid to use a potion when this unit's hp and mp are full.
    if (mp >= maxMp) {
        return false;
    }
    
    if (mp + mpToInc >= maxMp) {
        mp = maxMp;
    }
    else {
        mp += mpToInc;
    }
    return true;
}

void Unit::decMp(int mpToDec) {
    if (mp - mpToDec <= 0) {
        mp = 0;
    }
    else {
        mp -= mpToDec;
    }
}

int Unit::getGold() {
    return gold;
}

void Unit::incGold(int goldToInc) {
    gold += goldToInc;
}

void Unit::decGold(int goldToDec) {
    if (gold - goldToDec <= 0) {
        gold = 0;
    }
    else {
        gold -= goldToDec;
    }
}

int Unit::getExp() {
    return exp;
}

void Unit::incExp(int expToInc) {
	while (exp + expToInc >= currentMaxExp) {
		exp = (exp + expToInc) - currentMaxExp;
		incLevel(1);
		expToInc = 0;
	}
	exp += expToInc;
}

int Unit::getAtk() {
    return atk;
}

void Unit::incAtk(int atkToInc) {
    atk += atkToInc;
}

void Unit::decAtk(int atkToDec) {
    atk -= atkToDec;
}

int Unit::getDef() {
    return def;
}

void Unit::incDef(int defToInc) {
    def += defToInc;
}

void Unit::decDef(int defToDec) {
    def -= defToDec;
}

int Unit::getLevel() {
	return level;
}

/*
 #define INTERVAL_EXP 1000
 #define DEFAULT_UNIT_MAX_EXP INTERVAL_EXP
 #define INTERVAL_MAX_HP
 #define INTERVAL_MAX_MP
 #define INTERVAL_GOLD_REWARD
 #define INTERVAL_ATK
 #define INTERVAL_DEF
*/
void Unit::incLevel(int levelToInc) {
	for (int i = 0; i < levelToInc; i++) {
		level++;
		currentMaxExp += level * INTERVAL_EXP;
		maxHp += level * INTERVAL_MAX_HP;
		hp = maxHp;
		maxMp += level * INTERVAL_MAX_MP;
		mp = maxMp;
		gold += level * INTERVAL_GOLD_REWARD;
		atk += level * INTERVAL_ATK;
		def += level * INTERVAL_DEF;
	}
}

void Unit::freeze() {
    freezed = true;
}

void Unit::unfreeze() {
    freezed = false;
}

void Unit::revive() {
    maxHp = DEFAULT_UNIT_MAX_HP;
    hp = DEFAULT_UNIT_HP;
    maxMp = DEFAULT_UNIT_MAX_MP;
    mp = DEFAULT_UNIT_MP;
    gold = DEFAULT_UNIT_GOLD;
    currentMaxExp = DEFAULT_UNIT_MAX_EXP;
    exp = DEFAULT_UNIT_EXP;
    
    atk = DEFAULT_UNIT_ATK;
    def = DEFAULT_UNIT_DEF;
    
    level = DEFAULT_UNIT_LEVEL;
    
    freezed = false;
    died = false;
    
    unitEngaged = NULL;
    
    died = false;
}

bool Unit::isDead() {
    return died;
}

bool Unit::isMovable() {
    // N/A
    return true; // merchant등 안 움직이는 애들만 false를 return하게 한다.
}

void Unit::useItem(int indexSlot) {
    // N/A
}

bool Unit::isMonster() {
    return false;
}

bool Unit::isBoss() {
    return false;
}

bool Unit::isHero() {
    return false;
}

bool Unit::isMerchant() {
    return false;
}

void Unit::interact(Unit *otherUnit) {
    // N/A
}

bool Unit::equip(Item *item) {
    // N/A
    return false;
}

void Unit::unequip(int partNo) {
    // N/A
}

bool Unit::canOpenLockedChest() {
    return false;
}

bool Unit::canReceiveAnItem() {
    // N/A
    return false;
}

// pre-condition: canReceiveAnItem() == true
void Unit::receiveAnItem(Item *item) {
    // N/A
}

void Unit::save(ostream &out) {
    out << "#---------------------------------------- class Unit" << endl;
    
    out << "# shape" << endl;
    out << shape << endl;
    out << "# row" << endl;
    out << row << endl;
    out << "# col" << endl;
    out << col << endl;
    out << "# maxHp" << endl;
    out << maxHp << endl;
    out << "# hp" << endl;
    out << hp << endl;
    out << "# maxMp" << endl;
    out << maxMp << endl;
    out << "# mp" << endl;
    out << mp << endl;
    out << "# gold" << endl;
    out << gold << endl;
	out << "# currentMaxExp" << endl;
	out << currentMaxExp << endl;
    out << "# exp" << endl;
    out << exp << endl;
    out << "# atk" << endl;
    out << atk << endl;
    out << "# def" << endl;
    out << def << endl;
	out << "# level" << endl;
	out << level << endl;
	
    out << "# freezed" << endl;
    out << freezed << endl;
    out << "# died" << endl;
    out << died << endl;

    out << "# unitEngaged" << endl;
    out << "# N/A" << endl; // We don't save unitEngaged.
}

void Unit::load(istream &in) {
    char buf[MAX_LEN_BUF];
    
    //out << "#---------------------------------------- class Unit" << endl;
    in.getline(buf, MAX_LEN_BUF); // skip a line
    
    //out << "# shape" << endl;
    in.getline(buf, MAX_LEN_BUF); // skip a line
    //out << shape << endl;
    in >> shape;
    in.get(); // skip a char(new line = '\n')
    
    //out << "# row" << endl;
    in.getline(buf, MAX_LEN_BUF); // skip a line
    //out << row << endl;
    in >> row;
    in.get(); // skip a char(new line = '\n')

    //out << "# col" << endl;
    in.getline(buf, MAX_LEN_BUF); // skip a line
    //out << col << endl;
    in >> col;
    in.get(); // skip a char(new line = '\n')
    
    //out << "# maxHp" << endl;
    in.getline(buf, MAX_LEN_BUF); // skip a line
    //out << maxHp << endl;
    in >> maxHp;
    in.get(); // skip a char(new line = '\n')
    
    //out << "# hp" << endl;
    in.getline(buf, MAX_LEN_BUF); // skip a line
    //out << hp << endl;
    in >> hp;
    in.get(); // skip a char(new line = '\n')
    
    //out << "# maxMp" << endl;
    in.getline(buf, MAX_LEN_BUF); // skip a line
    //out << maxMp << endl;
    in >> maxMp;
    in.get(); // skip a char(new line = '\n')
    
    //out << "# mp" << endl;
    in.getline(buf, MAX_LEN_BUF); // skip a line
    //out << mp << endl;
    in >> mp;
    in.get(); // skip a char(new line = '\n')
    
    //out << "# gold" << endl;
    in.getline(buf, MAX_LEN_BUF); // skip a line
    //out << gold << endl;
    in >> gold;
    in.get(); // skip a char(new line = '\n')
    
    //out << "# exp" << endl;
    in.getline(buf, MAX_LEN_BUF); // skip a line
    //out << exp << endl;
    in >> exp;
    in.get(); // skip a char(new line = '\n')
	
	//out << "# currentMaxExp" << endl;
	in.getline(buf, MAX_LEN_BUF); // skip a line
	//out << currentMaxExp << endl;
	in >> currentMaxExp;
	in.get(); // skip a char(new line = '\n')
	
    //out << "# atk" << endl;
    in.getline(buf, MAX_LEN_BUF); // skip a line
    //out << atk << endl;
    in >> atk;
    in.get(); // skip a char(new line = '\n')
    
    //out << "# def" << endl;
    in.getline(buf, MAX_LEN_BUF); // skip a line
    //out << def << endl;
    in >> def;
    in.get(); // skip a char(new line = '\n')

	//out << "# level" << endl;
	in.getline(buf, MAX_LEN_BUF); // skip a line
	//out << level << endl;
	in >> level;
	in.get(); // skip a char(new line = '\n')

    //out << "# freezed" << endl;
    in.getline(buf, MAX_LEN_BUF); // skip a line
    //out << freezed << endl;
    in >> freezed;
    in.get(); // skip a char(new line = '\n')
    
    //out << "# died" << endl;
    in.getline(buf, MAX_LEN_BUF); // skip a line
    //out << died << endl;
    in >> died;
    in.get(); // skip a char(new line = '\n')
    
    //out << "# unitEngaged" << endl;
    in.getline(buf, MAX_LEN_BUF); // skip a line
    //out << "# N/A" << endl;
    in.getline(buf, MAX_LEN_BUF); // skip a line
}

Unit *createUnitByID(string unitID) {
    if (unitID == UNIT_ID_HERO) {
        return new Hero();
    }
    else if (unitID == UNIT_ID_MONSTER) {
        return new Monster();
    }
    else if (unitID == UNIT_ID_BOSS) {
        return new Boss();
    }
	else if (unitID == UNIT_ID_MERCHANT) {
		return new Merchant();
	}
    else {
        cout << "error: no such unit exist! unitID = \"" << unitID << "\"" << endl;
        exit(1);
    }
    
    return NULL;
}
