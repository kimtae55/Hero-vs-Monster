#ifndef BOARD_H
#define BOARD_H

class Unit;
class Hero;
class Monster;
class Boss;
class Merchant;
class Item;
class Prop;
class Tile;

class Board {
private:
public: // debugging purpose
    int rowSize;
    int colSize;
    Tile ***board;
    
    Monster **mons;
    int maxNumMons;
    int numMons;
    
    Boss **bosses;
    int maxNumBosses;
    int numBosses;
    
    Unit **units;
	int maxNumUnits;
	int numUnits;

	// * CAUTION: this hero must not be saved. this will be used by loadLevel().
	Hero *hero;

    // private helper function.
    void initBoard(int rowSize, int colSize);
    void destroyBoard();

    void initMons(int maxNumMons);
    void initBosses(int maxNumBosses);
	void initUnits(int maxNumUnits);
	
    void initProps(int maxNumTrees, int maxNumPortals);
    void initItems(int maxNumPotions, int maxNumKeys, int maxNumArmors, int maxNumWeapons);

public:
    Board();
    Board(int rowSize, int colSize, int numTrees, int numPortals, int numPotions, int numKeys,
		  int maxNumArmors, int numWeapons, int numMons, int maxNumBosses, int maxNumMerchants,
          int heroRow, int heroCol);

    virtual ~Board();

    void print();
    void printViewport(int centerRow, int centerCol,
					   int viewportRowHalfRange, int viewportColHalfRange,
					   int screenWidth);
	
	int getRowSize();
	int getColSize();
	
    Unit *getUnit(int row, int col);
    bool setUnit(int row, int col, Unit *unit);

    Prop *getProp(int row, int col);
    bool setProp(int row, int col, Prop *prop);

    Item *getItem(int row, int col);
    void setItem(int row, int col, Item *item);
    
    bool validate(int row, int col);
    
    void moveMons();
    void moveBosses();
	
	// * CAUTION: This function returns the new Hero object which is created by loadLevel()
	Hero *getHero();
	
	// save the printed map of Board.
    void saveLevel(ostream &out);
	// load edited level map.
    void loadLevel(istream &in);

	// save/load the current snapshot of Board object
	virtual void save(ostream &out);
	virtual void load(istream &in);
};

#endif
