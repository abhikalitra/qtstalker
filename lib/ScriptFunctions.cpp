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

#include "ScriptFunctions.h"
#include "KeyScriptDataBase.h"
#include "EAVSearch.h"

#include <QtDebug>
#include <QUuid>

ScriptFunctions::ScriptFunctions ()
{
  _db = EAVDataBase("scripts");
}

int ScriptFunctions::save (Entity &script)
{
  QString name = script.name();
  if (name.isEmpty())
    script.setName(QUuid::createUuid().toString());
  
  _db.transaction();
  if (_db.set(script))
  {
    qDebug() << "ScriptFunctions::save: EAVDataBase error" << script.name();
    return 1;
  }
  _db.commit();
  
  return 0;
}

int ScriptFunctions::remove (QStringList l)
{
  _db.transaction();
  _db.remove(l);
  _db.commit();

  return 0;
}

int ScriptFunctions::names (QStringList &l)
{
  l.clear();
  
  if (_db.names(l))
  {
    qDebug() << "ScriptFunctions::names: db error";
    return 1;
  }

  return 0;
}

int ScriptFunctions::startupNames (QStringList &l)
{
  l.clear();

  KeyScriptDataBase skeys;
  EAVSearch search;
  search.append(skeys.indexToString(KeyScriptDataBase::_STARTUP), "=", "true");

  if (_db.search(search, l))
  {
    qDebug() << "ScriptFunctions::startupNames: db error";
    return 1;
  }

  return 0;
}

int ScriptFunctions::load (Entity &script)
{
  if (_db.get(script))
  {
    qDebug() << "ScriptFunctions::load: EAVDataBase error" << script.name();
    return 1;
  }
  
  return 0;
}

int ScriptFunctions::intervalNames (QStringList &l)
{
  l.clear();
  
  KeyScriptDataBase keys;
  EAVSearch search;
  search.append(keys.indexToString(KeyScriptDataBase::_INTERVAL), ">", "0");
  if (_db.search(search, l))
  {
    qDebug() << "ScriptFunctions::intervalNames: db error";
    return 1;
  }

  return 0;
}

int ScriptFunctions::intervalPendingNames (QStringList &l)
{
  l.clear();
  
  QStringList tl;
  intervalNames(tl);

  KeyScriptDataBase keys;
  int loop = 0;
  for (; loop < tl.size(); loop++)
  {
    Entity script;
    script.setName(tl.at(loop));
    
    if (_db.get(script))
    {
      qDebug() << "ScriptFunctions::intervalPendingNames: EAVDataBase error" << script.name();
      continue;
    }
    
    Data lastDate, interval;
    script.toData(keys.indexToString(KeyScriptDataBase::_LAST_RUN), lastDate);
    script.toData(keys.indexToString(KeyScriptDataBase::_INTERVAL), interval);
    
    QDateTime currentTime = QDateTime::currentDateTime();
    int seconds = lastDate.toDateTime().secsTo(currentTime);
    
    if (seconds < interval.toInteger())
      continue;
    
    l << tl.at(loop);
  }  

  return 0;
}

int ScriptFunctions::keys (QStringList &l)
{
  return _db.keys(l);
}
