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

#ifndef SETTING_HPP
#define SETTING_HPP

#include <QStringList>
#include <QColor>
#include <QFont>
#include <QDateTime>

class Setting
{
  public:
    Setting ();
    virtual int set (QString, void *);
    virtual int set (QString);
    virtual void set (int);
    virtual void set (double);
    virtual void set (QStringList);
    virtual void set (QColor);
    virtual void set (QFont);
    virtual void set (QDateTime);
    virtual void set (bool);
    virtual int toInteger ();
    virtual double toDouble ();
    virtual QStringList toList ();
    virtual QColor toColor ();
    virtual QFont toFont ();
    virtual QDateTime toDateTime ();
    virtual bool toBool ();
    virtual QString toString ();

    int type ();

  protected:
    int _type;
};

#endif
