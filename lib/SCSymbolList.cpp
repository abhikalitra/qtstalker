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

#include "SCSymbolList.h"
#include "DBPlugin.h"
#include "BarData.h"
#include "Group.h"

#include <QtDebug>
#include <QList>

SCSymbolList::SCSymbolList ()
{
}

int SCSymbolList::calculate (QStringList &l, QByteArray &ba)
{
  // format = SYMBOL_LIST,EXCHANGE,SEARCH_STRING

  ba.clear();
  ba.append("ERROR\n");

  if (l.count() != 3)
  {
    qDebug() << "SCSymbolList::calculate: invalid parm count" << l.count();
    return 1;
  }

  DBPlugin db;
  Group bdl;
  db.getSearchList(l[1], l[2], bdl);
  
  int loop;
  QStringList sl;
  for (loop = 0; loop < bdl.count(); loop++)
  {
     BarData *bd = bdl.getItem(loop); 
     sl.append(bd->getSymbol());
  }
  
  ba.clear();
  ba.append(sl.join(","));
  ba.append('\n');

  return 0;
}

