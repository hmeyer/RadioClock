#ifndef MENUSYSTEM_H
#define MENUSYSTEM_H

#include <inttypes.h>
#include "switch.h"

class Menu;

class Item
{
public:
    Item(char *n);
    virtual Item *interact(sw::Button b) = 0;
    virtual void repaint() = 0;
    void addSibling(Item &s);
protected:
    char *name;
    Item *parent;
    Item *prev;
    Item *next;
    friend class Menu;
};

class Menu: public Item
{
public:
	Menu(char *name);
	void addChild(Item &c);
    	virtual Item *interact(sw::Button b);
	virtual void repaint();
protected:
	Item *currentChild;
};





#endif
