#include <iostream>

using namespace std;

#include "event.h"
#include "eventqueue.h"

EventQueue::EventQueue() {
    head = NULL;
    tail = NULL;
    numNodes = 0;
}

EventQueue::~EventQueue() {
    if (head != NULL) { // size() > 0
        if (head->next == NULL) { // size() == 1
            delete head;
            head = NULL;
            tail = NULL;
            numNodes = 0;
        }
        else { // size() > 1
            EQNode *current = head;
            
            while (current != NULL) {
                EQNode *next = current->next;
                delete current;
                
                current = next;
            }
            head = NULL;
            tail = NULL;
            numNodes = 0;
        }
    }
}

void EventQueue::print() {
    if (head == NULL) { // size() == 0
    }
    else if (head->next == NULL) { // size() == 1
        cout << head->elem << endl;
    }
    else { // size() > 1
        EQNode *current = head;
        
        while (current != NULL) {
            current->elem->print();
            cout << " ";
            
            current = current->next;
        }
        cout << endl;
    }
}

// ordered list용.
// dupilcate ok
void EventQueue::enqueue(Event *elem) {
    EQNode *newNode = new EQNode(elem);
    
    if (head == NULL) { // size() == 0
        head = newNode;
        tail = newNode;
        numNodes++;
    }
    else if (head->next == NULL) { // size() == 1
        if (elem < head->elem) { // prepend
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
        EQNode *prev = NULL;
        EQNode *current = head;
        
        while (current != NULL) {
            // elem은 Event *이기 때문에, *elem이라고 해야만, object 그 자체가 되어
            // 비교할 수 있다. 만약 안 붙이면, 주소가 비교되므로, 엉뚱한 결과가 나온다.
            if (*elem < *current->elem) {
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

Event *EventQueue::peek() {
    if (numNodes <= 0) {
        //cout << "error" << endl;
        return NULL;
    }

    return head->elem;
}

Event *EventQueue::dequeue() {
    if (head == NULL) { // size() == 0
        return NULL; // error
    }
    else if (head->next == NULL) { // size() == 1
        Event *elemToReturn = head->elem;
        delete head;
        head = NULL;
        tail = NULL;
        numNodes--;
        return elemToReturn;
    }
    else { // size() > 1
        Event *elemToReturn = head->elem;
        EQNode *nodeToRemove = head;
        head = head->next;
        head->prev = NULL;
        delete nodeToRemove;
        numNodes--;
        return elemToReturn;
    }
}

int EventQueue::size() {
    return numNodes;
}
