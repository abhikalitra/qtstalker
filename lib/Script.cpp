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

#include "Script.h"

#include <QDebug>

Script::Script ()
{
  _status = 0;
  _stopFlag = 0;

  connect(&_scriptServer, SIGNAL(signalDone()), this, SLOT(done()));
}

void Script::start ()
{
  _stopFlag = 0;
  QString command = _command + " " + _file;

  emit signalMessage(QString(tr("Script ")) + _name + tr(" started."));
    
  _scriptServer.calculate2(command);
}

void Script::done ()
{
  if (_stopFlag)
  {
    emit signalMessage(QString(tr("Script ")) + _name + tr(" cancelled."));
    emit signalCancel();
  }
  else
  {
    emit signalMessage(QString(tr("Script ")) + _name + tr(" completed."));
    emit signalDone(_name);
  }
}

void Script::setName (QString &d)
{
  _name = d;
}

QString & Script::getName ()
{
  return _name;
}

void Script::setCommand (QString &d)
{
  _command = d;
}

QString & Script::getCommand ()
{
  return _command;
}

void Script::setFile (QString &d)
{
  _file = d;
}

QString & Script::getFile ()
{
  return _file;
}

void Script::setComment (QString &d)
{
  _comment = d;
}

QString & Script::getComment ()
{
  return _comment;
}

void Script::setStatus (int d)
{
  _status = d;
}

int Script::getStatus ()
{
  return _status;
}

void Script::setLastRun (QDateTime &d)
{
  _lastRun = d;
}

QDateTime & Script::getLastRun ()
{
  return _lastRun;
}

void Script::stop ()
{
  _stopFlag = 1;
  _scriptServer.stop();
}

