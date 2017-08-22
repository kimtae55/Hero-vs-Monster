#include <iostream>

using namespace std;

#include "main.h"
#include "board.h"
#include "unit.h"
#include "hero.h"
#include "eventqueue.h"
#include "sim.h"

Sim::Sim() {
    time = 0;
    hero = NULL;
    board = NULL;
    
    eventQueue = NULL;
}

void Sim::init(int rowSize, int colSize,
          int numTrees, int numPortals,
          int numPotions, int numKeys,
          int maxNumArmors, int numWeapons,
          int numMons, int maxNumBosses, int maxNumMerchants,
          int heroRow, int heroCol) {
    
    // 얘를 미리 만드는 이유는, board등에서 event를 미리 스케쥴링하므로..
    eventQueue = new EventQueue();

    board = new Board(rowSize, colSize,
                           numTrees, numPortals, numPotions,
                           numKeys, maxNumArmors,
                           numWeapons, numMons,
                           maxNumBosses, maxNumMerchants,
                           heroRow, heroCol);
    hero = board->getHero(); // 이제부터는 board에서 hero를 만들고 관리.
}

Sim::~Sim() {
    if (board != NULL) {
        delete board;
    }
}
