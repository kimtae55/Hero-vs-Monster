#include <iostream>
#include <cmath>

using namespace std;

#include "main.h"
#include "prop.h"
#include "board.h"
#include "as_set.h"
#include "as_graph.h"

#include "sim.h"
extern Sim *sim; // main.cpp에서 global variable로 만들어진 sim을 쓰겠다는 얘기.

int curAsNodeID = 0; // debugging purpose
int numRemovedAsNode = 0; // debugging purpose

//------------------------------------------------------------------------------
// class AsNode
//------------------------------------------------------------------------------

AsNode::AsNode() {
    f = 0;
    g = 0;
    h = 0;
    
    row = -1;
    col = -1;
    
    parent = NULL;
    
    north = NULL;
    east = NULL;
    south = NULL;
    west = NULL;
}

AsNode::AsNode(double f, double g, double h, int row, int col) {
    this->f = f;
    this->g = g;
    this->h = h;
    
    this->row = row;
    this->col = col;

    parent = NULL;

    north = NULL;
    east = NULL;
    south = NULL;
    west = NULL;
}

AsNode::~AsNode() {
    // debugging purpose.
    numRemovedAsNode--;
    //cout << "AsNode::~AsNode(): id=" << id << endl;
    //cout << "                   numRemovedAsNode=" << numRemovedAsNode << endl;
    //print();
    //cout << endl;
}

void AsNode::print() {
    cout << "[ f(" << f << ") g(" << g << ") h(" << h
         << ") row(" << row << ") col(" << col << ") ]";
}

//------------------------------------------------------------------------------
// class AsGraph
//------------------------------------------------------------------------------

AsGraph::AsGraph() {
    openSet = new AsSet();
    closedSet = new AsSet();
    blockedSet = new AsSet();
    
    start = NULL;
    numNodes = 0;
    
    maxBlocked = 3;
    
    curAsNodeID = 0;
    numRemovedAsNode = 0;
}

AsGraph::~AsGraph() {
    //cout << "AsGraph::~AsGraph(): delete openSet; started" << endl;
    delete openSet;
    //cout << "AsGraph::~AsGraph(): delete openSet; end" << endl;
    //cout << "AsGraph::~AsGraph(): delete closedSet; started" << endl;
    delete closedSet;
    //cout << "AsGraph::~AsGraph(): delete closedSet; end" << endl;
    //cout << "AsGraph::~AsGraph(): delete blockedSet; started" << endl;
    delete blockedSet;
    //cout << "AsGraph::~AsGraph(): delete blockedSet; end" << endl;
}

double AsGraph::heuristicCostEstimate(int curRow, int curCol, int endRow, int endCol) {
    double diffRow = fabs(curRow - endRow);
    double diffCol = fabs(curCol - endCol);
    return sqrt(diffRow * diffRow + diffCol * diffCol);
}

AsNode **AsGraph::findPath(int startRow, int startCol, int endRow, int endCol, int *pathSize) {
    // 이 숫자만큼만 blocked에 add할 것.
    // 최대는 3이지만, 만약 hero주변에 3개의 prop이 있다면, 남은 한개의 unit은 그냥 통과하여
    // path로 허용하게 해야하므로, 일단 초기 maxBlocked는 3으로 하고, findPath를 시작하기 전에
    // hero에 adjacent된 prop을 count하여 이 3에서 뺀다.
    maxBlocked = 3;

    // check north
    if (!sim->board->validate(endRow-1, endCol) ||
        !(sim->board->getProp(endRow-1, endCol) == NULL ||
          sim->board->getProp(endRow-1, endCol)->isClimbable())) {
        maxBlocked--;
    }
    // check east
    if (!sim->board->validate(endRow, endCol+1) ||
        !(sim->board->getProp(endRow, endCol+1) == NULL ||
          sim->board->getProp(endRow, endCol+1)->isClimbable())) {
        maxBlocked--;
    }
    // check south
    if (!sim->board->validate(endRow+1, endCol) ||
        !(sim->board->getProp(endRow+1, endCol) == NULL ||
          sim->board->getProp(endRow+1, endCol)->isClimbable())) {
        maxBlocked--;
    }
    // check west
    if (!sim->board->validate(endRow, endCol-1) ||
        !(sim->board->getProp(endRow, endCol-1) == NULL ||
          sim->board->getProp(endRow, endCol-1)->isClimbable())) {
        maxBlocked--;
    }
    
    // closedset := the empty set    // The set of nodes already evaluated.
    // * 컨스트럭터에서 했으므로, 여기서는 안 해도 된다.
    
    // openset := {start}    // The set of tentative nodes to be evaluated, initially containing the start node
    double startG = 0;
    double startH = heuristicCostEstimate(startRow, startCol, endRow, endCol);
    double startF = startG + startH;
    start = new AsNode(startF, startG, startH, startRow, startCol);
    
    // for debugging purpose
    start->id = curAsNodeID;
    curAsNodeID++;
    
    //cout << "start: ";
    //start->print();
    //cout << endl;
    // * start node는 parent node가 없으므로, 자동으로 NULL이 들어가게 놔둔다.
    openSet->enqueue(start);
    
    // came_from := the empty map    // The map of navigated nodes.
    // * 이 경우, AsNode안에 parent가 있어서 어떤 node에서 왔는지 기록하고 있으므로 skip.
    
    // g_score := map with default value of Infinity
    // g_score[start] := 0    // Cost from start along best known path.
    //// Estimated total cost from start to goal through y.
    // f_score = map with default value of Infinity
    // f_score[start] := g_score[start] + heuristic_cost_estimate(start, goal)
    // 위에서 새 노드 만들때 하고 있음.
    
    // while openset is not empty
    while (!openSet->isEmpty()) {
        // current := the node in openset having the lowest f_score[] value
        AsNode *current = openSet->dequeue();
        //cout << "current: ";
        //current->print();
        //cout << endl;

        // if current = goal
        if (current->row == endRow && current->col == endCol) {
            // return reconstruct_path(came_from, goal)
            AsNode **path = reconstructPath(pathSize, current);
            if (path != NULL) {
                return path;
            }
            // path cancelled: 왜냐하면, hero와 adjecent된 tile에 다른 unit이
            // 있기 때문에, 그 unit의 위치를 blockedSet에 넣고, 다시 path를 finding한다.
            else {
                //delete current;
                
                delete openSet;
                delete closedSet;
                openSet = new AsSet();
                closedSet = new AsSet();
                
                curAsNodeID = 0; // debugging purpose
                numRemovedAsNode = 0; // debugging purpose
                
                start = new AsNode(startF, startG, startH, startRow, startCol);
                openSet->enqueue(start);
                
                // for debugging purpose
                start->id = curAsNodeID;
                curAsNodeID++;
            }
        }
        // path를 아직 찾지 못했거나, 찾았는데, hero에 unit이 근접해있어서 3번까지 다시 reset하고
        // 찾을 경우..
        else {
            // remove current from openset
            // * dequeue했으므로 상관없음.
            
            // add current to closedset
            closedSet->prepend(current);
            
            // for each neighbor in neighbor_nodes(current)
            for (int dir = 0; dir < NUM_DIRS; dir++) {
                int neighRow = current->row;
                int neighCol = current->col;
                
                if (dir == DIR_NORTH) {
                    neighRow = current->row - 1;
                }
                else if (dir == DIR_EAST) {
                    neighCol = current->col + 1;
                }
                else if (dir == DIR_SOUTH) {
                    neighRow = current->row + 1;
                }
                else if (dir == DIR_WEST) {
                    neighCol = current->col - 1;
                }
                
                // if neighbor is not in closedset
                AsNode *neighFromClosedSet = closedSet->find(neighRow, neighCol);

                // 만약 그 자리가 안 막혔고. closeSet에 이미 없을 경우..
                if (sim->board->validate(neighRow, neighCol) &&
                    // blockedSet에 들어있는 tile은 hero와 근접한 unit이 있으므로 장애물로
                    // 간주한다.
                    blockedSet->find(neighRow, neighCol) == NULL &&
                    // 바로 아래의 조건은 일단 그자리가 valid한 index일 경우, 찾는 goal인지 우선 확인하고,
                    // 찾고 있는 goal이라면, hero가 있을 것이므로, 우선 add하게 한다.
                    ((neighRow == endRow && neighCol == endCol) ||
                     // 하지만 찾는 goal이 아닌 장애물이라면 피해야 하므로 넣지 않게 조건을 체크한다.
                     // 또한 unit은 장애물에 넣지 않는다. 이유는, 몬스터나 보스가 hero를 둘러 쌓을 경우,
                     // 그냥 일단 path는 찾고 통과못하고 밖에서 대기하게 한다.
                     (sim->board->getProp(neighRow, neighCol) == NULL ||
                      sim->board->getProp(neighRow, neighCol)->isClimbable())) &&
                      neighFromClosedSet == NULL) {
                    // tentative_g_score := g_score[current] + dist_between(current,neighbor)
                    double neighG = current->g + 1;
                    double neighH = heuristicCostEstimate(neighRow, neighCol, endRow, endCol);
                    double neighF = neighG + neighH;

                    // if neighbor not in openset or tentative_g_score < g_score[neighbor]
                    AsNode *neighFromOpenSet = openSet->find(neighRow, neighCol);
                    
                    if (neighFromOpenSet != NULL) {
                        if (neighG < neighFromOpenSet->g) {
                            // * 만약 이미 openSet에 있는 경우, 현재 path로 계산되어온 g값이 더 작을 경우, 이미
                            //   있었던 그 node의 원래 parent로부터 온 path가 더 긴 셈이므로, 새롭게 현재 path로
                            //   바꿔주기 위해, parent를 current로 바꾼다.
                            
                            // came_from[neighbor] := current
                            neighFromOpenSet->parent = current;
                            // g_score[neighbor] := tentative_g_score
                            neighFromOpenSet->g = neighG;
                            // f_score[neighbor] := g_score[neighbor] + heuristic_cost_estimate(neighbor, goal)
                            neighFromOpenSet->f = neighF;
                        }
                    }
                    // if neighbor not in openset
                    else {
                        // add neighbor to openset
                        AsNode *newNode = new AsNode(neighF, neighG, neighH, neighRow, neighCol);
                        newNode->parent = current;
                        
                        // for debugging purpose
                        newNode->id = curAsNodeID;
                        curAsNodeID++;

                        //cout << "  newNode: ";
                        //newNode->print();
                        //cout << endl;

                        openSet->enqueue(newNode);
                        
                        if (dir == DIR_NORTH) {
                            current->north = newNode;
                        }
                        else if (dir == DIR_EAST) {
                            current->east = newNode;
                        }
                        else if (dir == DIR_SOUTH) {
                            current->south = newNode;
                        }
                        else if (dir == DIR_WEST) {
                            current->west = newNode;
                        }
                    }
                }
                // if neighbor in closedset
                // continue
            }
        }
    }
    
    // return failure
    return NULL;
}

AsNode **AsGraph::reconstructPath(int *pathSize, AsNode *goalNode) {
    // 아래에서 "blockedSet->size() < 3"를 비교하는 이유는, 3개의 근접한 unit만 blocked로
    // 처리하고, 만약 hero의 동서남북 4방향이 모두 unit으로 막혔다면, 그냥 path를 찾은 것으로 하고
    // 근처까지 boss를 오게한다.
    if (blockedSet->size() < maxBlocked &&
        sim->board->getUnit(goalNode->parent->row, goalNode->parent->col) != NULL) {
        openSet->remove(goalNode->parent->row, goalNode->parent->col);
        closedSet->remove(goalNode->parent->row, goalNode->parent->col);
        blockedSet->prepend(goalNode->parent);
        return NULL;
    }
    
    // 우선 goalNode부터 거꾸로 따라가면서(parent를 따라간다) start까지 노드가 몇개인지를
    // count한다. 그래야만, return할 path를 담을 array를 만들 수 있다.
    int numNodesInPath = 0;
    AsNode *current = goalNode;
    while (current != NULL) {
        numNodesInPath++;
        
        current = current->parent;
    }
    
    // 위에서 count된 node(path의) 갯수로 array를 만든다.
    AsNode **path = new AsNode * [numNodesInPath];
    
    // goalNode(path의 맨 끝부터)에서부터 따라가면서 array의 맨 끝부터 AsNode를 넣어준다.
    current = goalNode;
    for (int i = numNodesInPath-1; i >= 0; i--) {
        openSet->remove(current->row, current->col);
        closedSet->remove(current->row, current->col);
        path[i] = current;
        
        current = current->parent;
    }
    
    // caller가 return된 array의 size를 알 수 있도록 원격으로(remotely) 넣어준다.
    *pathSize = numNodesInPath;
    return path; // 이렇게 path가 들어가 있는 완성된 array를 return한다.
}

/* source: https://en.wikipedia.org/wiki/A*_search_algorithm
function A*(start,goal)
    closedset := the empty set    // The set of nodes already evaluated.
    openset := {start}    // The set of tentative nodes to be evaluated, initially containing the start node
    came_from := the empty map    // The map of navigated nodes.

    g_score := map with default value of Infinity
    g_score[start] := 0    // Cost from start along best known path.
    // Estimated total cost from start to goal through y.
    f_score = map with default value of Infinity
    f_score[start] := g_score[start] + heuristic_cost_estimate(start, goal)

    while openset is not empty
        current := the node in openset having the lowest f_score[] value
        if current = goal
            return reconstruct_path(came_from, goal)

        remove current from openset
        add current to closedset
        for each neighbor in neighbor_nodes(current)
            if neighbor in closedset
                continue
            tentative_g_score := g_score[current] + dist_between(current,neighbor)

            if neighbor not in openset or tentative_g_score < g_score[neighbor] 
                came_from[neighbor] := current
                g_score[neighbor] := tentative_g_score
                f_score[neighbor] := g_score[neighbor] + heuristic_cost_estimate(neighbor, goal)
                if neighbor not in openset
                    add neighbor to openset

    return failure
 }
 
function reconstruct_path(came_from,current)
    total_path := [current]
    while current in came_from:
        current := came_from[current]
        total_path.append(current)
    return total_path
 */


int AsGraph::size() {
    return numNodes;
}
