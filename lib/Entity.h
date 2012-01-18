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

#include "Data.h"

class Entity
{
  public:
    Entity ();
    void setType (int);
    int type ();
    void setName (QString);
    QString name ();
    QHash<QString, Data> data ();
    void remove (QString);
    int set (QString, Data);
    int set (QHash<QString, Data>);
    int setEntity (QString, Entity);
    int toData (QString, Data &);
    int toEntity (QString, Entity &);
    int highLow (double &, double &);
    QList<QString> dkeys ();
    QList<QString> ekeys ();
    int dkeyCount ();
    int ekeyCount ();
    void ekeyRange (int &, int &);
    void merge (Entity &);

  protected:
    int _startIndex;
    int _endIndex;
    QString _name;
    int _type;
    QHash<QString, Data> _data;
    QHash<QString, Entity> _bars;
};

//Q_DECLARE_METATYPE(Entity)

#endif
