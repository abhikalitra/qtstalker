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
#include "ChartObjectKey.h"
#include "EntityType.h"
#include "CommandParse.h"
#include "CommandFactory.h"

#include <QDebug>
#include <QUuid>

Script::Script (QObject *p) : QThread (p)
{
  _killFlag = 0;
  _verbList << "DIALOG" << "GET" << "NEW" << "RUN" << "SET";
  _id = QUuid::createUuid().toString();
}

Script::~Script ()
{
  qDeleteAll(_commands);
  emit signalDeleted(_id);
//qDebug() << "Script::~Script:" << _file << "deleted";
}

void Script::run ()
{
  QProcess _proc(0);
//  connect(_proc, SIGNAL(readyReadStandardOutput()), this, SLOT(readFromStdout()));
//  connect(&_proc, SIGNAL(readyReadStandardError()), this, SLOT(readFromStderr()));
//  connect(&_proc, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(done(int, QProcess::ExitStatus)));
//  connect(&_proc, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(deleteLater()));
//  connect(&_proc, SIGNAL(error(QProcess::ProcessError)), this, SLOT(deleteLater()));
//  connect(&_proc, SIGNAL(error(QProcess::ProcessError)), this, SLOT(error(QProcess::ProcessError)));

  QString command = _command + " " + _file;

  // start the _process
  _proc.start(command, QIODevice::ReadWrite);

  // make sure _process starts error free
  if (! _proc.waitForStarted())
  {
    qDebug() << "Script::run: error timed out" << _file;
    return;
  }

  QStringList l;
  l << QDateTime::currentDateTime().toString();
  l << _file;
  l << tr("started");
  qDebug() << l.join(" ");

  while (_proc.state() == QProcess::Running)
  {
    if (_killFlag)
    {
      qDebug() << "Script::run: script terminated";
//      emit signalDone(_file);
      _proc.kill();
      break;
    }

    // wait until we have some input from _process
    _proc.waitForReadyRead(-1);
    QByteArray ba = _proc.readAllStandardOutput();
    qDebug() << _file << ba;

    // parse command
    QString s(ba);
    CommandParse cp;
    if (cp.parse(s))
    {
      qDebug() << "Script::run: verb parse error";
//      emit signalDone(_file);
      _proc.kill();
      break;
    }

    // check if verb
    if (_verbList.indexOf(cp.command()) == -1)
    {
      qDebug() << "Script::run: invalid verb" << cp.command();
//      emit signalDone(_file);
      _proc.kill();
      break;
    }

    CommandFactory fac;
    Command *command = fac.command(cp.command());
    if (! command)
    {
      qDebug() << "Script::run: invalid command" << cp.command();
//      emit signalDone(_file);
      _proc.kill();
      break;
    }

    QString rc = command->run(cp, this);
    delete command;
    
    if (rc == "ERROR")
    {
      qDebug() << "Script::run: command error" << cp.command() << rc;
      _proc.kill();
      break;
    }

    QByteArray rba;
    rba.append(rc + "\n");
    _proc.write(rba);
    _proc.waitForBytesWritten(-1);
  }
  
  done(0, _proc.exitStatus());
  
  deleteLater();
}

void Script::done (int, QProcess::ExitStatus)
{
  QStringList l;
  l << QDateTime::currentDateTime().toString();
  l << tr("Script");
  l << _file;

  if (_killFlag)
  {
    l << tr("cancelled");
    qDebug() << l.join(" ");
    emit signalStopped(_file);
  }
  else
  {
    l << tr("completed");
    qDebug() << l.join(" ");
    emit signalDone(_file);
  }
}

void Script::readFromStderr ()
{
//  qDebug() << "Script::readFromStderr:" << _proc->readAllStandardError();
}

void Script::stopScript ()
{
  _killFlag = TRUE;
  emit signalKill();
//qDebug() << "Script::stopScript";
}

int Script::count ()
{
  return _data.count();
}

QList<QString> Script::dataKeys ()
{
  return _data.keys();
}

int Script::nextROID ()
{
  ChartObjectKey cokeys;
  int low = 0;
  QHashIterator<QString, Entity> it(_data);
  while (it.hasNext())
  {
    it.next();

    Entity dg = it.value();
    if (dg.type() != EntityType::_CHART_OBJECT)
      continue;

    Data td;
    dg.toData(cokeys.indexToString(ChartObjectKey::_ID), td);
    int t = td.toInteger();
    if (t < low)
      low = t;
  }

  low--;

  return low;
}

void Script::setCommand (QString d)
{
  _command = d;
}

QString Script::command ()
{
  return _command;
}

void Script::setData (QString key, Entity &d)
{
  _data.insert(key, d);
}

int Script::data (QString k, Entity &d)
{
  if (! _data.contains(k))
    return 1;
  
  d = _data.value(k);
  
  return 0;
}

void Script::setName (QString d)
{
  _name = d;
}

QString Script::name ()
{
  return _name;
}

void Script::setFile (QString d)
{
  _file = d;
}

QString & Script::file ()
{
  return _file;
}

void Script::setSymbol (Symbol &d)
{
  _symbol = d;
}

Symbol & Script::symbol ()
{
  return _symbol;
}

void Script::error (QProcess::ProcessError)
{
  QStringList l;
  l << QDateTime::currentDateTime().toString();
  l << tr("Script");
  l << _file;
  l << tr("error");
  qDebug() << l.join(" ");
  emit signalDone(_file);
  deleteLater();
}

QString Script::id ()
{
  return _id;
}

void Script::threadMessage (QString d)
{
  emit signalMessage(d);
}

void Script::setScriptCommand (QString k, Command *d)
{
  Command *c = _commands.value(k);
  if (c)
    delete c;
  
  _commands.insert(k, d);
}

Command * Script::scriptCommand (QString k)
{
  return _commands.value(k);
}
