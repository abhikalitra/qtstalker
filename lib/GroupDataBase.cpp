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

#include "GroupDataBase.h"
#include "Globals.h"

#include <QtDebug>
#include <QDir>
#include <QSettings>

GroupDataBase::GroupDataBase ()
{
}

int GroupDataBase::load (QString name, QStringList &l)
{
  l.clear();
  QSettings db(g_globalSettings);
  QString key = "group_" + name;
  l = db.value(key).toStringList();

  return 0;
}

int GroupDataBase::merge (QString name, QStringList &l)
{
  // merge
  QSettings db(g_globalSettings);

  // check if this is a new indicator and add it to master list
  QStringList gl = db.value("groups").toStringList();
  if (gl.indexOf(name) == -1)
  {
    gl.append(name);
    db.setValue("groups", gl);
  }

  QString key = "group_" + name;
  QStringList g = db.value(key).toStringList();

  int loop = 0;
  for (; loop < l.count(); loop++)
    g << l.at(loop);
  g.removeDuplicates();
  
  db.setValue(key, g);

  return 0;
}

int GroupDataBase::saveAll (QString name, QStringList &l)
{
  QSettings db(g_globalSettings);

  // check if this is a new indicator and add it to master list
  QStringList gl = db.value("groups").toStringList();
  if (gl.indexOf(name) == -1)
  {
    gl.append(name);
    db.setValue("groups", gl);
  }

  QString key = "group_" + name;

  db.setValue(key, l);

  return 0;
}

int GroupDataBase::deleteGroup (QStringList &l)
{
  QSettings db(g_globalSettings);

  QStringList il = db.value("groups").toStringList();

  int loop = 0;
  for (; loop < l.count(); loop++)
  {
    il.removeAll(l.at(loop));

    QString key = "group_" + l.at(loop);
    db.remove(key);
  }

  db.setValue("groups", il);

  return 0;
}

int GroupDataBase::groups (QStringList &l)
{
  l.clear();
  QSettings db(g_globalSettings);
  l = db.value("groups").toStringList();

  return 0;
}
