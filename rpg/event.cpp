#include <iostream>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

using namespace std;

#include "main.h"
#include "unit.h"
#include "monster.h"
#include "event.h"
#include "eventqueue.h"
#include "board.h"

#include "sim.h"
extern Sim *sim; // main.cpp에서 global variable로 만들어진 sim을 쓰겠다는 얘기.

//------------------------------------------------------------------------------
// class Event
//------------------------------------------------------------------------------

Event::Event() {
    time = 0;
}

Event::Event(int time) {
    this->time = time;
}

Event::~Event() {
}

void Event::print() {
    cout << "time: " << time;
}

int Event::getTime() {
    return time;
}

bool Event::operator==(Event &other) {
    return time == other.time;
}

bool Event::operator<(Event &other) {
    return time < other.time;
}

bool Event::operator<=(Event &other) {
    return time <= other.time;
}

bool Event::operator>(Event &other) {
    return time > other.time;
}

bool Event::operator>=(Event &other) {
    return time >= other.time;
}

//------------------------------------------------------------------------------
// class MonsterRespawnEvent
//------------------------------------------------------------------------------

MonsterRespawnEvent::MonsterRespawnEvent() : Event() {
    mon = NULL;
}

// rpg ver0051: 새로 시뮬레이션에서 만들어진 부분:
//    - 일단 이렇게 부활할 monster(죽어있고 board에서 제거된, 하지만 board의 mons[]에 남아있는)
//      의 주소를 event가 만들어질때 받아둔다.

MonsterRespawnEvent::MonsterRespawnEvent(int time, Monster *mon) : Event(time) {
    this->mon = mon;
}

MonsterRespawnEvent::~MonsterRespawnEvent() {
    // 여기서는 절대 몬스터를 delete하면 안 됨. 나중에 tile이나 board등에서 지워지게 된다.
    // 살아있는 경우는, tile의 destructor가 지우게 되고, 죽은 경우, board의 destroyMons()가
    // 지운다.
    //if (mon != NULL) {
    //    delete mon;
    //}
}

// rpg ver0051: 새로 시뮬레이션에서 만들어진 부분:
//    - main()의 main game loop안의 event처리 loop안에서.. dequeue()가 된 후, 아래의
//      trigger()가 call되면, 죽어 있던 mon을 부활시킨다.
//    - "mon->revive()"가 필요한 이유는, 죽은 monster의 경우, hp가 0이고, died라는 변수는
//      true이고.. 등등 여러가지가 훼손되어 있기 때문에, 다시 원래대로 값들을 모두 reset해준다.
//      
//    - 그리고 board의 랜덤위치에 갖다 놓는다.
//
//    - 그 후, "delete this;"를 이용하여 자신(event)을 heap에서 직접 지운다.

void MonsterRespawnEvent::trigger() {
    mon->revive();
    
    while (true) {
        int randRow = rand() % sim->board->getRowSize();
        int randCol = rand() % sim->board->getColSize();
        
        if (sim->board->getUnit(randRow, randCol) == NULL &&
            sim->board->getProp(randRow, randCol) == NULL &&
            sim->board->getItem(randRow, randCol) == NULL) {
            sim->board->setUnit(randRow, randCol, mon);
            break;
        }
    }
    
    delete this;
}

//------------------------------------------------------------------------------
// class BossRushEvent
//------------------------------------------------------------------------------

BossRushEvent::BossRushEvent() : Event() {
    boss = NULL;
}

BossRushEvent::BossRushEvent(int time, Boss *boss) : Event(time) {
    this->boss = boss;
}

BossRushEvent::~BossRushEvent() {
    // 여기서는 절대 boss를 delete하면 안 됨. 나중에 tile이나 board등에서 지워지게 된다.
    // 살아있는 경우는, tile의 destructor가 지우게 되고, 죽은 경우, board의 destroyBosses()가
    // 지운다.
    //if (boss != NULL) {
    //    delete boss;
    //}
}

void BossRushEvent::trigger() {
    // boss가 rush 시간이 되기 전에 죽었다면 처리하지 않아도 된다.
    if (!boss->isDead()) {
        if (sim->board->getRowSize() < sim->board->getColSize()) {
            boss->setRange(sim->board->getColSize());
        }
        else {
            boss->setRange(sim->board->getRowSize());
        }

        // boss가 죽지 않았을 때만 계속 scheduling.
        sim->eventQueue->enqueue(
            new BossNormalEvent(sim->time + BOSS_RUSH_EVENT_DURATION, boss));
    }
    
    delete this;
}

//------------------------------------------------------------------------------
// class BossNormalEvent
//------------------------------------------------------------------------------

BossNormalEvent::BossNormalEvent() : Event() {
    boss = NULL;
}

BossNormalEvent::BossNormalEvent(int time, Boss *boss) : Event(time) {
    this->boss = boss;
}

BossNormalEvent::~BossNormalEvent() {
    // 여기서는 절대 boss를 delete하면 안 됨. 나중에 tile이나 board등에서 지워지게 된다.
    // 살아있는 경우는, tile의 destructor가 지우게 되고, 죽은 경우, board의 destroyBosses()가
    // 지운다.
    //if (boss != NULL) {
    //    delete boss;
    //}
}

void BossNormalEvent::trigger() {
    // boss가 죽었다면 처리하지 않아도 된다.
    if (!boss->isDead()) {
        boss->resetRange();
        
        // boss가 죽지 않았을 때만 계속 scheduling.
        sim->eventQueue->enqueue(
                new BossRushEvent(sim->time + BOSS_RUSH_EVENT_INTERVAL, boss));
    }
        
    delete this;
}
