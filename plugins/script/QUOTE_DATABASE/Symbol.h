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

// ******************************************************
// ******************************************************

#ifndef SYMBOL_HPP
#define SYMBOL_HPP

#include <QList>
#include <QHash>
#include <QStringList>
//#include <QMetaType>

#include "QuoteBar.h"

class Symbol
{
  public:
    Symbol ();
    ~Symbol ();
    void clear ();
    int count ();
    void prepend (QuoteBar *bar);
    void append (QuoteBar *bar);
    QuoteBar * bar (int);
    QString & symbol ();
    void setSymbol (QString);
    QString & name ();
    void setName (QString);
    QString & exchange ();
    void setExchange (QString);
    QString & type ();
    void setType (QString);
    QString & table ();
    void setTable (QString);

  protected:
    QList<QuoteBar *> _list;
    QString _symbol;
    QString _name;
    QString _exchange;
    QString _type;
    QString _table;
};

// this is for passing Indicator data between threads
//Q_DECLARE_METATYPE(Symbol)

#endif


