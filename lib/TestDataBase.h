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

#ifndef TESTDATABASE_HPP
#define TESTDATABASE_HPP

#include <QString>
#include <QStringList>
#include <QSqlQuery>

#include "Test.h"

class TestDataBase
{
  public:
    TestDataBase ();
    void init (); // called only at qtstalker startup, initializes database tables
    void transaction ();
    void commit ();
    void getTests (QStringList &);
    void getTest (Test &);
    void setTest (Test &);
    void deleteTest (Test &);
    void getRankings (QSqlQuery &rq);
};

#endif