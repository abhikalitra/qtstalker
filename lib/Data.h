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
#include <QDateTime>
#include <QColor>
#include <QList>
#include <QFont>
#include <QHash>

class Data
{
  public:
    Data ();
    QString type ();
    bool dataContains (QString);
    int dataKeyCount ();
    virtual void clear ();
    virtual int set (QString, QString);
    virtual int set (int, QString);
    virtual int set (int, QDateTime);
    virtual int set (QString, QDateTime);
    virtual int set (int, QColor);
    virtual int set (QString, QColor);
    virtual int set (int, double);
    virtual int set (QString, double);
    virtual int set (int, int);
    virtual int set (QString, int);
    virtual int set (int, QStringList);
    virtual int set (QString, QStringList);
    virtual int set (int, bool);
    virtual int set (QString, bool);
    virtual int set (int, Data *);
    virtual int set (QString, Data *);
    virtual int set (int, QFont);
    virtual int set (QString, QFont);
    virtual QString get (QString);
    virtual QString get (int);
    virtual QDateTime getDateTime (int);
    virtual QColor getColor (int);
    virtual QColor getColor (QString);
    virtual double getDouble (int);
    virtual double getDouble (QString);
    virtual int getInteger (int);
    virtual int getInteger (QString);
    virtual QStringList getList (int);
    virtual bool getBool (int);
    virtual bool getBool (QString);
    virtual Data * getData (int);
    virtual QFont getFont (int);
    virtual QString toString ();
    virtual int fromString (QString);
    virtual int highLow (double &, double &);
    virtual QList<int> barKeys ();
    virtual QList<QString> dataKeys ();
    virtual int barKeyCount ();
    virtual void barKeyRange (int &, int &);

  protected:
    QHash<QString, QString> _data;
    QString _type;
};

#endif
