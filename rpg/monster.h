#ifndef MONSTER_H
#define MONSTER_H

class Unit;
class AsNode;

class Monster : public Unit {
private:
public:
    Monster();
	Monster(char shape, int row, int col, int maxHp, int hp, int maxMp, int mp,
			int gold, int currentMaxExp, int exp, int atk, int def, int level);

    virtual ~Monster();
    
    virtual string getUnitID();

    virtual void move(int dir);
    virtual bool isMonster();
    virtual void interact(Unit *otherUnit);
    
    virtual void save(ostream &out);
    virtual void load(istream &in);
};

class Boss : public Monster {
private:
    int normalRange;
    int range; // to detect any human in the range
    
    bool pathExist;
    int pathSize;
    AsNode **path;
    int currentIndexOfNodeInPath;
    
    void resetPath();
public:
    Boss();
    Boss(char shape, int range);
    
    virtual ~Boss();

    // ver0053: 아래의 두 펑션은 BossRushEvent::trigger()에서 사용.
    void setRange(int range); // range를 board전체로 확대할 때 사용.
    void resetRange(); // normal(original) range로 되돌린다.

    virtual string getUnitID();

    virtual bool isBoss();
    
    bool attackHeroWhenAdjacent();
    virtual void move(int dir);
    
    virtual void save(ostream &out);
    virtual void load(istream &in);
};

#endif
