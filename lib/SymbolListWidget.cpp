/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2007 Stefan S. Stratigakos
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307,
 *  USA.
 */

#include "SymbolListWidget.h"
#include "QuoteDataBase.h"
#include "KeySymbol.h"

SymbolListWidget::SymbolListWidget ()
{
}

void SymbolListWidget::setSymbols (QList<Symbol> &l)
{
  clear();
  setSortingEnabled(FALSE);

  KeySymbol keys;
  int loop = 0;
  for (; loop < l.size(); loop++)
  {
    Symbol symbol = l.at(loop);
    addSymbol(symbol);
  }

  setSortingEnabled(TRUE);
}

void SymbolListWidget::setSymbols (QStringList &l)
{
  clear();
  setSortingEnabled(FALSE);

  QuoteDataBase db;
  KeySymbol keys;
  int loop = 0;
  for (; loop < l.size(); loop++)
  {
    Symbol symbol;
    symbol.set(keys.indexToString(KeySymbol::_SYMBOL), Data(l.at(loop)));

    if (db.getSymbol(symbol))
      continue;

    addSymbol(symbol);
  }

  setSortingEnabled(TRUE);
}

void SymbolListWidget::addSymbol (Symbol &d)
{
  KeySymbol keys;
  Data sym, name;
  d.toData(keys.indexToString(KeySymbol::_SYMBOL), sym);
  d.toData(keys.indexToString(KeySymbol::_NAME), name);

  QListWidgetItem *item = new QListWidgetItem;
  item->setText(sym.toString());
  item->setToolTip(name.toString());
    
  addItem(item);
}
