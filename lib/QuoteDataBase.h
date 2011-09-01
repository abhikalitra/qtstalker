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

#ifndef QUOTE_DATA_BASE_HPP
#define QUOTE_DATA_BASE_HPP

#include <QtSql>
#include <QStringList>
#include <QList>

#include "Data.h"

class QuoteDataBase
{
  public:
    QuoteDataBase ();
    void init ();
    int getBars (Data *);
    int setBars (Data *);
    int newSymbol (Data *);
    int getSymbol (Data *);
    int transaction ();
    int commit ();
    int deleteSymbol (Data *);
    int getExchange (QStringList &);
    int rename (Data *osymbol, Data *nsymbol);
    int search (Data *bd, QList<Data *> &l);
    int setName (Data *);
//    int dump (DataGroup *, QString);

  private:
    QSqlDatabase _db;
};

#endif
