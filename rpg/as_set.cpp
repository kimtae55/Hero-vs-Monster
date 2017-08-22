#include <iostream>

using namespace std;

#include "as_graph.h"
#include "as_set.h"

extern int curAsNodeID; // debugging purpose
extern int numRemovedAsNode; // debugging purpose

//------------------------------------------------------------------------------
// class SetNode
//------------------------------------------------------------------------------

SetNode::SetNode() {
    elem = NULL;
    prev = NULL;
    next = NULL;
}

SetNode::SetNode(AsNode *elem) {
    this->elem = elem;
    prev = NULL;
    next = NULL;
}

SetNode::~SetNode() {
}

//------------------------------------------------------------------------------
// class AsSet
//------------------------------------------------------------------------------

AsSet::AsSet() {
    head = NULL;
    tail = NULL;
    numNodes = 0;
}

AsSet::~AsSet() {
    if (head != NULL) { // size() > 0
        if (head->next == NULL) { // size() == 1
            delete head;
            head = NULL;
            tail = NULL;
            numNodes = 0;
        }
        else { // size() > 1
            SetNode *current = head;
            
            while (current != NULL) {
                SetNode *next = current->next;
                // * 원래는 내부에 담고 있는 elem은 free시키지 않지만, AsNode는 나중에
                //   openSet/closedSet이 delete될 때, 누군가 지워주지 않으므로, 이곳에서
                //   같이 지워야 한다.
                // * 아니면, AsGraph의 start지점으로부터 traversing하면서 지워야 한다.
                delete current->elem;
                delete current;
                
                current = next;
            }
            head = NULL;
            tail = NULL;
            numNodes = 0;
        }
    }
}

void AsSet::print() {
    if (head == NULL) { // size() == 0
    }
    else if (head->next == NULL) { // size() == 1
        cout << head->elem << endl;
    }
    else { // size() > 1
        SetNode *current = head;
        
        while (current != NULL) {
            current->elem->print();
            cout << endl;
            
            current = current->next;
        }
        cout << endl;
    }
}

// [ A* 앨거리듬의 closedSset용 ]
// * pqueue용으로 이 AsSet을 사용할 경우, prepend는 사용하지 말아야 한다.
void AsSet::prepend(AsNode *elem) {
    SetNode *newNode = new SetNode(elem);
    
    if (numNodes == 0) {
        head = newNode;
        tail = newNode;
        numNodes++;
    }
    else {
        newNode->next = head;
        head->prev = newNode;
        head = newNode;
        numNodes++;
    }
}

// [ A* 앨거리듬의 openSet용 ]
// * pqueue용 enqueue이다. AsNode의 f값을 이용하여 incremental order로 insert한다.
//   따라서 head가 가리키는 first node가 dequeue나 peek에 의해 나올 node이다.
// * order는 f값을 이용해서 정해진다.
void AsSet::enqueue(AsNode *elem) {
    SetNode *newNode = new SetNode(elem);
    
    if (head == NULL) { // size() == 0
        head = newNode;
        tail = newNode;
        numNodes++;
    }
    else if (head->next == NULL) { // size() == 1
        if (elem->f < head->elem->f) { // prepend
            newNode->next = head;
            head->prev = newNode;
            head = newNode;
            numNodes++;
        }
        else { // append
            tail->next = newNode;
            newNode->prev = tail;
            tail = newNode;
            numNodes++;
        }
    }
    else { // size() > 1
        SetNode *prev = NULL;
        SetNode *current = head;
        
        while (current != NULL) {
            // elem은 Event *이기 때문에, *elem이라고 해야만, object 그 자체가 되어
            // 비교할 수 있다. 만약 안 붙이면, 주소가 비교되므로, 엉뚱한 결과가 나온다.
            if (elem->f < current->elem->f) {
                break;
            }
            
            prev = current;
            current = current->next;
        }
        
        // 끼워넣을 장소
        if (prev == NULL) { // prepend
            newNode->next = head;
            head->prev = newNode;
            head = newNode;
            numNodes++;
        }
        else if (current == NULL) { // append
            tail->next = newNode;
            newNode->prev = tail;
            tail = newNode;
            numNodes++;
        }
        else { // insert the new node between prev and current
            prev->next = newNode;
            newNode->prev = prev;
            newNode->next = current;
            current->prev = newNode;
            numNodes++;
        }
    }
}

// [ A* 앨거리듬의 openSet용 ]
AsNode *AsSet::dequeue() {
    if (head == NULL) { // size() == 0
        return NULL; // error
    }
    else if (head->next == NULL) { // size() == 1
        AsNode *elemToReturn = head->elem;
        delete head;
        head = NULL;
        tail = NULL;
        numNodes--;
        return elemToReturn;
    }
    else { // size() > 1
        AsNode *elemToReturn = head->elem;
        SetNode *nodeToRemove = head;
        head = head->next;
        head->prev = NULL;
        delete nodeToRemove;
        numNodes--;
        return elemToReturn;
    }
}

// [ A* 앨거리듬의 openSet용 ]
AsNode *AsSet::peek() {
    if (numNodes <= 0) {
        //cout << "error" << endl;
        return NULL;
    }
    
    return head->elem;
}

// [ A* 앨거리듬의 openSet/closedSet용 ]
// * 이 find는 주어진 row, col로 같은 위치의 AsNode가 이미 들어가있는지 확인할 때 사용.
AsNode *AsSet::find(int row, int col) {
    if (head == NULL) { // size() == 0
        return NULL;
    }
    else if (head->next == NULL) { // size() == 1
        if (row == head->elem->row && 
            col == head->elem->col) {
            return head->elem;
        }
        else {
            return NULL;
        }
    }
    else { // size() > 1
        SetNode *current = head;
        
        while (current != NULL) {
            if (row == current->elem->row &&
                col == current->elem->col) {
                return current->elem;
            }
            
            current = current->next;
        }
        //  current가 NULL되어서 빠져나왔다는 것은, 결국 못 찾았다는 말이므로, -1을 return.
        return NULL;
    }
}

// [ A* 앨거리듬의 closedSet용 ]
// * 이 remove는 주어진 row, col로 같은 위치의 AsNode가 이미 들어가있는지 확인하고,
//   해당 포인터를 free시키지 않고, 지운 후, return하게 한다.
// * 나중에 AsGraph::reconstructPath()에서 return될 path에 들어간 AsNode들을
//   closedSet으로부터 지워서, 후에 AsGraph가 delete될 때, destructor에 의해
//   지워지지 않게 한다.
AsNode *AsSet::remove(int row, int col) {
    if (head == NULL) { // size() == 0
        return NULL;
    }
    else if (head->next == NULL) { // size() == 1
        if (row == head->elem->row && 
            col == head->elem->col) {
            AsNode *elemToReturn = head->elem;
            delete head;
            head = NULL;
            tail = NULL;
            numNodes--;
            return elemToReturn;
        }
        else {
            return NULL;
        }
    }
    else { // size() > 1
        SetNode *prev = NULL;
        SetNode *current = head;
        
        while (current != NULL) {
            if (row == current->elem->row &&
                col == current->elem->col) {
                break;
            }
            
            prev = current;
            current = current->next;
        }
        
        if (prev == NULL) { // remove first node
            AsNode *elemToReturn = current->elem;
            head = head->next;
            head->prev = NULL;
            delete current;
            numNodes--;
            return elemToReturn;
        }
        //  current가 NULL되어서 빠져나왔다는 것은, 결국 못 찾았다는 말이므로, -1을 return.
        else if (current == NULL) {
            return NULL;
        }
        // 첫번째 노드가 아닌 노드를 지울 때, 단, 마지막 노드인지 체크해서 tail도 handle해줘야 한다.
        else {
            AsNode *elemToReturn = current->elem;
            prev->next = current->next;
            if (prev->next == NULL) {
                tail = prev;
            }
            else {
                prev->next->prev = prev;
            }
            delete current;
            numNodes--;
            return elemToReturn;
        }
    }
}

int AsSet::size() {
    return numNodes;
}

bool AsSet::isEmpty() {
    return numNodes == 0;
}
