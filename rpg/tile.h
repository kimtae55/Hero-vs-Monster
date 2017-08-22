#ifndef TILE_H
#define TILE_H

class Unit;
class Item;
class Prop;

class Tile {
private:
    Unit *unit;
    Item *item;
    Prop *prop;
    
public:
    Tile();
    
    void print();
	char getShape();
	
    Unit *getUnit();
    void setUnit(Unit *unit);
    Item *getItem();
    void setItem(Item *item);
    Prop *getProp();
    void setProp(Prop *prop);
	
	// load edited level map.
	void loadLevel(istream &in);
	
    virtual void save(ostream &out);
    virtual void load(istream &in);
};

#endif
