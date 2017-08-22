#include <iostream>
#include <iomanip>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

using namespace std;

#include "main.h"
#include "unit.h"
#include "item.h"
#include "prop.h"
#include "monster.h"
#include "hero.h"
#include "merchant.h"
#include "tile.h"
#include "board.h"
#include "item.h"
#include "event.h"
#include "eventqueue.h"

#include "sim.h"
extern Sim *sim; // main.cpp에서 global variable로 만들어진 sim을 쓰겠다는 얘기.

int numBossesLeft = 0; // debugging purpose

Board::Board() {
    initBoard(DEFAULT_ROW_SIZE, DEFAULT_COL_SIZE);

    // init hero
    hero = new Hero();
    setUnit(DEFAULT_ROW_SIZE/2, DEFAULT_COL_SIZE/2, hero);

    initProps(MAX_NUM_TREES, MAX_NUM_PORTALS);
    initItems(MAX_NUM_POTIONS, MAX_NUM_KEYS, MAX_NUM_ARMORS, MAX_NUM_WEAPONS);

    initMons(MAX_NUM_MONS);
    initBosses(MAX_NUM_BOSSES);
	initUnits(MAX_NUM_MERCHANTS);
}

Board::Board(int rowSize, int colSize, int numTrees, int numPortals, int numPotions, int numKeys,
			 int numArmors, int numWeapons, int maxNumMons, int maxNumBosses, int maxNumMerchants,
             int heroRow, int heroCol) {
    initBoard(rowSize, colSize);
    
    // init hero
    hero = new Hero();
    setUnit(heroRow, heroCol, hero);
    
    initProps(numTrees, numPortals);
    initItems(numPotions, numKeys, numArmors, numWeapons);
    
    initMons(maxNumMons);
    initBosses(maxNumBosses);
	initUnits(maxNumMerchants);
}

void Board::initBoard(int rowSize, int colSize) {
    // initialize the board.
    this->rowSize = rowSize;
    this->colSize = colSize;
    
    board = new Tile **[rowSize];
    
    for (int i = 0; i < rowSize; i++) {
        board[i] = new Tile *[colSize];
    }
    
    for (int i = 0; i < rowSize; i++) {
        for (int j = 0; j < colSize; j++) {
            board[i][j] = new Tile();
        }
    }
}

Board::~Board() {
    destroyBoard();
}

void Board::destroyBoard() {
    if (mons != NULL) {
        for (int i = 0; i < maxNumMons; i++) {
            if (mons[i] != NULL && mons[i]->isDead()) {
                delete mons[i];
            }
        }
        
        delete [] mons;
        
        maxNumMons = 0;
        numMons = 0;
    }

    if (bosses != NULL) {
        for (int i = 0; i < maxNumBosses; i++) {
            if (bosses[i] != NULL && bosses[i]->isDead()) {
                delete bosses[i];
            }
        }
        
        delete [] bosses;
        
        maxNumBosses = 0;
        numBosses = 0;
    }

	if (units != NULL) {
		for (int i = 0; i < maxNumUnits; i++) {
			if (units[i] != NULL && units[i]->isDead()) {
				delete units[i];
			}
		}
		
		delete [] units;
		
		maxNumUnits = 0;
		numUnits = 0;
	}
    
    if (board != NULL) {
        for (int i = 0; i < rowSize; i++) {
            for (int j = 0; j < colSize; j++) {
                delete board[i][j];
            }
        }
        
        for (int i = 0; i < rowSize; i++) {
            delete [] board[i];
        }
        
        delete [] board;
        board = NULL;
        
        rowSize = 0;
        colSize = 0;
    }
}


void Board::initMons(int maxNumMons) {
    this->maxNumMons = maxNumMons;
    numMons = 0;
    
    mons = new Monster *[maxNumMons];
    
    for (int i = 0; i < maxNumMons; i++) {
        mons[i] = new Monster();
        
        while (true) {
            int randRow = rand() % rowSize;
            int randCol = rand() % colSize;
            
            if (board[randRow][randCol]->getUnit() == NULL &&
                board[randRow][randCol]->getProp() == NULL &&
                board[randRow][randCol]->getItem() == NULL) {
                setUnit(randRow, randCol, mons[i]);
                break;
            }
        }
        
        numMons++;
    }
}

void Board::initBosses(int maxNumBosses) {
    this->maxNumBosses = maxNumBosses;
    numBossesLeft = maxNumBosses;
    numBosses = 0;
    
    bosses = new Boss *[maxNumBosses];
    
    for (int i = 0; i < maxNumBosses; i++) {
        bosses[i] = new Boss();
        
        while (true) {
            int randRow = rand() % rowSize;
            int randCol = rand() % colSize;
            
            if (board[randRow][randCol]->getUnit() == NULL &&
                board[randRow][randCol]->getProp() == NULL &&
                board[randRow][randCol]->getItem() == NULL) {
                setUnit(randRow, randCol, bosses[i]);
                sim->eventQueue->enqueue(
                        new BossRushEvent(
                            sim->time + BOSS_RUSH_EVENT_INTERVAL, bosses[i]));
                break;
            }
        }
        
        numBosses++;
    }
}

void Board::initUnits(int maxNumMerchants) { // general units
	maxNumUnits = maxNumMerchants;
	numUnits = 0;
	
	units = new Unit *[maxNumUnits];
	
	// create random merchants
	for (int i = 0; i < maxNumMerchants; i++) {
		units[numUnits] = new Merchant();

		// create a Merchant object
		while (true) {
			int randRow = rand() % rowSize;
			int randCol = rand() % colSize;
			
			if (board[randRow][randCol]->getUnit() == NULL &&
				board[randRow][randCol]->getProp() == NULL &&
				board[randRow][randCol]->getItem() == NULL) {
				setUnit(randRow, randCol, units[numUnits]);
				break;
			}
		}
		numUnits++;
	}
}

void Board::initProps(int maxNumTrees, int maxNumPortals) {
    // create Tree objects.
    for (int i = 0; i < maxNumTrees; i++) {
        while (true) {
            int randRow = rand() % rowSize;
            int randCol = rand() % colSize;
            
            if (board[randRow][randCol]->getUnit() == NULL &&
                board[randRow][randCol]->getProp() == NULL &&
                board[randRow][randCol]->getItem() == NULL) {
                setProp(randRow, randCol, new Tree());
                break;
            }
        }
    }
    
    // create a LockedChest object.
    while (true) {
        int randRow = rand() % rowSize;
        int randCol = rand() % colSize;
        
        if (board[randRow][randCol]->getUnit() == NULL &&
            board[randRow][randCol]->getProp() == NULL &&
            board[randRow][randCol]->getItem() == NULL) {
			if (rand() % 2 == 0) {
				setProp(randRow, randCol, new LockedChest(createRandomArmor()));
			}
			else {
				setProp(randRow, randCol, new LockedChest(createRandomWeapon()));
			}
            break;
        }
    }
	
	// create a Fountain object.
	while (true) {
		int randRow = rand() % rowSize;
		int randCol = rand() % colSize;
		
		if (board[randRow][randCol]->getUnit() == NULL &&
			board[randRow][randCol]->getProp() == NULL &&
			board[randRow][randCol]->getItem() == NULL) {
			setProp(randRow, randCol, new Fountain());
			break;
		}
	}
	
	// create Tree objects.
	for (int i = 0; i < maxNumPortals; i++) {
		while (true) {
			int randRow = rand() % rowSize;
			int randCol = rand() % colSize;
			
			if (board[randRow][randCol]->getUnit() == NULL &&
				board[randRow][randCol]->getProp() == NULL &&
				board[randRow][randCol]->getItem() == NULL) {
				setProp(randRow, randCol, new Portal());
				break;
			}
		}
	}
	
	// create Exit objects.
	while (true) {
		int randRow = rand() % rowSize;
		int randCol = rand() % colSize;
		
		if (board[randRow][randCol]->getUnit() == NULL &&
			board[randRow][randCol]->getProp() == NULL &&
			board[randRow][randCol]->getItem() == NULL) {
			setProp(randRow, randCol, new Exit());
			break;
		}
	}
}

void Board::initItems(int maxNumPotions, int maxNumKeys, int maxNumArmors, int maxNumWeapons) {
    for (int i = 0; i < maxNumPotions; i++) {
        while (true) {
            int randRow = rand() % rowSize;
            int randCol = rand() % colSize;
            
            if (board[randRow][randCol]->getUnit() == NULL &&
                board[randRow][randCol]->getProp() == NULL &&
                board[randRow][randCol]->getItem() == NULL) {
                board[randRow][randCol]->setItem(new Potion());
                break;
            }
        }
    }
    
    // create Key objects.
    for (int i = 0; i < maxNumKeys; i++) {
        while (true) {
            int randRow = rand() % rowSize;
            int randCol = rand() % colSize;
            
            if (board[randRow][randCol]->getUnit() == NULL &&
                board[randRow][randCol]->getProp() == NULL &&
                board[randRow][randCol]->getItem() == NULL) {
                board[randRow][randCol]->setItem(new Key());
                break;
            }
        }
    }
    
    // create random armors
	for (int i = 0; i < maxNumArmors; i++) {
		while (true) {
			int randRow = rand() % rowSize;
			int randCol = rand() % colSize;
			
			if (board[randRow][randCol]->getUnit() == NULL &&
				board[randRow][randCol]->getProp() == NULL &&
				board[randRow][randCol]->getItem() == NULL) {
				board[randRow][randCol]->setItem(createRandomArmor());
				break;
			}
		}
	}
	
    // create random weapons
	for (int i = 0; i < maxNumWeapons; i++) {
		while (true) {
			int randRow = rand() % rowSize;
			int randCol = rand() % colSize;
			
			if (board[randRow][randCol]->getUnit() == NULL &&
				board[randRow][randCol]->getProp() == NULL &&
				board[randRow][randCol]->getItem() == NULL) {
				board[randRow][randCol]->setItem(createRandomWeapon());
				break;
			}
		}
    }
}

void Board::print() {
	for (int j = 0; j < colSize; j++) {
		cout << '-';
	}
	cout << endl;

    for (int i = 0; i < rowSize; i++) {
        for (int j = 0; j < colSize; j++) {
            board[i][j]->print();
        }
        cout << endl;
    }
}

void Board::printViewport(int centerRow, int centerCol, int viewportRowHalfRange, int viewportColHalfRange, int screenWidth) {
    int startRow = centerRow-viewportRowHalfRange;
    int endRow = centerRow+viewportRowHalfRange;
    
    int startCol = centerCol-viewportColHalfRange;
    int endCol = centerCol+viewportColHalfRange;
    
    cout << endl;
    
    //--------------------------------------------------------------------------
    // First Digit
    //--------------------------------------------------------------------------
    
    // left margin
    int leftMargin = (screenWidth-(viewportColHalfRange*2+1))/2;
    for (int j = 0; j < leftMargin; j++) {
        cout << " ";
    }
    
    cout << "    ";
    for (int j = startCol; j <= endCol; j++) {
        if (j >= 0 && j < colSize) {
            cout << j / 100;
        }
        else {
            cout << " ";
        }
    }
    cout << endl;
    
    //--------------------------------------------------------------------------
    // Second Digit
    //--------------------------------------------------------------------------
    
    // left margin
    leftMargin = (screenWidth-(viewportColHalfRange*2+1))/2;
    for (int j = 0; j < leftMargin; j++) {
        cout << " ";
    }
    
    cout << "    ";
    for (int j = startCol; j <= endCol; j++) {
        if (j >= 0 && j < colSize) {
            cout << (j / 10) % 10;
        }
        else {
            cout << " ";
        }
    }
    cout << endl;
    
    //--------------------------------------------------------------------------
    // Third Digit
    //--------------------------------------------------------------------------
    
    // left margin
    leftMargin = (screenWidth-(viewportColHalfRange*2+1))/2;
    for (int j = 0; j < leftMargin; j++) {
        cout << " ";
    }
    
    cout << "    ";
    for (int j = startCol; j <= endCol; j++) {
        if (j >= 0 && j < colSize) {
            cout << j % 10;
        }
        else {
            cout << " ";
        }
    }
    cout << endl;
    
    //--------------------------------------------------------------------------
    // Third Line
    //--------------------------------------------------------------------------

    // left margin
    leftMargin = (screenWidth-(viewportColHalfRange*2+1))/2;
    for (int j = 0; j < leftMargin; j++) {
        cout << " ";
    }

    cout << "   +";
    for (int j = 0; j < viewportColHalfRange*2+1; j++) {
        cout << "-";
    }
    cout << "+" << endl;
    
    //--------------------------------------------------------------------------
    // Viewport
    //--------------------------------------------------------------------------

    for (int i = startRow; i <= endRow; i++) {
        // left margin
        for (int j = 0; j < leftMargin; j++) {
            cout << " ";
        }

        // Tiles
        if (i >= 0 && i < rowSize) {
            cout << setw(3) << setfill('0') << i << "|";
        }
        else {
            cout << "   |";
        }
        for (int j = startCol; j <= endCol; j++) {
            if (validate(i, j)) {
                board[i][j]->print();
            }
            else {
                cout << OUT_OF_BORDER_SHAPE;
            }
        }
        if (i >= 0 && i < rowSize) {
            cout << "|" << setw(3) << setfill('0') << i << endl;
        }
        else {
            cout << "|" << endl;
        }
    }
    
    //--------------------------------------------------------------------------
    // Last Line
    //--------------------------------------------------------------------------
    // left margin
    for (int j = 0; j < leftMargin; j++) {
        cout << " ";
    }

    cout << "   +";
    for (int j = 0; j < viewportColHalfRange*2+1; j++) {
        cout << "-";
    }
    cout << "+" << endl;
    
    //--------------------------------------------------------------------------
    // First Digit
    //--------------------------------------------------------------------------
    
    // left margin
    leftMargin = (screenWidth-(viewportColHalfRange*2+1))/2;
    for (int j = 0; j < leftMargin; j++) {
        cout << " ";
    }
    
    cout << "    ";
    for (int j = startCol; j <= endCol; j++) {
        if (j >= 0 && j < colSize) {
            cout << j / 100;
        }
        else {
            cout << " ";
        }
    }
    cout << endl;
    
    //--------------------------------------------------------------------------
    // Second Digit
    //--------------------------------------------------------------------------
    
    // left margin
    leftMargin = (screenWidth-(viewportColHalfRange*2+1))/2;
    for (int j = 0; j < leftMargin; j++) {
        cout << " ";
    }
    
    cout << "    ";
    for (int j = startCol; j <= endCol; j++) {
        if (j >= 0 && j < colSize) {
            cout << (j / 10) % 10;
        }
        else {
            cout << " ";
        }
    }
    cout << endl;
    
    //--------------------------------------------------------------------------
    // Third Digit
    //--------------------------------------------------------------------------
    
    // left margin
    leftMargin = (screenWidth-(viewportColHalfRange*2+1))/2;
    for (int j = 0; j < leftMargin; j++) {
        cout << " ";
    }
    
    cout << "    ";
    for (int j = startCol; j <= endCol; j++) {
        if (j >= 0 && j < colSize) {
            cout << j % 10;
        }
        else {
            cout << " ";
        }
    }
    cout << endl;
}

bool Board::validate(int row, int col) {
    return row >= 0 && row < rowSize && col >= 0 && col < colSize;
}

int Board::getRowSize() {
	return rowSize;
}

int Board::getColSize() {
	return colSize;
}

Unit *Board::getUnit(int row, int col) {
    if (!validate(row, col)) {
        return NULL;
    }
    
    return board[row][col]->getUnit();
}

bool Board::setUnit(int row, int col, Unit *unit) {
    if (!validate(row, col)) {
        return false;
    }
    
    board[row][col]->setUnit(unit);
    if (unit != NULL) {
        unit->setRow(row);
        unit->setCol(col);
    }
    return true;
}

Prop *Board::getProp(int row, int col) {
    if (!validate(row, col)) {
        return NULL;
    }
    
    return board[row][col]->getProp();
}

bool Board::setProp(int row, int col, Prop *prop) {
    if (!validate(row, col)) {
        return false;
    }
    
    board[row][col]->setProp(prop);
    if (prop != NULL) {
        prop->setRow(row);
        prop->setCol(col);
    }
    return true;
}

Item *Board::getItem(int row, int col) {
    if (!validate(row, col)) {
        return NULL;
    }
    
    return board[row][col]->getItem();
}

void Board::setItem(int row, int col, Item *item) {
    if (!validate(row, col)) {
        return;
    }
    
    board[row][col]->setItem(item);
}

void Board::moveMons() {
    for (int i = 0; i < numMons; i++) {
        // if the current monster died and it does not removed from the board.
        if (mons[i]->isDead() && mons[i]->getRow() != -1) {
            board[mons[i]->getRow()][mons[i]->getCol()]->setUnit(NULL);
            mons[i]->setRow(-1);
            mons[i]->setCol(-1);
            
            // scheduling MonsterRespawnEvent
            int respawnTime = sim->time
                + (rand() % (MONSTER_RESPAWN_INTERVAL_MAX - MONSTER_RESPAWN_INTERVAL_MIN))
                + MONSTER_RESPAWN_INTERVAL_MIN;
            sim->eventQueue->enqueue(new MonsterRespawnEvent(respawnTime, mons[i]));
        }
        
        if (!mons[i]->isDead()) {
            int randDir = rand() % NUM_DIRS;
            mons[i]->move(randDir);
        }
    }
}

void Board::moveBosses() {
    for (int i = 0; i < numBosses; i++) {
        // if the current boss died and it does not removed from the board.
        if (bosses[i]->isDead() && bosses[i]->getRow() != -1) {
            board[bosses[i]->getRow()][bosses[i]->getCol()]->setUnit(NULL);
            bosses[i]->setRow(-1);
            bosses[i]->setCol(-1);
            numBossesLeft--;
        }
        
        if (!bosses[i]->isDead()) {
            int randDir = rand() % NUM_DIRS;
            bosses[i]->move(randDir);
        }
    }
}

Hero *Board::getHero() {
	return hero;
}

// save the printed map of Board.
void Board::saveLevel(ostream &out) {
	out << "# rowSize" << endl;
	out << rowSize << endl;
	out << "# colSize" << endl;
	out << colSize << endl;
	
	for (int i = 0; i < rowSize; i++) {
		for (int j = 0; j < colSize; j++) {
			out << board[i][j]->getShape();
		}
		out << endl;
	}
}

// load edited level map.
void Board::loadLevel(istream &in) {
	char buf[MAX_LEN_BUF];
	
	destroyBoard(); // clear this object.
	
	//out << "# rowSize" << endl;
	in.getline(buf, MAX_LEN_BUF); // skip a line
	//out << rowSize << endl;
	in >> rowSize;
	in.get(); // skip a char(new line = '\n')
	
	//out << "# colSize" << endl;
	in.getline(buf, MAX_LEN_BUF); // skip a line
	//out << colSize << endl;
	in >> colSize;
	in.get(); // skip a char(new line = '\n')

	initBoard(rowSize, colSize);
	
	// init member variables
	hero = NULL;
	
	maxNumMons = 0;
	maxNumBosses = 0;
	maxNumUnits = 0;
	
	for (int i = 0; i < rowSize; i++) {
		for (int j = 0; j < colSize; j++) {
			board[i][j]->loadLevel(in);
			
			// set row and col for props
			if (board[i][j]->getProp() != NULL) {
				board[i][j]->getProp()->setRow(i);
				board[i][j]->getProp()->setCol(j);
			}
			
			// get the hero
			else if (board[i][j]->getUnit() != NULL &&
				board[i][j]->getUnit()->getUnitID() == UNIT_ID_HERO) {
				hero = (Hero *)board[i][j]->getUnit();
				hero->setRow(i);
				hero->setCol(j);
			}

			// count the number of monsters
			else if (board[i][j]->getUnit() != NULL &&
				board[i][j]->getUnit()->getUnitID() == UNIT_ID_MONSTER) {
				maxNumMons++;
			}
			
			// count the number of bosses
			else if (board[i][j]->getUnit() != NULL &&
				board[i][j]->getUnit()->getUnitID() == UNIT_ID_BOSS) {
				maxNumBosses++;
			}
			
			// count the merchants
			else if (board[i][j]->getUnit() != NULL &&
				board[i][j]->getUnit()->getUnitID() == UNIT_ID_MERCHANT) {
				maxNumUnits++;
			}
		}
		in.get(); // skip a char(new line = '\n')
		cout << endl;

	}
	
	numMons = 0;
	mons = new Monster *[maxNumMons];
	
	numBosses = 0;
	bosses = new Boss *[maxNumBosses];

	numUnits = 0;
	units = new Unit *[maxNumUnits];

	for (int i = 0; i < rowSize; i++) {
		for (int j = 0; j < colSize; j++) {
			// collect the monsters
			if (board[i][j]->getUnit() != NULL &&
				board[i][j]->getUnit()->getUnitID() == UNIT_ID_MONSTER) {
				mons[numMons] = (Monster *)board[i][j]->getUnit();
				mons[numMons]->setRow(i);
				mons[numMons]->setCol(j);
				numMons++;
			}
			
			// collect the bosses
			if (board[i][j]->getUnit() != NULL &&
				board[i][j]->getUnit()->getUnitID() == UNIT_ID_BOSS) {
				bosses[numBosses] = (Boss *)board[i][j]->getUnit();
				bosses[numBosses]->setRow(i);
				bosses[numBosses]->setCol(j);
				numBosses++;
			}
			
			// collect the merchants
			if (board[i][j]->getUnit() != NULL &&
				board[i][j]->getUnit()->getUnitID() == UNIT_ID_MERCHANT) {
				units[numUnits] = board[i][j]->getUnit();
				units[numUnits]->setRow(i);
				units[numUnits]->setCol(j);
				numUnits++;
			}
		}
	}
}

void Board::save(ostream &out) {
    out << "#---------------------------------------- class Board" << endl;
    
    out << "#&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& board[][]" << endl;
    out << "# rowSize" << endl;
    out << rowSize << endl;
    out << "# colSize" << endl;
    out << colSize << endl;
    out << "#================================================== board[][] begin" << endl;
    for (int i = 0; i < rowSize; i++) {
        for (int j = 0; j < colSize; j++) {
            out << "#================================================== board[" << i << "][" << j << "]" << endl;
            board[i][j]->save(out);
        }
    }
    out << "#================================================== board[][] end" << endl;
    
    out << "#&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& mons[]" << endl;
    out << "# maxNumMons" << endl;
    out << maxNumMons << endl;
    out << "# numMons" << endl;
    out << numMons << endl;
    out << "#================================================== mons[] begin" << endl;
    for (int i = 0; i < maxNumMons; i++) {
        out << "#================================================== mons[" << i << "]" << endl;
        mons[i]->save(out);
    }
    out << "#================================================== mons[] end" << endl;
    
    out << "#&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& bosses[]" << endl;
    out << "# maxNumBosses" << endl;
    out << maxNumBosses << endl;
    out << "# numBosses" << endl;
    out << numBosses << endl;
    out << "#================================================== bosses[] begin" << endl;
    for (int i = 0; i < maxNumBosses; i++) {
        out << "#================================================== bosses[" << i << "]" << endl;
        bosses[i]->save(out);
    }
    out << "#================================================== bosses[] end" << endl;

	out << "#&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& units[]" << endl;
	out << "# maxNumUnits" << endl;
	out << maxNumUnits << endl;
	out << "# numUnits" << endl;
	out << numUnits << endl;
	out << "#================================================== units[] begin" << endl;
	for (int i = 0; i < maxNumUnits; i++) {
		out << "#================================================== units[" << i << "]" << endl;
		out << "# Unit class type" << endl;
		out << units[i]->getUnitID() << endl;

		units[i]->save(out);
	}
	out << "#================================================== units[] end" << endl;
}

void Board::load(istream &in) {
    char buf[MAX_LEN_BUF];
    
    destroyBoard(); // clear this object.
    
    //out << "#---------------------------------------- class Board" << endl;
    in.getline(buf, MAX_LEN_BUF); // skip a line

    //out << "#&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& board[][]" << endl;
    in.getline(buf, MAX_LEN_BUF); // skip a line

    //out << "# rowSize" << endl;
    in.getline(buf, MAX_LEN_BUF); // skip a line
    //out << rowSize << endl;
    in >> rowSize;
    in.get(); // skip a char(new line = '\n')
    
    //out << "# colSize" << endl;
    in.getline(buf, MAX_LEN_BUF); // skip a line
    //out << colSize << endl;
    in >> colSize;
    in.get(); // skip a char(new line = '\n')

    initBoard(rowSize, colSize);
    
    //out << "#================================================== board[][] begin" << endl;
    in.getline(buf, MAX_LEN_BUF); // skip a line
    for (int i = 0; i < rowSize; i++) {
        for (int j = 0; j < colSize; j++) {
            //out << "#================================================== board[" << i << "][" << j << "]" << endl;
            in.getline(buf, MAX_LEN_BUF); // skip a line
            
            //board[i][j]->save(out);
            board[i][j]->load(in);
        }
    }
    //out << "#================================================== board[][] end" << endl;
    in.getline(buf, MAX_LEN_BUF); // skip a line
    
    //out << "#&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& mons[]" << endl;
    in.getline(buf, MAX_LEN_BUF); // skip a line
    
    //out << "# maxNumMons" << endl;
    in.getline(buf, MAX_LEN_BUF); // skip a line
    //out << maxNumMons << endl;
    in >> maxNumMons;
    in.get(); // skip a char(new line = '\n')
    
    //out << "# numMons" << endl;
    in.getline(buf, MAX_LEN_BUF); // skip a line
    //out << numMons << endl;
    in >> numMons;
    in.get(); // skip a char(new line = '\n')

    mons = new Monster *[maxNumMons];
    
    //out << "#================================================== mons[] begin" << endl;
    in.getline(buf, MAX_LEN_BUF); // skip a line
    for (int i = 0; i < maxNumMons; i++) {
        //out << "#================================================== mons[" << i << "]" << endl;
        in.getline(buf, MAX_LEN_BUF); // skip a line
        
        mons[i] = new Monster();
        
        //mons[i]->save(out);
        mons[i]->load(in);
        
        // put the monster on board when the monster is not dead.
        if (!mons[i]->isDead()) {
            board[mons[i]->getRow()][mons[i]->getCol()]->setUnit(mons[i]);
        }
    }
    //out << "#================================================== mons[] end" << endl;
    in.getline(buf, MAX_LEN_BUF); // skip a line

    //out << "#&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& bosses[]" << endl;
    in.getline(buf, MAX_LEN_BUF); // skip a line
    
    //out << "# maxNumBosses" << endl;
    in.getline(buf, MAX_LEN_BUF); // skip a line
    //out << maxNumBosses << endl;
    in >> maxNumBosses;
    in.get(); // skip a char(new line = '\n')
    
    //out << "# numBosses" << endl;
    in.getline(buf, MAX_LEN_BUF); // skip a line
    //out << numBosses << endl;
    in >> numBosses;
    in.get(); // skip a char(new line = '\n')
    
    bosses = new Boss *[maxNumBosses];
    
    //out << "#================================================== bosses[] begin" << endl;
    in.getline(buf, MAX_LEN_BUF); // skip a line
    for (int i = 0; i < maxNumBosses; i++) {
        //out << "#================================================== bosses[" << i << "]" << endl;
        in.getline(buf, MAX_LEN_BUF); // skip a line
        
        bosses[i] = new Boss();
        
        //bosses[i]->save(out);
        bosses[i]->load(in);
        
        // put the boss on board when the boss is not dead.
        if (!bosses[i]->isDead()) {
            board[bosses[i]->getRow()][bosses[i]->getCol()]->setUnit(bosses[i]);
        }
    }
    //out << "#================================================== bosses[] end" << endl;
    in.getline(buf, MAX_LEN_BUF); // skip a line
	
	//out << "#&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& units[]" << endl;
	in.getline(buf, MAX_LEN_BUF); // skip a line
	
	//out << "# maxNumUnits" << endl;
	in.getline(buf, MAX_LEN_BUF); // skip a line
	//out << maxNumUnits << endl;
	in >> maxNumUnits;
	in.get(); // skip a char(new line = '\n')
	
	//out << "# numUnits" << endl;
	in.getline(buf, MAX_LEN_BUF); // skip a line
	//out << numUnits << endl;
	in >> numUnits;
	in.get(); // skip a char(new line = '\n')
	
	units = new Unit *[maxNumUnits];
	
	//out << "#================================================== units[] begin" << endl;
	in.getline(buf, MAX_LEN_BUF); // skip a line
	for (int i = 0; i < maxNumUnits; i++) {
		//out << "#================================================== units[" << i << "]" << endl;
		in.getline(buf, MAX_LEN_BUF); // skip a line
		
		//out << "# Unit class type" << endl;
		in.getline(buf, MAX_LEN_BUF); // skip a line
		
		//out << slots[i]->getUnitID() << endl;
		string unitID = "";
		in >> unitID;
		in.get(); // skip a char(new line = '\n')
		
		units[i] = createUnitByID(unitID); // create an object using factory.
		
		//units[i]->save(out);
		units[i]->load(in);
		
		// put the unit on board when the unit is not dead.
		if (!units[i]->isDead()) {
			board[units[i]->getRow()][units[i]->getCol()]->setUnit(units[i]);
		}
	}
	//out << "#================================================== units[] end" << endl;
	in.getline(buf, MAX_LEN_BUF); // skip a line
}
