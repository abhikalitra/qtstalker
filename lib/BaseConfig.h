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
#include <QDateTime>

#include "DataBase.h"

class BaseConfig : public DataBase
{
  public:
    BaseConfig ();
    void createTable ();
    void setData (int, QString &);
    void getData (int, QString &);
    void getData (int, QSplitter *);
    void setData (int, QSplitter *);
    bool getBool (int);
    void setData (int, bool);
    int getInt (int);
    void setData (int, int);
    void setData (int, qint64);
    double getDouble (int);
    void setData (int, double);
    void getData (int, QColor &);
    void setData (int, QColor &);
    void getData (int, QFont &);
    void setData (int, QFont &);
    void getData (int, QPoint &);
    void setData (int, QPoint &);
    void getData (int, QSize &);
    void setData (int, QSize &);
    void setData (int, QStringList &);
    void getData (int, QStringList &);
    void setData (int, QDateTime &);
    void getData (int, QDateTime &);
};

#endif

