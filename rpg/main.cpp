#include <iostream>
#include <fstream>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <string>
#include <cmath>

using namespace std;

#include "main.h"
#include "unit.h"
#include "hero.h"
#include "item.h"
#include "prop.h"
#include "tile.h"
#include "inventory.h"
#include "paperdoll.h"
#include "board.h"
#include "sim.h"

#ifdef VISUAL_STUDIO
#include <windows.h>
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

int _kbhit(void)
{
    struct termios oldt, newt;
    int ch;
    int oldf;
    
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
    
    ch = getchar();
    
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);
    
    if(ch != EOF)
    {
        ungetc(ch, stdin);
        return 1;
    }
    
    return 0;
}
#endif

void handleUserCommands();
void processHeroMove(char command);
void processHeroUseSlots(char command);
void processMoveItemSlotsToBackpack(char command);
void processMoveItemBackpackToSlots(char command);
void processHeroUnequip(char command);
void processSaveAndLoad(char command);
void processSaveLevelAndLoadLevel(char command);

//------------------------------------------------------------------------------
// Global Variable
//------------------------------------------------------------------------------
Sim *sim; // design pattern: singleton

extern int numBossesLeft; // debugging purpose

#include "event.h"
#include "eventqueue.h"

//==============================================================================
// main()
//==============================================================================
#include "backpack.h"
int main() {
    // 안녕! 태효!
	// Bye!
    srand((unsigned)time(NULL));
    
    int screenWidth = SCREEN_WIDTH;
    int viewportRowHalfRange = VIEWPORT_ROW_HALF_RANGE;
    int viewportColHalfRange = VIEWPORT_COL_HALF_RANGE;
    
    // rpg ver0051: 새로 시뮬레이션에서 만들어진 부분:
    //    - Global Variable인 sim에 heap object를 만들어줌.
    //      이렇게 해서 만들어진 sim을 모든 .cpp들에서 "extern Board *board;"를 이용해서
    //      사용할 수 있다.
    sim = new Sim(); // 이 과정에서 이미 Global Variable인 sim에 새 Sim object의
                     // 주소가 주어진다. 그리고 아래의 init을 진행해야만 된다.
    sim->init(HUGE_MAP_ROW_SIZE, HUGE_MAP_COL_SIZE,
              HUGE_MAP_NUM_TREES, HUGE_MAP_NUM_PORTALS, HUGE_MAP_NUM_POTIONS,
              HUGE_MAP_NUM_KEYS, HUGE_MAP_NUM_ARMORS,
              HUGE_MAP_NUM_WEAPONS, HUGE_MAP_NUM_MONS,
              HUGE_MAP_NUM_BOSSES, HUGE_MAP_NUM_MERCHANTS,
              DEFAULT_ROW_SIZE/2, DEFAULT_COL_SIZE/2);

    sim->board->printViewport(sim->hero->getRow(), sim->hero->getCol(),
                    viewportRowHalfRange, viewportColHalfRange, screenWidth);
    
    while (true) {
        //----------------------------------------------------------------------
        //----------------------------------------------------------------------
        //----------------------------------------------------------------------
        // rpg ver0051: 새로 시뮬레이션에서 만들어진 부분:
        //    - 시간이 지나거나 다 된 event들을 모두 꺼내어 처리한다.
        //    - while loop의 condition statement를 보면, peek()을 사용하여,
        //      queue로부터 event를 꺼내지 않고, head element(dequeue()를 하면 꺼내질려고
        //      대기중인 element)를 체크한다.
        //    - trigger()펑션을 그냥 그 event가 할 일을 시키는 것이다.

        // handle the current event
        while (sim->eventQueue->size() > 0 &&
               sim->eventQueue->peek()->getTime() <= sim->time) {
            Event *curEvent = sim->eventQueue->dequeue();
            curEvent->trigger();
        }
        //----------------------------------------------------------------------
        //----------------------------------------------------------------------
        //----------------------------------------------------------------------
        
		handleUserCommands();
		
		if (sim->hero->hasEscaped()) {
			cout << "You have found the exit!!" << endl;
			break;
		}

        if (sim->time % 10 == 0) {
            sim->board->moveMons();
        }
        if (sim->time % 2 == 0) {
            sim->board->moveBosses();
        }
		
        sim->board->printViewport(sim->hero->getRow(), sim->hero->getCol(),
                                  viewportRowHalfRange, viewportColHalfRange, screenWidth);
        sim->hero->printStat();
        cout << endl;
        cout << "numBossesLeft=" << numBossesLeft; // debugging purpose
        cout << endl;
		
        // check whether the hero died or not.
        if (sim->hero->isDead()) {
            cout << "You died!! Game Over!!" << endl;
            break;
        }
        
        // give computer player a bit of random duration of time to think.
#ifdef VISUAL_STUDIO
        Sleep(DELAY_MILLI_SECOND); // 1 sec = 1,000
		system("cls");
#else
        usleep(DELAY_MILLI_SECOND*1000); // 1 sec = 1,000,000
#endif
        sim->time++;
    }
    
    cout << "Bye!!" << endl;

    delete sim;

    return 0;
}

//==============================================================================
// User Defined Functions
//==============================================================================
void handleUserCommands() {
	string filename = "";
	
	char command = '\0';
	
	if (_kbhit()) {
#ifdef VISUAL_STUDIO
		command = _getch();
#else
		command = getchar();
#endif
		cout << endl;
		
		processHeroMove(command);
		processHeroUseSlots(command);
		processMoveItemSlotsToBackpack(command);
		processMoveItemBackpackToSlots(command);
		processHeroUnequip(command);
		processSaveAndLoad(command);
		processSaveLevelAndLoadLevel(command);
	}
}

void processHeroMove(char command) {
	int heroDir = -1;

	// process hero movement.
	if (command == DIR_NORTH_CHAR) {
		heroDir = DIR_NORTH;
	}
	else if (command == DIR_EAST_CHAR) {
		heroDir = DIR_EAST;
	}
	else if (command == DIR_SOUTH_CHAR) {
		heroDir = DIR_SOUTH;
	}
	else if (command == DIR_WEST_CHAR) {
		heroDir = DIR_WEST;
	}
	
	if (heroDir != -1) {
		sim->hero->move(heroDir);
	}
}

void processHeroUseSlots(char command) {
	if (command >= '1' && command <= '5') {
		int indexSlot = command - '1';
		sim->hero->useItem(indexSlot);
	}
}

void processMoveItemSlotsToBackpack(char command) {
	if (command == '.') {
		cout << "Enter the index of the item in slots : ";
		int indexSlot;
		cin >> indexSlot;
		
		indexSlot--; // convert human index to computer index.
		
		sim->hero->moveItemSlotsToBackpack(indexSlot);
	}
}

void processMoveItemBackpackToSlots(char command) {
	if (command == ',') {
		cout << "Enter the index of the item in backpack : ";
		int indexBackpack;
		cin >> indexBackpack;
		
		indexBackpack--; // convert human index to computer index.
		
		sim->hero->moveItemBackpackToSlots(indexBackpack);
	}
}

void processHeroUnequip(char command) {
	// when the user wants to unequip.
	if (command == 'u') {
		int bodyPartID = -1;
		cout << "Enter the body part ID(1-8) : ";
		cin >> bodyPartID;
		sim->hero->printStat();
		sim->hero->unequip(bodyPartID-1);
	}
}

void processSaveAndLoad(char command) {
	string filename = "";
	
	if (command == 'l' || command == 'L') { // Load
		cout << "Load: filename? : ";
		cin >> filename;
		ifstream fin;
		fin.open(filename.c_str());
		
		if (fin.fail()) {
			cout << "error: loading error" << endl;
		}
		else {
			sim->hero->load(fin);
			sim->board->load(fin);
			sim->board->setUnit(sim->hero->getRow(), sim->hero->getCol(), sim->hero);
		}
		
		fin.close();
	}
	if (command == 'o' || command == 'O') { // Save
		cout << "Save: filename? : ";
		cin >> filename;
		
		ofstream fout;
		fout.open(filename.c_str());
		
		if (fout.fail()) {
			cout << "error: saving error" << endl;
		}
		else {
			sim->hero->save(fout);
			sim->board->save(fout);
		}
		
		fout.close();
	}
}

void processSaveLevelAndLoadLevel(char command) {
	string filename = "";

	if (command == ';' || command == ':') { // Load
		cout << "Load Level: filename? : ";
		cin >> filename;
		ifstream fin;
		fin.open(filename.c_str());
		
		if (fin.fail()) {
			cout << "error: loading error" << endl;
		}
		else {
			sim->board->loadLevel(fin);
			// the previous hero removed from heap memory.
			sim->hero = sim->board->getHero(); // get the new hero loaded from the level.
		}
		fin.close();
	}
	if (command == 'p' || command == 'P') { // Save
		cout << "Save Level: filename? : ";
		cin >> filename;
		
		ofstream fout;
		fout.open(filename.c_str());
		
		if (fout.fail()) {
			cout << "error: saving error" << endl;
		}
		else {
			sim->board->saveLevel(fout);
		}
		
		fout.close();
	}
}
