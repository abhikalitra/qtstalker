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

// *************************************************************************************************
// Interface for the Exchange db. This holds a list of all ISO????? exchange symbols so that
// we can keep a standard exchange naming system. The db is updated at each app startup depending
// if the docs/exchanges.csv file has been modified.
// *************************************************************************************************

#ifndef EXCHANGE_DATA_BASE_HPP
#define EXCHANGE_DATA_BASE_HPP

#include <QtSql>
#include <QStringList>

class ExchangeDataBase
{
  public:
    ExchangeDataBase ();
    void init ();
    int verifyExchangeName (QString &);
    int createExchanges ();
    void getExchanges (QStringList &);
    void getFieldList (QString &, QStringList &);
    int search (QString &, QString &, QString &, QStringList &);
    void codeFromName (QString &, QString &);

  private:
    QSqlDatabase _db;
};

#endif
