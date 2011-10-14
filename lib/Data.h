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
#include <QMetaType>
#include <QColor>
#include <QFont>
#include <QDateTime>

class Data
{
  public:
    Data ();
    int type ();
    void setDeleteFlag (int);
    int deleteFlag ();
    void setTab (int);
    int tab ();
    void setLabel (QString);
    QString label ();

    virtual void clear ();
    virtual int set (int, Data *);
    virtual int set (QString);
    virtual int set (QStringList);
    virtual int set (int);
    virtual int set (double);
    virtual int set (bool);
    virtual int set (QColor);
    virtual int set (QFont);
    virtual int set (QDateTime);
    virtual QString toString ();
    virtual QStringList toList ();
    virtual int toInteger ();
    virtual double toDouble ();
    virtual bool toBool ();
    virtual QColor toColor ();
    virtual QFont toFont ();
    virtual QDateTime toDateTime ();
    virtual Data * toData (int);
    virtual int highLow (double &, double &);
    virtual QList<int> keys ();
    virtual int keyCount ();
    virtual void keyRange (int &, int &);
    virtual void append (Data *);
    virtual QString toSaveString ();
    virtual int fromSaveString (QString);

  protected:
    int _type;
    int _delFlag;
    int _tab;
    QString _label;
};

Q_DECLARE_METATYPE(Data)

#endif
