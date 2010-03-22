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
#include "Config.h"

#include <QDebug>

Script::Script ()
{
  status = 0;
  refresh = 0;
  scriptServer = 0;

  timer = new QTimer(this);
  connect(timer, SIGNAL(timeout()), this, SLOT(timerDone()));
}

Script::~Script ()
{
  timer->stop();
  
  if (scriptServer)
  {
    scriptServer->stop();
    delete scriptServer;
  }
}

void Script::setName (QString &d)
{
  name = d;
}

QString & Script::getName ()
{
  return name;
}

void Script::setCommand (QString &d)
{
  command = d;
}

QString & Script::getCommand ()
{
  return command;
}

void Script::setFile (QString &d)
{
  file = d;
}

QString & Script::getFile ()
{
  return file;
}

void Script::setComment (QString &d)
{
  comment = d;
}

QString & Script::getComment ()
{
  return comment;
}

void Script::setStatus (int d)
{
  status = d;
}

int Script::getStatus ()
{
  return status;
}

void Script::setLastRun (QDateTime &d)
{
  lastRun = d;
}

QDateTime & Script::getLastRun ()
{
  return lastRun;
}

void Script::setRefresh (int d)
{
  refresh = d;
}

int Script::getRefresh ()
{
  return refresh;
}

void Script::start ()
{
  if (timer->isActive())
    return;
  
  if (! scriptServer)
  {
    // setup the script server
    Config config;
    QString ipp, dbpp;
    config.getData(Config::IndicatorPluginPath, ipp);
    config.getData(Config::DBPluginPath, dbpp);
    scriptServer = new ExScript(ipp, dbpp);
    scriptServer->setDeleteFlag(TRUE);
    connect(scriptServer, SIGNAL(signalDone()), this, SLOT(scriptDone()));
  }
    
  if (refresh > 0)
    timer->start(1000 * refresh);	
  else
    startScript();
}

void Script::stop ()
{
  timer->stop();	

  if (scriptServer)
    scriptServer->stop();

  emit signalMessage(QString(tr("Script ")) + name + tr(" cancelled."));
}

void Script::scriptDone ()
{
  if (! timer->isActive())
  {
    emit signalMessage(QString(tr("Script ")) + name + tr(" completed."));
    emit signalDone(name);
  }
  else
    emit signalMessage(QString(tr("Script ")) + name + tr(" completed."));
}

void Script::startScript ()
{
  if (! scriptServer)
    return;
  
  if (scriptServer->getState())
    return;
  
  QString s = command + " " + file;
  emit signalMessage(QString(tr("Script ")) + name + tr(" started."));
  scriptServer->calculate2(s);
}

void Script::timerDone ()
{
  startScript();
}


