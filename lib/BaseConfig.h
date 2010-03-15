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


#ifndef BASECONFIG_HPP
#define BASECONFIG_HPP

#include <QString>
#include <QStringList>
#include <QSplitter>
#include <QColor>
#include <QFont>
#include <QPoint>
#include <QSize>

class BaseConfig
{
  public:
    BaseConfig ();
    void createTable ();
    void transaction ();
    void commit ();

    void setBaseData (int, QString &);
    void getBaseData (int, QString &);

    void getBaseData (int, QSplitter *);
    void setBaseData (int, QSplitter *);

    bool getBool (int);
    void setBaseData (int, bool);

    int getInt (int);
    void setBaseData (int, int);

    double getDouble (int);
    void setBaseData (int, double);

    void getBaseData (int, QColor &);
    void setBaseData (int, QColor &);

    void getBaseData (int, QFont &);
    void setBaseData (int, QFont &);

    void getBaseData (int, QPoint &);
    void setBaseData (int, QPoint &);

    void getBaseData (int, QSize &);
    void setBaseData (int, QSize &);

    void setBaseData (int, QStringList &);
    void getBaseData (int, QStringList &);
};

#endif

