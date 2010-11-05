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

#include "ScriptItem.h"

#include <QDebug>

ScriptItem::ScriptItem ()
{
  _status = 0;
  _stopFlag = 0;

  connect(&_scriptServer, SIGNAL(signalDone()), this, SLOT(done()));
}

void ScriptItem::start ()
{
  _stopFlag = 0;
  QString command = _command + " " + _file;

  QStringList l;
  l << QDateTime::currentDateTime().toString();
  l << tr("Script");
  l << _name;
  l << tr("started");
  emit signalMessage(l.join(" "));
    
  _scriptServer.calculate2(command);
}

void ScriptItem::done ()
{
  QStringList l;
  l << QDateTime::currentDateTime().toString();
  l << tr("Script");
  l << _name;

  if (_stopFlag)
  {
    l << tr("cancelled");
    emit signalMessage(l.join(" "));
    emit signalStopped(_name);
  }
  else
  {
    l << tr("completed");
    emit signalMessage(l.join(" "));
    emit signalDone(_name);
  }
}

void ScriptItem::setName (QString d)
{
  _name = d;
}

QString & ScriptItem::name ()
{
  return _name;
}

void ScriptItem::setCommand (QString d)
{
  _command = d;
}

QString & ScriptItem::command ()
{
  return _command;
}

void ScriptItem::setFile (QString d)
{
  _file = d;
}

QString & ScriptItem::file ()
{
  return _file;
}

void ScriptItem::setComment (QString d)
{
  _comment = d;
}

QString & ScriptItem::comment ()
{
  return _comment;
}

void ScriptItem::setStatus (int d)
{
  _status = d;
}

int ScriptItem::status ()
{
  return _status;
}

void ScriptItem::setLastRun (QDateTime d)
{
  _lastRun = d;
}

QDateTime & ScriptItem::lastRun ()
{
  return _lastRun;
}

void ScriptItem::setType (QString d)
{
  _type = d;
}

QString & ScriptItem::type ()
{
  return _type;
}

void ScriptItem::stop ()
{
  _stopFlag = 1;
  _scriptServer.stop();
}

