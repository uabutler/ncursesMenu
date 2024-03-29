
/* Last Modified: 12/18/2019 */
/* Version: 1.3              */

#ifndef MENU_H
#define MENU_H

#define HA A_BOLD | A_UNDERLINE
#define IA A_NORMAL 
#define SA A_STANDOUT

#define VS 2

#include <vector>
#include <string>
#include <cstring>
#include <ncurses.h>

using namespace std;

/******************************************\
  MENU 
 ======
 This class represents the menu. It
 includes methods to print the menu and
 interact with it.
\******************************************/

template <typename T>
class Menu
{
private:
  vector< pair<string, T> > items;

  string header;
  bool hasHeader;

  int height;
  int width;

  int row;
  int col;

  size_t current;

  bool vertical;

  unsigned headerAttr;
  unsigned itemAttr;
  unsigned selectedAttr;

  void changeSelectionHelper(int dif);
  void paddedPrint(int row, int col, string str, size_t len);

public:
  Menu(unsigned ha = HA, unsigned ia = IA, unsigned sa = SA);
  Menu(string h, unsigned ha = HA, unsigned ia = IA, unsigned sa = SA);
  Menu(bool v, unsigned ha = HA, unsigned ia = IA, unsigned sa = SA);
  Menu(string h, bool v, unsigned ha = HA, unsigned ia = IA, unsigned sa = SA);
  Menu(const char* h, unsigned ha = HA, unsigned ia = IA, unsigned sa = SA);
  Menu(const char* h, bool v, unsigned ha = HA, unsigned ia = IA, unsigned sa = SA);

  void addItem(string name, T value);
  
  int getHeight();
  int getWidth();
  void getyx(int& h, int& w);

  bool isVertical();

  void print(int r, int c);
  
  size_t getLoc();

  bool prev();
  bool next();
  
  T getCurrent(); 

  void isSelected(bool flag);
};


// This is used to print using ncurses, and right-pad the string with
// spaces for the specified length
template <typename T>
void Menu<T>::paddedPrint(int row, int col, string str, size_t len)
{
  mvprintw(row, col, "%s", str.c_str());
  for(size_t i = 0; i < (len - str.length()); i++)
    mvaddch(row, col + str.length() + i, ' ');
}

// Constructors for the menu. They can take in the attricuted the
// items sould be printed with (which have default values). You can
// also pass an optional header, and boolean that tells the program
// if the menu should be printed horizontally or vertically
template <typename T>
Menu<T>::Menu(unsigned ha, unsigned ia, unsigned sa):
header(""), hasHeader(false), height(0), width(0), current(0),
vertical(true), headerAttr(ha), itemAttr(ia), selectedAttr(sa) {}

template <typename T>
Menu<T>::Menu(string h, unsigned ha, unsigned ia, unsigned sa):
header(h), hasHeader(true), height(1), width(h.length()), current(0),
vertical(true), headerAttr(ha), itemAttr(ia), selectedAttr(sa) {}

template <typename T>
Menu<T>::Menu(bool v, unsigned ha, unsigned ia, unsigned sa):
header(""), hasHeader(false), height(0), width(0), current(0),
vertical(v), headerAttr(ha), itemAttr(ia), selectedAttr(sa) {}

template <typename T>
Menu<T>::Menu(string h, bool v, unsigned ha, unsigned ia, unsigned sa):
header(h), hasHeader(true), height(1), width(h.length()), current(0),
vertical(v), headerAttr(ha), itemAttr(ia), selectedAttr(sa) {}

template <typename T>
Menu<T>::Menu(const char* h, unsigned ha, unsigned ia, unsigned sa):
header(h), hasHeader(true), height(1), width(strlen(h)), current(0),
vertical(true), headerAttr(ha), itemAttr(ia), selectedAttr(sa) {}

template <typename T>
Menu<T>::Menu(const char* h, bool v, unsigned ha, unsigned ia, unsigned sa):
header(h), hasHeader(true), height(1), width(strlen(h)), current(0),
vertical(v), headerAttr(ha), itemAttr(ia), selectedAttr(sa) {}

// A few getters and setters
template <typename T>
void Menu<T>::addItem(string name, T value)
{
  items.emplace_back(name, value);

  if(vertical)
  {
    height++;
    width = max(width, static_cast<int>(name.length()));
  }
  else
  {
    height = hasHeader ? 2 : 1;
    width += name.length() + VS;
  }
}

template <typename T>
int Menu<T>::getHeight()
{
  return height;
}

template <typename T>
int Menu<T>::getWidth()
{
  return width;
}

template <typename T>
void Menu<T>::getyx(int& h, int& w)
{
  h = height;
  w = width;
}

template <typename T>
bool Menu<T>::isVertical()
{
  return vertical;
}

// This prints the menu on the screen at the given coordinated.
// It then selects the first item on the menu
template <typename T>
void Menu<T>::print(int r, int c)
{
  row = r;
  col = c;

  size_t curRow = row;

  if(hasHeader)
  {
    attrset(headerAttr);
    mvprintw(curRow++, col, "%s", header.c_str());
  }
  
  attrset(selectedAttr);

  int mvrow = curRow;
  if(vertical)
  {
    paddedPrint(curRow++, col, items[0].first, width);
    
    attrset(itemAttr);
    for(size_t curItem = 1; curItem < items.size(); curItem++)
      paddedPrint(curRow++, col, items[curItem].first, width);
  }
  else // if(horizontal)
  {
    size_t curCol = col;

    mvprintw(curRow, curCol, "%s", items[0].first.c_str());

    attrset(itemAttr);
    for(size_t curItem = 1; curItem < items.size(); curItem++)
    {
      curCol += items[curItem - 1].first.length() + VS;
      mvprintw(curRow, curCol, "%s", items[curItem].first.c_str());
    }
  }

  move(mvrow, col);
  refresh();

  attrset(A_NORMAL);
}

// This returns the index of the index of the item that is currently
// selected
template <typename T>
size_t Menu<T>::getLoc()
{
  return current;
}

// These next three functions select the next or previous item. If
// this can be done, the selection is changed and true is returned.
// Otherwise, the selection stays the same and we return false.

// This function specifically assumes the change can be made
template <typename T>
void Menu<T>::changeSelectionHelper(int dif)
{
  if(vertical)
  {
    attrset(itemAttr);
    paddedPrint(row + current + ((hasHeader ? 1 : 0)),
      col, items[current].first, width);

    current += dif;

    attrset(selectedAttr);
    paddedPrint(row + current + ((hasHeader ? 1 : 0)),
      col, items[current].first, width);

    move(row + current + (hasHeader ? 1 : 0), col);
  }
  else // if(horizontal)
  {
    int offset = 0;
    for(int i = 0; i < (int)current; i++)
      offset += items[i].first.length() + VS;

    attrset(itemAttr);
    mvprintw(row + (hasHeader ? 1 : 0), col + offset,
      "%s", items[current].first.c_str());

    // 0 if zero, -1 if negative, 1 if positive
    int inc = (dif > 0) ? 1 : -1;
    inc *= dif ? 1 : 0;

    if(inc == 1)
    { 
      for(int i = current; i < (int)current + dif; i += inc)
        offset += items[i].first.length() + VS;
    }
    else if(inc == -1)
    {
      for(int i = (int)current - 1; i >= (int)current + dif; i += inc)
        offset -= items[i].first.length() + VS;
    }

    attrset(selectedAttr);
    mvprintw(row + (hasHeader ? 1 : 0), col + offset,
      "%s", items[current += dif].first.c_str());

    move(row + (hasHeader ? 1 : 0), col + offset);
  }

  refresh();
  attrset(A_NORMAL);
}

template <typename T>
bool Menu<T>::prev()
{
  bool out = current > 0;
  if(out)
    changeSelectionHelper(-1);
  return out;
}

template <typename T>
bool Menu<T>::next()
{
  bool out = current < (items.size() - 1);
  if(out)
    changeSelectionHelper(1);
  return out;
}

// This returns the value associated with the currently selected item
template <typename T>
T Menu<T>::getCurrent()
{
  return items[current].second;
}

// This controls whether or not the user can see what is selected
template <typename T>
void Menu<T>::isSelected(bool flag)
{
  attrset(flag ? selectedAttr : itemAttr);
  paddedPrint(row + current + (hasHeader ? 1 : 0),
    col, items[current].first, width);

  move(row + current + (hasHeader ? 1 : 0), col);

  attrset(A_NORMAL);
}

#endif // MENU_H
