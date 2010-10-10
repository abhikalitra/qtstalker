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
// Base class for config databases. saves/returns different types of data and converts to a string
// for storage in an sqlite db.
// *************************************************************************************************

#ifndef CONFIG_DATA_BASE_HPP
#define CONFIG_DATA_BASE_HPP

#include <QStringList>
#include <QSplitter>
#include <QColor>
#include <QFont>
#include <QPoint>
#include <QSize>
#include <QDateTime>

#include "DataBase.h"

class ConfigDataBase : public DataBase
{
  public:
    ConfigDataBase ();
    
    void setData (QString, QString);
    void getData (QString &, QString &);

    void setData (int, QString);
    void getData (int, QString &);
    
    void setData (int, QSplitter *);
    void getData (int, QSplitter *);
    
    void setData (int, bool);
    bool getBool (int);
    
    void setData (int, int);
    void setData (int, qint64);
    int getInt (int);
    
    void setData (int, double);
    double getDouble (int);
    
    void setData (int, QColor);
    void getData (int, QColor &);
    
    void setData (int, QFont);
    void getData (int, QFont &);
    
    void setData (QString, QPoint);
    void setData (int, QPoint);
    void getData (int, QPoint &);
    void getData (QString, QPoint &);
    
    void setData (QString, QSize);
    void setData (int, QSize);
    void getData (int, QSize &);
    void getData (QString, QSize &);
    
    void setData (int, QStringList &);
    void getData (int, QStringList &);
    
    void setData (int, QDateTime);
    void getData (int, QDateTime &);

  protected:
    QString _tableName;
};

#endif

