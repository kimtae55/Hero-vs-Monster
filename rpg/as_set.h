#ifndef AS_SET_H
#define AS_SET_H

class AsNode;

// DLLNode와 같다.
class SetNode {
public:
    AsNode *elem;
    SetNode *prev;
    SetNode *next;
    
    SetNode();
    SetNode(AsNode *elem);

    virtual ~SetNode();
};

// DLL
class AsSet {
private:
    SetNode *head;
    SetNode *tail;
    int numNodes;
    
public:
    AsSet();
    virtual ~AsSet();
    
    void print();
    
    // [ A* 앨거리듬의 closedSset용 ]
    // * pqueue용으로 이 AsSet을 사용할 경우, prepend는 사용하지 말아야 한다.
    void prepend(AsNode *elem);
    
    // [ A* 앨거리듬의 openSet용 ]
    // * pqueue용 enqueue이다. AsNode의 f값을 이용하여 incremental order로 insert한다.
    //   따라서 head가 가리키는 first node가 dequeue나 peek에 의해 나올 node이다.
    // * order는 f값을 이용해서 정해진다.
    void enqueue(AsNode *elem);
    
    // [ A* 앨거리듬의 openSet용 ]
    AsNode *dequeue();
    
    // [ A* 앨거리듬의 openSet용 ]
    AsNode *peek();
    
    // [ A* 앨거리듬의 openSet/closedSet용 ]
    // * 이 find는 주어진 row, col로 같은 위치의 AsNode가 이미 들어가있는지 확인할 때 사용.
    AsNode *find(int row, int col);

    // [ A* 앨거리듬의 closedSet용 ]
    // * 이 remove는 주어진 row, col로 같은 위치의 AsNode가 이미 들어가있는지 확인하고,
    //   해당 포인터를 free시키지 않고, 지운 후, return하게 한다.
    // * 나중에 AsGraph::reconstructPath()에서 return될 path에 들어간 AsNode들을
    //   closedSet으로부터 지워서, 후에 AsGraph가 delete될 때, destructor에 의해
    //   지워지지 않게 한다.
    AsNode *remove(int row, int col);

    int size();
    bool isEmpty();
};

#endif
