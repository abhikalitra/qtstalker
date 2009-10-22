/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2007 Stefan S. Stratigakos
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

#ifndef INDICATORSETTINGS_HPP
#define INDICATORSETTINGS_HPP

#include <QString>
#include <QList>

class IndicatorSettings
{
  public:
    typedef struct
    {
      QString name;
      int type;
      QString val;
    } Item;

    enum Type
    {
      TypeInput,
      TypeInput2,
      TypeOutput,
      TypeOutput2,
      TypeOutput3,
      TypeInt,
      TypeDouble,
      TypeColor,
      TypeOperator,
      TypeSymbol,
      TypeMA
    };

    IndicatorSettings ();
    void addItem (QString &name, int type, QString &val);
    void addItem (QString &name, int type, int val);
    void addItem (QString &name, int type, double val);
    void getItem (int index, QString &name, int &type, QString &val);
    double getItemDouble (int index);
    int getItemInt (int index);
    void getItemVal (int index, QString &);
    void getItemInput (QString &);
    void getItemInput2 (QString &);
    void getItemOutput (QString &);
    void getItemOutput2 (QString &);
    void getItemOutput3 (QString &);
    void clear ();
    int count ();
    void setIndicator (QString &);
    void getIndicator (QString &);
    void getString (QString &);
    void parse (QString &);
    void load (QStringList &);
    void getName (QString &);
    void setName (QString &);
    
  protected:
    QList<Item> list;
    QString indicator;
    QString name;
};

#endif
