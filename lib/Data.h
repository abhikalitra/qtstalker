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

#ifndef DATA_HPP
#define DATA_HPP

#include <QStringList>
#include <QColor>
#include <QFont>
#include <QDateTime>
#include <QVariant>

#include "DataType.h"

class Data
{
  public:
    Data ();
    Data (DataType::Type);
    Data (QString);
    Data (QStringList);
    Data (QStringList, QString);
    Data (int);
    Data (double);
    Data (bool);
    Data (QColor);
    Data (QFont);
    Data (QDateTime);
    void clear ();
    int type ();
    void setType (int);
    int set (QString);
    int set (QString, int);
    int set (QStringList);
    int set (QStringList, QString);
    int set (int);
    int set (double);
    int set (bool);
    int set (QColor);
    int set (QFont);
    int set (QDateTime);
    QString toString ();
    QStringList toList ();
    int toInteger ();
    double toDouble ();
    bool toBool ();
    QColor toColor ();
    QFont toFont ();
    QDateTime toDateTime ();
    void setTab (int);
    int tab ();
    void setLabel (QString);
    QString label ();

  protected:
    int _type;
    int _tab;
    QString _label;
    QVariant _value;
    QStringList _list;
};

#endif
