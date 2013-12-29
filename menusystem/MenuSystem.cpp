/*
  MenuSystem.h - Library for creating menu structures.
  Created by Jon Black, August 8th 2011.
  Released into the public domain.

  License: LGPL 3
*/

#include "MenuSystem.h"
#include "display.h"

extern "C" {
	void __cxa_pure_virtual() { }
}

Item::Item(char *n): name(n), parent(this), prev(this), next(this) {}
Menu::Menu(char *n): Item(n), currentChild(this) {}

Item *Menu::interact(sw::Button b) {
	switch(b) {
		case sw::up: currentChild = currentChild->prev; break;
		case sw::down: currentChild = currentChild->next; break;
		case sw::right: return currentChild;
		case sw::center: return currentChild;
		case sw::left: return parent;
	}
	return this;
}

void Menu::repaint() {
	DISP.drawString(8, currentChild->name);
}

void Menu::addChild(Item &c) {
	if (currentChild == this) {
		currentChild = &c;
		c.parent = this;
	}
	else currentChild->addSibling(c);
}

void Item::addSibling(Item &s) {
	if (next == this) {
		next = &s;
		s.prev = this;
		s.parent = parent;
	}
	else next->addSibling(s);
}

