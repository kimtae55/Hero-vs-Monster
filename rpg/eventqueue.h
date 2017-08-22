#ifndef EVENTQUEUE_H
#define EVENTQUEUE_H

class Event;

// rpg ver0051: 새로 시뮬레이션에서 만들어진 부분:
//    - 그냥 DLL을 이용하여(어떻게 DLL인 줄 알까? EQNode에 보면 next외에도 prev가 있다.)
//      priority queue를 implement한 것이다.
//    - PQ(priority queue)? 그냥 enqueue()가 ordered로 insert한 것이다.
//      이때, 중복성(duplication)을 허용해야 하는 이유는, event들은 time을 비교하여
//      organize되는데, time이 같은 event들도 있으므로..
//    - 나머지는 너무 쉽다.

class EQNode {
public:
    Event *elem;
    EQNode *prev;
    EQNode *next;
    
    EQNode() {
        elem = NULL;
        prev = NULL;
        next = NULL;
    }
    
    EQNode(Event *elem) {
        this->elem = elem;
        prev = NULL;
        next = NULL;
    }
};

class EventQueue {
private:
    EQNode *head;
    EQNode *tail;
    int numNodes;
    
public:
    EventQueue();
    virtual ~EventQueue();
    
    void print();
    
    void enqueue(Event *elem);
    Event *dequeue();
    Event *peek();
    int size();
};

#endif
