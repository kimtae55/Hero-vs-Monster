#include <iostream>

using namespace std;

#include "main.h"
#include "as_graph.h"
#include "unit.h"
#include "board.h"
#include "monster.h"

#include "sim.h"
extern Sim *sim; // main.cpp에서 global variable로 만들어진 sim을 쓰겠다는 얘기.

extern int curAsNodeID; // debugging purpose
extern int numRemovedAsNode; // debugging purpose

//------------------------------------------------------------------------------
// class Monster
//------------------------------------------------------------------------------

Monster::Monster() : Unit(DEFAULT_MONSTER_SHAPE, -1, -1, DEFAULT_MONSTER_MAX_HP, DEFAULT_MONSTER_HP,
                    DEFAULT_MONSTER_MAX_MP, DEFAULT_MONSTER_MP, DEFAULT_MONSTER_GOLD,
					DEFAULT_MONSTER_MAX_EXP, DEFAULT_MONSTER_EXP,
                    DEFAULT_MONSTER_ATK, DEFAULT_MONSTER_DEF, DEFAULT_MONSTER_LEVEL) {
}

Monster::Monster(char shape, int row, int col, int maxHp, int hp, int maxMp, int mp,
				 int gold, int currentMaxExp, int exp, int atk, int def, int level)
    : Unit(shape, row, col, maxHp, hp, maxMp, mp, gold, currentMaxExp, exp, atk, def, level) {
}

Monster::~Monster() {
}

string Monster::getUnitID() {
    return string(UNIT_ID_MONSTER);
}

void Monster::move(int dir) {
    if (freezed) {
        return;
    }
    
    // decide the next position.
    int nextRow = -1;
    int nextCol = -1;
    if (dir == DIR_NORTH) {
        nextRow = row-1;
        nextCol = col;
    }
    else if (dir == DIR_EAST) {
        nextRow = row;
        nextCol = col+1;
    }
    else if (dir == DIR_SOUTH) {
        nextRow = row+1;
        nextCol = col;
    }
    else if (dir == DIR_WEST) {
        nextRow = row;
        nextCol = col-1;
    }
    
    // move the monster when the next position is empty(no unit && no prop).
    if (sim->board->validate(nextRow, nextCol) &&
        sim->board->getProp(nextRow, nextCol) == NULL &&
        sim->board->getUnit(nextRow, nextCol) == NULL) {
        sim->board->setUnit(row, col, NULL);
        sim->board->setUnit(nextRow, nextCol, this);
        // now row and col have new coord.
    }
}

bool Monster::isMonster() {
    return true;
}

// battle
void Monster::interact(Unit *otherUnit) {
    if (otherUnit == NULL) {
        return;
    }
    
    decHp(otherUnit->getAtk());
    if (!isDead()) { // check whether I(mon) died or not.
        otherUnit->decHp(atk);
    }
    else { // when I(mon) died
        // the hero is looting.
        otherUnit->incGold(gold);
        otherUnit->incExp(exp);
    }
}

void Monster::save(ostream &out) {
    Unit::save(out);
    
    out << "#---------------------------------------- class Monster" << endl;
}

void Monster::load(istream &in) {
    Unit::load(in);
    
    char buf[MAX_LEN_BUF];
    
    //out << "#---------------------------------------- class Monster" << endl;
    in.getline(buf, MAX_LEN_BUF); // skip a line
}

//------------------------------------------------------------------------------
// class Boss
//------------------------------------------------------------------------------

Boss::Boss() : Monster(DEFAULT_BOSS_SHAPE, -1, -1, DEFAULT_BOSS_MAX_HP, DEFAULT_BOSS_HP,
                       DEFAULT_BOSS_MAX_MP, DEFAULT_BOSS_MP, DEFAULT_BOSS_GOLD,
					   DEFAULT_BOSS_MAX_EXP, DEFAULT_BOSS_EXP,
                       DEFAULT_BOSS_ATK, DEFAULT_BOSS_DEF, DEFAULT_BOSS_LEVEL) {
    normalRange = DEFAULT_BOSS_RANGE;
    range = DEFAULT_BOSS_RANGE;
    
    resetPath();
}

Boss::Boss(char shape, int range)
             : Monster(shape, -1, -1, DEFAULT_BOSS_MAX_HP, DEFAULT_BOSS_HP,
                       DEFAULT_BOSS_MAX_MP, DEFAULT_BOSS_MP, DEFAULT_BOSS_GOLD,
					   DEFAULT_BOSS_MAX_EXP, DEFAULT_BOSS_EXP,
                       DEFAULT_BOSS_ATK, DEFAULT_BOSS_DEF, DEFAULT_BOSS_LEVEL) {
    normalRange = range;
    this->range = range;

    resetPath();
}

Boss::~Boss() {
    if (path != NULL) {
        delete [] path;
    }
}

// ver0053: 아래의 두 펑션은 BossRushEvent::trigger()에서 사용.
void Boss::setRange(int range) { // range를 board전체로 확대할 때 사용.
    this->range = range;
}

void Boss::resetRange() { // normal(original) range로 되돌린다.
    range = normalRange;
}

string Boss::getUnitID() {
    return string(UNIT_ID_BOSS);
}

bool Boss::isBoss() {
    return true;
}

// reset path related variables.
void Boss::resetPath() {
    if (pathSize > 0 && path != NULL) {
        cout << "Boss::resetPath(): final: curAsNodeID=" << curAsNodeID << endl;
        cout << "                          numRemovedAsNode=" << numRemovedAsNode << endl;
        cout << "                          pathSize=" << pathSize << endl;

        // delete all the AsNodes in the path.
        for (int i = 0; i < pathSize; i++) {
            // debugging purpose.
            cout << "Boss::resetPath(): delete path[i]; current->id=" << path[i]->id << endl;

            delete path[i];
        }
        delete [] path;
        extern int curAsNodeID; // debugging purpose
        extern int numRemovedAsNode; // debugging purpose
        cout << "Boss::resetPath(): final: curAsNodeID=" << curAsNodeID << endl;
        cout << "                          numRemovedAsNode=" << numRemovedAsNode << endl;
    }
    
    pathExist = false;
    pathSize = 0;
    path = NULL;
    currentIndexOfNodeInPath = -1;
}

bool Boss::attackHeroWhenAdjacent() {
    cout << "Boss::attackHeroWhenAdjacent()" << endl;
    // check north
    if (sim->board->validate(row-1, col) &&
        sim->board->getUnit(row-1, col) != NULL &&
        sim->board->getUnit(row-1, col)->isHero()) {
        sim->board->getUnit(row-1, col)->decHp(atk); // hit him.
        return true;
    }
    // check east
    else if (sim->board->validate(row, col+1) &&
        sim->board->getUnit(row, col+1) != NULL &&
        sim->board->getUnit(row, col+1)->isHero()) {
        sim->board->getUnit(row, col+1)->decHp(atk); // hit him.
        return true;
    }
    // check south
    else if (sim->board->validate(row+1, col) &&
        sim->board->getUnit(row+1, col) != NULL &&
        sim->board->getUnit(row+1, col)->isHero()) {
        sim->board->getUnit(row+1, col)->decHp(atk); // hit him.
        return true;
    }
    // check west
    else if (sim->board->validate(row, col-1) &&
        sim->board->getUnit(row, col-1) != NULL &&
        sim->board->getUnit(row, col-1)->isHero()) {
        sim->board->getUnit(row, col-1)->decHp(atk); // hit him.
        return true;
    }
    return false;
}

void Boss::move(int dir) {
    // 만약 A* 앨거리듬에 의해 길을 찾아놓았을 경우, 그 길을 우선 따라가게 한다.
    if (pathExist) {
        // hero의 바로 직전까지만 오게 한다.
        if (currentIndexOfNodeInPath >= pathSize-2) {
            resetPath();
        }
        // 아직 갈 길이 남아있을 경우.
        else {
            //cout << "if (pathExist)" << endl;

            //cout << "row=" << row << " col=" << col << endl;
            //cout << "pathSize=" << pathSize
            //     << " currentIndexOfNodeInPath=" << currentIndexOfNodeInPath << endl;
            int nextRow = path[currentIndexOfNodeInPath]->row;
            int nextCol = path[currentIndexOfNodeInPath]->col;
            if (sim->board->getUnit(nextRow, nextCol) == NULL) {
                sim->board->setUnit(row, col, NULL);
                sim->board->setUnit(nextRow, nextCol, this);
                // now row and col have new coord.
                
                currentIndexOfNodeInPath++;
            }
            // 만약 길을 가다가 어떤 unit에 의해 막힌다면, 모두 cancel하고 다시 길을 찾게 한다.
            else {
                // 바로 hero와 adjacent된 tile에(4방향) 다른 unit이 막고 있다는 것은,
                // 현재 찾아낸 only path이다. 따라서, 그 경우만 reset하고 hero 주변에
                // 근접한 8개의 tile중에 random으로 위치를(boundary안에서만) 정하게 한다.
                if (currentIndexOfNodeInPath == pathSize-2 ||
                    !sim->board->getUnit(nextRow, nextCol)->isMovable() ||
                    sim->board->getUnit(nextRow, nextCol)->isBoss()) {
                    resetPath();
                }
                // 그 이외의 경우에는 그냥 기다렸다가 해당 unit이 비켜나면 지나가게 한다.
            }
        }
        
        if (attackHeroWhenAdjacent()) {
            resetPath();
        }
        
        return;
    }
    //cout << "if (!pathExist)" << endl;

    int heroRow = -1;
    int heroCol = -1;
    
    bool found = false;
    
    for (int i = row-range; i <= row+range && !found; i++) {
        for (int j = col-range; j <= col+range && !found; j++) {
            Unit *curUnit = sim->board->getUnit(i, j);
            if (curUnit != NULL && curUnit->isHero()) {
                heroRow = i;
                heroCol = j;
                found = true;
            }
        }
    }
    
    // hero를 찾았다면..    
    if (found) {
        // 만약 그 hero가 바로 adjacent되어 있다면, 한대 때리고 그냥 return.
        if (attackHeroWhenAdjacent()) {
            return;
        }

        AsGraph *asGraph = new AsGraph();
        
        pathSize = 0;
        path = asGraph->findPath(row, col, heroRow, heroCol, &pathSize);
        
        // path가 존재한다면..
        if (path != NULL) {
            pathExist = true;
            currentIndexOfNodeInPath = 1;

            //for (int i = 0; i < pathSize; i++) {
            //    cout << "[" << i << "]: ";
            //    path[i]->print();
            //    cout << endl;
            //}
            
            //exit(1);
            
            int nextRow = path[currentIndexOfNodeInPath]->row;
            int nextCol = path[currentIndexOfNodeInPath]->col;
            if (sim->board->getUnit(nextRow, nextCol) == NULL) {
                sim->board->setUnit(row, col, NULL);
                sim->board->setUnit(nextRow, nextCol, this);
                // now row and col have new coord.
                
                currentIndexOfNodeInPath++;
            }
        }
        // path가 존재하지 않는다면..
        else {
            //cout << "path not found!!" << endl;
            //exit(1);
            // reset path related variables.
            pathExist = false;
            pathSize = 0;
            path = NULL;
            currentIndexOfNodeInPath = -1;
            
            Monster::move(dir);
        }
        
        delete asGraph;
    }
    // hero를 못 찾았다면..
    else {
        Monster::move(dir);
    }
}

/*
void Boss::move(int dir) {
    for (int i = row-range; i <= row+range; i++) {
        for (int j = col-range; j <= col+range; j++) {
            Unit *curUnit = sim->board->getUnit(i, j);
            if (curUnit != NULL && curUnit->isHuman()) {
                // move toward the human
                if (row == curUnit->getRow()) {
                    if (col > curUnit->getCol()) { // on west
                        if (col - curUnit->getCol() == 1) { // when adjacent
                            curUnit->decHp(atk); // hit him.
                        }
                        else { // move to west
                            // move the boss when the next position is empty(no unit).
                            if (sim->board->getUnit(row, col-1) == NULL) {
                                sim->board->setUnit(row, col, NULL);
                                sim->board->setUnit(row, col-1, this);
                                // now row and col have new coord.
                            }
                        }
                    }
                    else { // on east
                        if (curUnit->getCol() - col == 1) { // when adjacent
                            curUnit->decHp(atk); // hit him.
                        }
                        else { // move to east
                            // move the boss when the next position is empty(no unit).
                            if (sim->board->getUnit(row, col+1) == NULL) {
                                sim->board->setUnit(row, col, NULL);
                                sim->board->setUnit(row, col+1, this);
                                // now row and col have new coord.
                            }
                        }
                    }
                }
                else if (col == curUnit->getCol()) {
                    if (row > curUnit->getRow()) { // on north
                        if (row - curUnit->getRow() == 1) { // when adjacent
                            curUnit->decHp(atk); // hit him.
                        }
                        else { // move to north
                            // move the boss when the next position is empty(no unit).
                            if (sim->board->getUnit(row-1, col) == NULL) {
                                sim->board->setUnit(row, col, NULL);
                                sim->board->setUnit(row-1, col, this);
                                // now row and col have new coord.
                            }
                        }
                    }
                    else { // on south
                        if (curUnit->getRow() - row == 1) { // when adjacent
                            curUnit->decHp(atk); // hit him.
                        }
                        else { // move to south
                            // move the boss when the next position is empty(no unit).
                            if (sim->board->getUnit(row+1, col) == NULL) {
                                sim->board->setUnit(row, col, NULL);
                                sim->board->setUnit(row+1, col, this);
                                // now row and col have new coord.
                            }
                        }
                    }
                }
                else {
                    // check whether a human is in north-east bound
                    if (row > curUnit->getRow() && col < curUnit->getCol()) {
                        if (row - curUnit->getRow() > curUnit->getCol() - col) { // move north
                            // move the boss when the next position is empty(no unit).
                            if (sim->board->getUnit(row-1, col) == NULL) {
                                sim->board->setUnit(row, col, NULL);
                                sim->board->setUnit(row-1, col, this);
                                // now row and col have new coord.
                            }
                        }
                        else { // move east
                            // move the boss when the next position is empty(no unit).
                            if (sim->board->getUnit(row, col+1) == NULL) {
                                sim->board->setUnit(row, col, NULL);
                                sim->board->setUnit(row, col+1, this);
                                // now row and col have new coord.
                            }
                        }
                    }
                    // check whether a human is in south-east bound
                    else if (row < curUnit->getRow() && col < curUnit->getCol()) {
                        if (curUnit->getRow() - row > curUnit->getCol() - col) { // move south
                            // move the boss when the next position is empty(no unit).
                            if (sim->board->getUnit(row+1, col) == NULL) {
                                sim->board->setUnit(row, col, NULL);
                                sim->board->setUnit(row+1, col, this);
                                // now row and col have new coord.
                            }
                        }
                        else { // move east
                            // move the boss when the next position is empty(no unit).
                            if (sim->board->getUnit(row, col+1) == NULL) {
                                sim->board->setUnit(row, col, NULL);
                                sim->board->setUnit(row, col+1, this);
                                // now row and col have new coord.
                            }
                        }
                    }
                    // check whether a human is in south-west bound
                    else if (row < curUnit->getRow() && col > curUnit->getCol()) {
                        if (curUnit->getRow() - row > col - curUnit->getCol()) { // move south
                            // move the boss when the next position is empty(no unit).
                            if (sim->board->getUnit(row+1, col) == NULL) {
                                sim->board->setUnit(row, col, NULL);
                                sim->board->setUnit(row+1, col, this);
                                // now row and col have new coord.
                            }
                        }
                        else { // move west
                            // move the boss when the next position is empty(no unit).
                            if (sim->board->getUnit(row, col-1) == NULL) {
                                sim->board->setUnit(row, col, NULL);
                                sim->board->setUnit(row, col-1, this);
                                // now row and col have new coord.
                            }
                        }
                    }
                    // check whether a human is in north-west bound
                    else if (row > curUnit->getRow() && col > curUnit->getCol()) {
                        if (row - curUnit->getRow() > col - curUnit->getCol()) { // move north
                            // move the boss when the next position is empty(no unit).
                            if (sim->board->getUnit(row-1, col) == NULL) {
                                sim->board->setUnit(row, col, NULL);
                                sim->board->setUnit(row-1, col, this);
                                // now row and col have new coord.
                            }
                        }
                        else { // move west
                            // move the boss when the next position is empty(no unit).
                            if (sim->board->getUnit(row, col-1) == NULL) {
                                sim->board->setUnit(row, col, NULL);
                                sim->board->setUnit(row, col-1, this);
                                // now row and col have new coord.
                            }
                        }
                    }
                }
            }
        }
    }
}
*/

void Boss::save(ostream &out) {
    Monster::save(out);
    
    out << "#---------------------------------------- class Boss" << endl;
    
    out << "# range" << endl;
    out << range << endl;
}

void Boss::load(istream &in) {
    Monster::load(in);
    
    char buf[MAX_LEN_BUF];
    
    //out << "#---------------------------------------- class Boss" << endl;
    in.getline(buf, MAX_LEN_BUF); // skip a line
    
    //out << "# range" << endl;
    in.getline(buf, MAX_LEN_BUF); // skip a line
    //out << range << endl;
    in >> range;
    in.get(); // skip a char(new line = '\n')
}
