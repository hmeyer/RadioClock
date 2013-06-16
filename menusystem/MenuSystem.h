/*
  MenuSystem.h - Library for creating menu structures.
  Created by Jon Black, August 8th 2011.
  Released into the public domain.

  License: LGPL 3
*/

#include <inttypes.h>

#ifndef MENUSYSTEM_H
#define MENUSYSTEM_H


#define MAX_MENU_ITEMS 5

namespace menu {

class Component
{
public:
    Component(char* name);
    char* get_name() const;
    virtual Component *select() = 0;
protected:
    char* _name;
};


class Item : public Component
{
public:
    Item(char* name);
    virtual Component *select()=0;
};

class TextItem : public Item
{
public:
    TextItem(char* name);
    virtual Component *select();
};


class Menu : public Component
{
public:
    Menu(char* name);

    bool next(bool loop=false);
    bool prev(bool loop=false);
    Component *activate();
    virtual Component *select();

    void add_item(Item* pItem);
    Menu const* add_menu(Menu* pMenu);

    void set_parent(Menu* pParent);
    Menu const* get_parent() const;

    Component const *get_selected() const;
    Component const* get_menu_component(uint8_t index) const;

    uint8_t get_num_menu_components() const;
    uint8_t get_cur_menu_component_num() const;

private:
    Component* _p_sel_menu_component;
    Component* _menu_components[MAX_MENU_ITEMS];
    Menu* _p_parent;
    uint8_t _num_menu_components;
    uint8_t _cur_menu_component_num;
};


class MenuSystem
{
public:
    MenuSystem();

    bool next(bool loop=false);
    bool prev(bool loop=false);
    void select();
    bool back();

    void set_root_menu(Menu* p_root_menu);

    Menu const* get_current_menu() const;

private:
    Menu* _p_root_menu;
    Menu* _p_curr_menu;
};

}


#endif
