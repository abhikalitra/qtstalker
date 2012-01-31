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

#include "ScriptTimerFunctions.h"
#include "GlobalScriptTimer.h"
#include "GlobalParent.h"
#include "GlobalSidePanel.h"
#include "KeyScriptDataBase.h"
#include "EAVSearch.h"

#include <QtDebug>
#include <QObject>

ScriptTimerFunctions::ScriptTimerFunctions ()
{
  _db = EAVDataBase("scripts");
}

int ScriptTimerFunctions::add (Entity &timer)
{
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

int ScriptTimerFunctions::save (Entity &timer)
{
  _db.transaction();
  if (_db.set(timer))
  {
    qDebug() << "ScriptTimerFunctions::save: EAVDataBase error" << timer.name();
    return 1;
  }
  _db.commit();
  
  return 0;
}

int ScriptTimerFunctions::remove (QStringList l)
{
  int loop = 0;
  for (; loop < l.size(); loop++)
  {
    ScriptTimer *st = g_scriptTimerList.value(l.at(loop));
    if (! st)
      continue;

    st->stop();
  
    g_scriptTimerList.remove(l.at(loop));
    delete st;
  }

  _db.transaction();
  _db.remove(l);
  _db.commit();
  
  return 0;
}

int ScriptTimerFunctions::modified (QString d)
{
  KeyScriptDataBase keys;
  Entity data;
  data.setName(d);

  if (_db.get(data))
    return 1;

  Data interval;
  data.toData(keys.indexToString(KeyScriptDataBase::_RUN_INTERVAL), interval);
  
  ScriptTimer *st = g_scriptTimerList.value(d);
  if (! st)
  {
    if (interval.toInteger() > 0)
      add(data);
  }
  else
  {
    st->stop();
    
    if (interval.toInteger() == 0)
    {
      g_scriptTimerList.remove(d);
      delete st;
    }
    else
    {
      Data file, command;
      data.toData(keys.indexToString(KeyScriptDataBase::_FILE), file);
      data.toData(keys.indexToString(KeyScriptDataBase::_COMMAND), command);
      
      st->setFile(file.toString());
      st->setCommand(command.toString());
      st->setIntervalString(interval.toString());
      
      st->start();
    }
  }
  
  return 0;
}

int ScriptTimerFunctions::setup ()
{
  QStringList l;
  if (names(l))
  {
    qDebug() << "ScriptTimerFunctions::setup: db error";
    return 1;
  }

  int loop = 0;
  for (; loop < l.size(); loop++)
  {
    Entity st;
    st.setName(l.at(loop));

    if (_db.get(st))
      continue;

    add(st);
  }
  
  return 0;
}

int ScriptTimerFunctions::names (QStringList &l)
{
  l.clear();
  
  KeyScriptDataBase skeys;
  EAVSearch search;
  search.append(skeys.indexToString(KeyScriptDataBase::_RUN_INTERVAL), ">", "0");

  if (_db.search(search, l))
  {
    qDebug() << "ScriptTimerFunctions::names: db error";
    return 1;
  }

  return 0;
}
