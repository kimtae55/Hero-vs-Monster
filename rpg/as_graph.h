#ifndef AS_GARPH_H
#define AS_GARPH_H

// 이것은 Graph에서 A* Algorithm을 위해 사용되어지는 node이다.(DLLNode와 헷갈리면 안 된다)
class AsNode {
public:
    int id;
    
    double f;
    double g;
    double h;
    
    int row;
    int col;
    
    AsNode *parent;
    
    AsNode *north;
    AsNode *east;
    AsNode *south;
    AsNode *west;
    
    AsNode();
    AsNode(double f, double g, double h, int row, int col);

    virtual ~AsNode();

    void print();
};

class AsSet;

class AsGraph {
private:
    AsSet *openSet;
    AsSet *closedSet;
    // 아래의 blockedSet은, hero바로 근접한 주변 4방향이 막힐 경우, 한 방향만 빼고 모두
    // blocked로 다시 간주하게 하여, hero의 4방향에 adjacent된 tile들이 한개라도 열려 있으면
    // 그쪽으로 boss가 붙도록 하게 한다.
    // * 만약 이렇게 해서 찾아낸 path가 찾아오는 도중 막히면 몇 번의 random방황후 다시 path를
    //   찾게 하는 것이 자연스러울 듯.(꼭 이렇게 implement할 필요는 없지만)
    AsSet *blockedSet;
    // 이 숫자만큼만 blocked에 add할 것.
    // 최대는 3이지만, 만약 hero주변에 3개의 prop이 있다면, 남은 한개의 unit은 그냥 통과하여
    // path로 허용하게 해야하므로, 일단 초기 maxBlocked는 3으로 하고, findPath를 시작하기 전에
    // hero에 adjacent된 prop을 count하여 이 3에서 뺀다.
    int maxBlocked;
    
    AsNode *start;
    int numNodes;
    
    double heuristicCostEstimate(int curRow, int curCol, int endRow, int endCol);
    AsNode **reconstructPath(int *pathSize, AsNode *goalNode);
    
public:
    AsGraph();
    virtual ~AsGraph();
    
    AsNode **findPath(int startRow, int startCol, int endRow, int endCol, int *pathSize);
    
    int size();
};

#endif
