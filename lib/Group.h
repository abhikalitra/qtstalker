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

#ifndef GROUP_HPP
#define GROUP_HPP

#include <QString>
#include <QHash>

#include "BarData.h"

class Group
{
  public:
    Group ();
    void clear ();
    QString & getName ();
    void setName (QString);
    void deleteSymbol (QString);
    int getSymbol (QString, BarData &);
    int count ();
    void setSymbol (BarData);
    void getStringList (QStringList &);
    void setStringList (QStringList &);
    int contains (BarData);

  protected:
    QString _name;
    QHash<QString, BarData> _symbols;
};

#endif

