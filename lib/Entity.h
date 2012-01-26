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

#ifndef ENTITY_HPP
#define ENTITY_HPP

#include <QStringList>
//#include <QMetaType>
#include <QHash>
#include <QMap>

#include "Data.h"

class Entity
{
  public:
    Entity ();
    void setType (int);
    int type ();
    void setName (QString);
    QString name ();
    void data (QHash<QString, Data> &);
    void remove (QString);
    int set (QString, Data);
    int set (QHash<QString, Data> &);
    int setEntity (int, Entity &);
    int toData (QString, Data &);
    int toIndex (int, Entity &);
    int toOffset (int, Entity &);
    int highLow (double &, double &);
    void dkeys (QList<QString> &);
    void ekeys (QList<int> &);
    int dkeyCount ();
    int ekeyCount ();
    void ekeyRange (int &, int &);
    void merge (Entity &);

  protected:
    QString _name;
    int _type;
    QHash<QString, Data> _data;
    QMap<int, Entity> _bars;
};

//Q_DECLARE_METATYPE(Entity)

#endif
