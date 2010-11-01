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

#include "GROUP.h"
#include "GroupDataBase.h"
#include "Group.h"

#include <QtDebug>

GROUP::GROUP ()
{
  _methodList << "ADD" << "DELETE" << "GET";
}

int GROUP::command (QStringList &l, Indicator &, BarData &, QByteArray &ba)
{
  // GROUP,METHOD,*
  //   0     1
  
  int rc = 1;
  ba.clear();
  ba.append("ERROR\n");

  if (l.count() < 2)
  {
    qDebug() << "GROUP::command: invalid parm count" << l.count();
    return rc;
  }
  
  switch ((Method) _methodList.indexOf(l[1]))
  {
    case _ADD:
      rc = addGroup(l, ba);
      break;
    case _DELETE:
      rc = deleteGroup(l, ba);
      break;
    case _GET:
      rc = getGroup(l, ba);
      break;
    default:
      qDebug() << "GROUP::command: invalid method" << l[1];
      break;
  }
  
  return rc;
}

int GROUP::addGroup (QStringList &l, QByteArray &ba)
{
  // GROUP,ADD,GROUP,EXCHANGE,SYMBOL
  //    0   1    2      3       4

  if (l.count() != 5)
  {
    qDebug() << "GROUP::addGroup: invalid parm count" << l.count();
    return 1;
  }

  GroupDataBase db;
  Group g;
  g.setName(l[2]);
  db.getGroup(g);
  
  BarData bd;
  bd.setExchange(l[3]);
  bd.setSymbol(l[4]);
  g.setSymbol(bd);
  
  db.transaction();
  db.setGroup(g);
  db.commit();

  ba.clear();
  ba.append("0\n");

  return 0;
}

int GROUP::deleteGroup (QStringList &l, QByteArray &ba)
{
  // GROUP,DELETE,GROUP
  //   0     1     2

  if (l.count() != 3)
  {
    qDebug() << "GROUP::addGroup: invalid parm count" << l.count();
    return 1;
  }

  GroupDataBase db;
  db.transaction();
  db.deleteGroup(l[2]);
  db.commit();

  ba.clear();
  ba.append("0\n");

  return 0;
}

int GROUP::getGroup (QStringList &l, QByteArray &ba)
{
  // GROUP,GET,GROUP
  //   0    1    2

  if (l.count() != 3)
  {
    qDebug() << "GROUP::getGroup: invalid parm count" << l.count();
    return 1;
  }

  GroupDataBase db;
  Group g;
  g.setName(l[2]);
  db.getGroup(g);
  
  QStringList rl;
  g.getStringList(rl);

  ba.clear();
  ba.append(rl.join(",") + "\n");

  return 0;
}

//*************************************************************
//*************************************************************
//*************************************************************

ScriptPlugin * createScriptPlugin ()
{
  GROUP *o = new GROUP;
  return ((ScriptPlugin *) o);
}

