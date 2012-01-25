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

#include "ScriptTimerAdd.h"
#include "GlobalScriptTimer.h"
#include "GlobalParent.h"
#include "GlobalSidePanel.h"
#include "EAVDataBase.h"
#include "KeyScriptDataBase.h"

#include <QtDebug>
#include <QObject>

ScriptTimerAdd::ScriptTimerAdd ()
{
}

int ScriptTimerAdd::add (Entity &timer)
{
  EAVDataBase db("scripts");
  db.transaction();
  if (db.set(timer))
  {
    qDebug() << "ScriptTimerAdd::add: EAVDataBase error" << timer.name();
    return 1;
  }
  db.commit();

  KeyScriptDataBase keys;
  Data file, command, interval;
  timer.toData(keys.indexToString(KeyScriptDataBase::_FILE), file);
  timer.toData(keys.indexToString(KeyScriptDataBase::_RUN_INTERVAL), interval);
  timer.toData(keys.indexToString(KeyScriptDataBase::_COMMAND), command);
  
  ScriptTimer *st = new ScriptTimer(g_parent);
  QObject::connect(st, SIGNAL(signalStartScript(QString, QString)),
		   g_sidePanel->scriptPanel(), SLOT(runScript(QString, QString)));
  st->setName(timer.name());
  st->setFile(file.toString());
  st->setCommand(command.toString());
  st->setIntervalString(interval.toString());
  g_scriptTimerMutex.lock();
  g_scriptTimerList.insert(timer.name(), st);
  g_scriptTimerMutex.unlock();
  st->start();
  
  return 0;
}
