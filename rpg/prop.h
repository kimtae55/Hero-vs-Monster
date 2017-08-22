#ifndef PROP_H
#define PROP_H

class Item;
class Unit;
class Board;

//==============================================================================
// 1st Generation
//==============================================================================

class Prop {
protected:
    char shape;
    bool disposed;
    
    int row;
    int col;

public:
    Prop();
    Prop(char shape);

    virtual ~Prop();

    virtual string getPropID() = 0;
    
    void print();

	char getShape();

    int getRow();
    void setRow(int row);
    int getCol();
    void setCol(int col);

    virtual bool isFunctionable();
	virtual bool isClimbable();
	virtual bool isExit();
    virtual bool interact(Unit *otherUnit);
    
    bool hasBeenDisposed();
    
    virtual void save(ostream &out);
    virtual void load(istream &in);
};

Prop *createPropByID(string propID);

//==============================================================================
// 2nd Generation
//==============================================================================

class NonFunctionableProp : public Prop {
protected:
public:
    NonFunctionableProp();
    NonFunctionableProp(char shape);
    
    virtual ~NonFunctionableProp();

    virtual void save(ostream &out);
    virtual void load(istream &in);
};

class FunctionableProp : public Prop {
protected:
public:
    FunctionableProp();
    FunctionableProp(char shape);
    
    virtual ~FunctionableProp();
    
    bool isFunctionable();
    
    virtual void save(ostream &out);
    virtual void load(istream &in);
};

//==============================================================================
// 3rd Generation
//==============================================================================

class Tree : public NonFunctionableProp {
protected:
public:
    Tree();
    Tree(char shape);
    
    virtual ~Tree();
    
    virtual string getPropID();
    
    virtual void save(ostream &out);
    virtual void load(istream &in);
};

class LockedChest : public FunctionableProp {
protected:
    Item *item;
public:
    LockedChest();
    LockedChest(Item *item);
    LockedChest(char shape, Item *item);
    
    virtual ~LockedChest();
    
    virtual string getPropID();

    virtual bool interact(Unit *otherUnit);
    
    virtual void save(ostream &out);
    virtual void load(istream &in);
};

class Wall : public NonFunctionableProp {
protected:
	int wallType;
public:
	Wall();
	Wall(int wallType);
	
	virtual ~Wall();
	
	virtual string getPropID();
	int getWallType();
	
	virtual void save(ostream &out);
	virtual void load(istream &in);
};

class Fountain : public FunctionableProp {
protected:
	int hp;
	int mp;
	
public:
	Fountain();
	Fountain(int hp, int mp);
	Fountain(char shape, int hp, int mp);
	
	virtual ~Fountain();
	
	virtual string getPropID();
	virtual bool interact(Unit *otherUnit);
	
	virtual void save(ostream &out);
	virtual void load(istream &in);
};

class Portal : public FunctionableProp {
protected:
public:
	Portal();
	Portal(char shape);
	
	virtual ~Portal();
	
	virtual bool isClimbable();

	virtual string getPropID();
	virtual bool interact(Unit *otherUnit);
	
	virtual void save(ostream &out);
	virtual void load(istream &in);
};

class Exit : public NonFunctionableProp {
protected:
public:
	Exit();
	Exit(char shape);
	
	virtual ~Exit();
	
	virtual bool isClimbable();
	
	bool isExit();
	
	virtual string getPropID();
	
	virtual void save(ostream &out);
	virtual void load(istream &in);
};

#endif
