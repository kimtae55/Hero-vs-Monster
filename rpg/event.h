#ifndef EVENT_H
#define EVENT_H

class Monster;
class Boss;

// rpg ver0051: 새로 시뮬레이션에서 만들어진 부분:
//    - parent class인 Event에서 time관리를 주로 하고, child class에서는, 각 event별로
//      trigger에서 각자 고유의 할 일을 처리할 때 필요한 member variable들을 만들고,
//      처리를 한다.
//    - class Event에 보면 priority queue인 sim->eventQueue에서 enqueue()가 event를
//      queue에 insert할 때, ordered로 insert하므로 반드시 operator overloading이
//      필요하다. operator==()/operator<()... 요것들..

class Event {
protected:
    int time;
    
public:
    Event();
    Event(int time);

    virtual ~Event();
    
    virtual void print();
    
    int getTime();
    
    bool operator==(Event &other);
    bool operator<(Event &other);
    bool operator<=(Event &other);
    bool operator>(Event &other);
    bool operator>=(Event &other);
    
    virtual void trigger() = 0;
};

class MonsterRespawnEvent : public Event {
protected:
    Monster *mon;
    
public:
    MonsterRespawnEvent();
    MonsterRespawnEvent(int time, Monster *mon);
    
    virtual ~MonsterRespawnEvent();
    
    virtual void trigger();
};

class BossRushEvent : public Event {
protected:
    Boss *boss;
    
public:
    BossRushEvent();
    BossRushEvent(int time, Boss *boss);
    
    virtual ~BossRushEvent();
    
    virtual void trigger();
};

class BossNormalEvent : public Event {
protected:
    Boss *boss;
    
public:
    BossNormalEvent();
    BossNormalEvent(int time, Boss *boss);
    
    virtual ~BossNormalEvent();
    
    virtual void trigger();
};

#endif
