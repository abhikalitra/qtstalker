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

#include "ScriptDataBase.h"
#include "Globals.h"

#include <QtDebug>
#include <QSettings>
#include <QDir>

ScriptDataBase::ScriptDataBase ()
{
}

int ScriptDataBase::load (Script *script)
{
  QString name = script->name();
  if (name.isEmpty())
  {
    qDebug() << "ScriptDataBase::load: invalid name";
    return 1;
  }

  QSettings db(g_globalSettings);

  QString key = "script_" + name;

  script->setCommand(db.value(key + "_command", "perl").toString());
  script->setFile(db.value(key + "_script", QDir::homePath()).toString());
  script->setMinutes(db.value(key + "_minutes", 0).toInt());
  script->setLastRun(db.value(key + "_last_run").toString());

  return 0;
}

int ScriptDataBase::save (Script *script)
{
  QString name = script->name();
  if (name.isEmpty())
  {
    qDebug() << "ScriptDataBase::save: invalid name";
    return 1;
  }

  QSettings db(g_globalSettings);

  // check if this is a new indicator and add it to master list
  QStringList l = db.value("scripts").toStringList();
  if (l.indexOf(name) == -1)
  {
    l.append(name);
    db.setValue("scripts", l);
  }

  QString key = "script_" + name;

  db.setValue(key + "_command", script->command());
  db.setValue(key + "_script", script->file());
  db.setValue(key + "_minutes", script->minutes());
  db.setValue(key + "_last_run", script->lastRun());
  
  return 0;
}

int ScriptDataBase::deleteScript (QStringList &l)
{
  QSettings db(g_globalSettings);

  QStringList il = db.value("scripts").toStringList();

  int loop = 0;
  for (; loop < l.count(); loop++)
  {
    il.removeAll(l.at(loop));

    QString key = "script_" + l.at(loop);
    
    db.remove(key + "_command");
    db.remove(key + "_script");
    db.remove(key + "_minutes");
    db.remove(key + "_last_run");
  }

  db.setValue("scripts", il);

  return 0;
}

int ScriptDataBase::scripts (QStringList &l)
{
  l.clear();
  QSettings db(g_globalSettings);
  l = db.value("scripts").toStringList();
  l.sort();

  return 0;
}

int ScriptDataBase::timerScripts (QStringList &l)
{
  l.clear();

  QSettings db(g_globalSettings);
  QStringList il = db.value("scripts").toStringList();

  int loop = 0;
  for (; loop < il.count(); loop++)
  {
    QString key = "script_" + il.at(loop);
    int minutes = db.value(key + "_minutes").toInt();
    if (minutes > 0)
      l << il.at(loop);
  }
    
  return 0;
}
