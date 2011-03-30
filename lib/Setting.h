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

#ifndef SETTING_HPP
#define SETTING_HPP

#include <QString>
#include <QStringList>
#include <QHash>
#include <QDateTime>
#include <QColor>
#include <QFont>
#include <QMetaType>

class Setting
{
  public:
    Setting ();
    void setData (QString, QString);
    void setData (int, QString);
    void setData (QString, int);
    void setData (int, int);
    void setData (QString, double);
    void setData (int, double);
    void setData (QString, QColor);
    void setData (QString, QDateTime);
    void setData (QString, QFont);
    
    QString data (QString);
    QString data (int);
    double getDouble (QString);
    double getDouble (int);
    int getInt (QString);
    int getInt (int);
    QDateTime dateTime (QString);
    QColor color (QString);
    QFont font (QString);
    
    void keyList (QStringList &);
    void remove (QString);
    void string (QString &);
    void parse (QString);
    void clear ();
    int count ();
    void strip (QString &);
    void copy (Setting *);

  protected:
    QHash<QString, QString> dict;
};

// this is for passing Indicator data between threads
Q_DECLARE_METATYPE(Setting)

#endif
