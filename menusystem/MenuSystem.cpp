/*
  MenuSystem.h - Library for creating menu structures.
  Created by Jon Black, August 8th 2011.
  Released into the public domain.

  License: LGPL 3
*/

#include "MenuSystem.h"

extern "C" {
	void __cxa_pure_virtual() { }
}

namespace menu {

// *********************************************************
// MenuComponent
// *********************************************************

Component::Component(char* name) : _name(name) { }

char *Component::get_name() const
{
    return _name;
}

// *********************************************************
// Menu
// *********************************************************

Menu::Menu(char* name)
: Component(name),
  _p_sel_menu_component(0),
  _p_parent(0),
  _num_menu_components(0),
  _cur_menu_component_num(0)
{
}

bool Menu::next(bool loop)
{
    if (_cur_menu_component_num != _num_menu_components - 1)
    {
        _cur_menu_component_num++;
        _p_sel_menu_component = _menu_components[_cur_menu_component_num];

        return true;
    } else if (loop)
    {
        _cur_menu_component_num = 0;
        _p_sel_menu_component = _menu_components[_cur_menu_component_num];

        return true;
    }

    return false;
}

bool Menu::prev(bool loop)
{
    if (_cur_menu_component_num != 0)
    {
        _cur_menu_component_num--;
        _p_sel_menu_component = _menu_components[_cur_menu_component_num];

        return true;
    } else if (loop)
    {
        _cur_menu_component_num = _num_menu_components - 1;
        _p_sel_menu_component = _menu_components[_cur_menu_component_num];

        return true;
    }

    return false;
}

Component *Menu::activate()
{
    Component* pComponent = _menu_components[_cur_menu_component_num];

    if (pComponent == 0)
        return 0;

    return pComponent->select();
}

Component *Menu::select()
{ return this; }

void Menu::add_item(Item* pItem)
{
    // Prevent memory overrun
    if (_num_menu_components == MAX_MENU_ITEMS)
        return;

    _menu_components[_num_menu_components] = pItem;

    if (_num_menu_components == 0)
        _p_sel_menu_component = pItem;

    _num_menu_components++;
}

Menu const* Menu::get_parent() const
{
    return _p_parent;
}

void Menu::set_parent(Menu* pParent)
{
    _p_parent = pParent;
}

Menu const* Menu::add_menu(Menu* pMenu)
{
    pMenu->set_parent(this);

    _menu_components[_num_menu_components] = pMenu;

    if (_num_menu_components == 0)
        _p_sel_menu_component = pMenu;

    _num_menu_components++;

    return pMenu;
}

Component const* Menu::get_menu_component(uint8_t index) const
{
  return _menu_components[index];
}

Component const* Menu::get_selected() const
{
    return _p_sel_menu_component;
}

uint8_t Menu::get_num_menu_components() const
{
    return _num_menu_components;
}

uint8_t Menu::get_cur_menu_component_num() const
{
    return _cur_menu_component_num;
}

// *********************************************************
// Item
// *********************************************************

Item::Item(char* name) : Component(name) { }

// *********************************************************
// TextItem
// *********************************************************

TextItem::TextItem(char* name) : Item(name) { }
Component *TextItem::select() {
//	diplayText( _name );
	return 0;
}

// *********************************************************
// MenuSystem
// *********************************************************

MenuSystem::MenuSystem()
: _p_root_menu(0),
  _p_curr_menu(0)
{
}

bool MenuSystem::next(bool loop)
{
    return _p_curr_menu->next(loop);
}

bool MenuSystem::prev(bool loop)
{
    return _p_curr_menu->prev(loop);
}

void MenuSystem::select()
{
    Component* pComponent = _p_curr_menu->activate();

    if (pComponent != 0)
    {
        _p_curr_menu = (Menu*) pComponent;
    }
    else
    {
        // A menu item was selected, so reset the menu ready for when
        // it's used again.
        _p_curr_menu = _p_root_menu;
    }
}

bool MenuSystem::back()
{
    if (_p_curr_menu != _p_root_menu)
    {
        _p_curr_menu = const_cast<Menu*>(_p_curr_menu->get_parent());
        return true;
    }

    // We are already in the root menu
    return false;
}

void MenuSystem::set_root_menu(Menu* p_root_menu)
{
    _p_root_menu = p_root_menu;
    _p_curr_menu = p_root_menu;
}

Menu const* MenuSystem::get_current_menu() const
{
  return _p_curr_menu;
}
}
