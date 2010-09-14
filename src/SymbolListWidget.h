/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2010 Stefan S. Stratigakos
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

#ifndef SYMBOL_LIST_WIDGET_HPP
#define SYMBOL_LIST_WIDGET_HPP

#include <QListWidget>
#include <QListWidgetItem>
#include <QKeyEvent>

#include "Group.h"
#include "BarData.h"

class SymbolListWidget : public QListWidget
{
  Q_OBJECT

  signals:
    void signalSymbolSelected (BarData);

  public:
    SymbolListWidget ();
    void clearSymbols ();
    void deleteSymbol ();
    Group & symbols ();
    BarData symbol (QString);

  public slots:
    void itemClicked (QListWidgetItem *);
    void addSymbol (BarData);

  protected:
    void keyPressEvent (QKeyEvent *);

  private:
    Group _symbols;
};

#endif


