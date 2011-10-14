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

#include <QString>
#include <QDateTime>
#include <QMetaType>
#include <QHash>

#include "Data.h"

class Symbol
{
  public:
    Symbol ();
    ~Symbol ();
    void clear ();
    int set (int, Data *);
    Data * getData (int);
    QList<int> barKeys ();
    int barKeyCount ();
    void append (Data *);
    void setSymbol (QString);
    QString symbol ();
    void setExchange (QString);
    QString exchange ();
    void setName (QString);
    QString name ();
    void setLength (int);
    int length ();
    void setRange (int);
    int range ();
    void setStartDate (QDateTime);
    QDateTime startDate ();
    void setEndDate (QDateTime);
    QDateTime endDate ();
    void setTable (QString);
    QString table ();
    void setType (QString);
    QString type ();
    void copy (Symbol *);
    QString symbolFull ();
    int setSymbolFull (QString);

  private:
    QHash<int, Data *> _bars;
    int _startIndex;
    int _endIndex;
    QString _symbol;
    QString _exchange;
    QString _name;
    int _length;
    int _range;
    QDateTime _startDate;
    QDateTime _endDate;
    QString _table;
    QString _type;
};

Q_DECLARE_METATYPE(Symbol)

#endif
