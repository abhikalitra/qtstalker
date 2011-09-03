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

#include <QVariant>
#include <QHash>

class Data
{
  public:
    Data ();
    QString type ();
    bool dataContains (QString);
    int dataKeyCount ();
    virtual void clear ();
    virtual int set (QString, QVariant);
    virtual int set (int, QVariant);
    virtual int set (int, Data *);
    virtual QVariant get (QString);
    virtual QVariant get (int);
    virtual Data * getData (int);
    virtual QString toString ();
    virtual int fromString (QString);
    virtual int highLow (double &, double &);
    virtual QList<int> barKeys ();
    virtual QList<QString> dataKeys ();
    virtual int barKeyCount ();
    virtual void barKeyRange (int &, int &);
    virtual void append (Data *);

  protected:
    QHash<QString, QVariant> _data;
    QString _type;
};

#endif
