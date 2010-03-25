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

#include "SCGroup.h"
#include "GroupDataBase.h"
#include "BarData.h"
#include "Group.h"

#include <QtDebug>

SCGroup::SCGroup ()
{
  methodList << "ADD" << "DELETE" << "GET";
}

int SCGroup::calculate (QStringList &l, QByteArray &ba)
{
  // format = GROUP,METHOD,*
  int rc = -1;

  if (l.count() < 2)
  {
    qDebug() << "SCGroup::calculate: invalid parm count" << l.count();
    return rc;
  }
  
  switch ((Method) methodList.indexOf(l[1]))
  {
    case ADD:
      rc = addGroup(l, ba);
      break;
    case DELETE:
      rc = deleteGroup(l, ba);
      break;
    case GET:
      rc = getGroup(l, ba);
      break;
    default:
      break;
  }
  
  return rc;
}

int SCGroup::addGroup (QStringList &l, QByteArray &ba)
{
  // format = GROUP,ADD,GROUP,EXCHANGE,SYMBOL

  ba.clear();
  ba.append("1\n");

  if (l.count() != 5)
  {
    qDebug() << "SCGroup::addGroup: invalid parm count" << l.count();
    return 1;
  }

  GroupDataBase db;
  Group g;
  g.setName(l[2]);
  db.getGroup(g);
  
  BarData *bd = new BarData;
  bd->setExchange(l[3]);
  bd->setSymbol(l[4]);
  g.append(bd);
  
  db.setGroup(g);

  ba.clear();
  ba.append("0\n");

  return 0;
}

int SCGroup::deleteGroup (QStringList &l, QByteArray &ba)
{
  // format = GROUP,DELETE,GROUP

  ba.clear();
  ba.append("1\n");

  if (l.count() != 3)
  {
    qDebug() << "SCGroup::addGroup: invalid parm count" << l.count();
    return 1;
  }

  GroupDataBase db;
  db.deleteGroup(l[2]);

  ba.clear();
  ba.append("0\n");

  return 0;
}

int SCGroup::getGroup (QStringList &l, QByteArray &ba)
{
  // format = GROUP,GET,GROUP

  ba.clear();
  ba.append("ERROR\n");

  if (l.count() != 3)
  {
    qDebug() << "SCGroup::getGroup: invalid parm count" << l.count();
    return 1;
  }

  GroupDataBase db;
  Group g;
  g.setName(l[2]);
  db.getGroup(g);
  
  int loop;
  QStringList rl;
  for (loop = 0; loop < g.count(); loop++)
  {
    BarData *bd = g.getItem(loop);
    rl.append(bd->getExchange());
    rl.append(bd->getSymbol());
  }

  ba.clear();
  ba.append(rl.join(","));
  ba.append("\n");

  return 0;
}

