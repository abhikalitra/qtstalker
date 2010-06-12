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

#include "SCSymbol.h"
#include "QuoteIndexDataBase.h"
#include "Group.h"
#include "Config.h"

#include <QtDebug>
#include <QList>

SCSymbol::SCSymbol ()
{
  methodList << "CURRENT" << "SEARCH";
}

int SCSymbol::calculate (QStringList &l, QByteArray &ba)
{
  // format = SYMBOL,METHOD,*

  int rc = 1;
  ba.clear();
  ba.append("ERROR\n");

  if (l.count() < 2)
  {
    qDebug() << "SCSymbol::calculate: invalid parm count" << l.count();
    return rc;
  }
  
  switch ((Method) methodList.indexOf(l[1]))
  {
    case CURRENT:
      rc = getCurrent(l, ba);
      break;
    case SEARCH:
      rc = getSearch(l, ba);
      break;
    default:
      break;
  }
  
  return rc;
}

int SCSymbol::getSearch (QStringList &l, QByteArray &ba)
{
  // format = SYMBOL,SEARCH,EXCHANGE,SEARCH_STRING

  if (l.count() != 4)
  {
    qDebug() << "SCSymbol::getSearch: invalid parm count" << l.count();
    return 1;
  }

  QuoteIndexDataBase idb;
  Group bdl;
  idb.getSearchList(l[2], l[3], bdl);
  
  int loop;
  QStringList sl;
  for (loop = 0; loop < bdl.count(); loop++)
  {
     BarData *bd = bdl.getItem(loop); 
     sl.append(bd->getExchange());
     sl.append(bd->getSymbol());
  }
  
  ba.clear();
  ba.append(sl.join(","));
  ba.append('\n');

  return 0;
}

int SCSymbol::getCurrent (QStringList &l, QByteArray &ba)
{
  // format = SYMBOL,CURRENT

  if (l.count() != 2)
  {
    qDebug() << "SCSymbol::getCurrent: invalid parm count" << l.count();
    return 1;
  }

  Config config;
  QStringList l2;
  config.getData(Config::CurrentChart, l2);
  QString s = "NONE";
  if (l2.count() == 2)
    s = l2.join(",");

  ba.clear();
  ba.append(s + '\n');
  
  return 0;
}

