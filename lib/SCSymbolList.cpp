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

#include "SCSymbolList.h"
#include "DataBase.h"

#include <QtDebug>


SCSymbolList::SCSymbolList ()
{
}

int SCSymbolList::calculate (QStringList &l, QByteArray &ba)
{
  // format = SYMBOL_LIST,SEARCH_STRING

  ba.clear();

  if (l.count() != 2)
  {
    qDebug() << "SCSymbolList::calculate: invalid parm count" << l.count();
    return 1;
  }

  DataBase db;
  QStringList symbolList;
  if (l[1] == "*")
    db.getAllChartsList(symbolList);
  else
    db.getSearchList(l[1], symbolList);

  ba.append(symbolList.join(","));
  ba.append('\n');

  return 0;
}

