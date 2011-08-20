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

#include <QStringList>
#include <QDateTime>
#include <QColor>
#include <QFont>

class Setting
{
  public:
    enum Type
    {
      _NONE,
      _CURVE,
      _LIST,
      _STRING,
      _COLOR,
      _INTEGER,
      _DOUBLE,
      _DATETIME,
      _BOOL,
      _CHART,
      _FONT
    };

    Setting ();
    virtual QString getString ();
    virtual void setString (QString);
    virtual QStringList getList ();
    virtual void setList (QStringList);
    virtual QColor getColor ();
    virtual void setColor (QColor);
    virtual QFont getFont ();
    virtual void setFont (QFont);
    virtual int getInteger ();
    virtual void setInteger (int);
    virtual double getDouble ();
    virtual void setDouble (double);
    virtual QDateTime getDateTime ();
    virtual void setDateTime (QDateTime);
    virtual QString toString ();
    virtual int fromString (QString);
    virtual bool getBool ();
    virtual void setBool (bool);
    virtual int getIntegerHigh ();
    virtual int getIntegerLow ();
    virtual double getDoubleHigh ();
    virtual double getDoubleLow ();

    int type ();
    int inputType ();
    int outputType ();
    void setTip (QString);
    QString tip ();
    void setKey (QString);
    QString key ();

  protected:
    int _type;
    int _inputType;
    int _outputType;
    QString _tip;
    QString _key;
};

#endif
