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
#include "CommandParse.h"
#include "CommandFactory.h"
#include "TypeVerb.h"
#include "ScriptFunctions.h"
#include "KeyScriptDataBase.h"

#include <QDebug>
#include <QUuid>

Script::Script (QObject *p) : QThread (p)
{
  _killFlag = 0;
  _id = QUuid::createUuid().toString();
  connect(this, SIGNAL(finished()), this, SLOT(deleteLater()));
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
  int rc = _ERROR;

  // make sure _process starts error free
  if (! _proc.waitForStarted())
  {
    qDebug() << "Script::run: error timed out" << _file;
    _proc.kill();
    done(_TIMEOUT);
//    deleteLater();
    return;
  }
  
//  QStringList l;
//  l << QDateTime::currentDateTime().toString();
//  l << _file;
//  l << tr("started");
//  qDebug() << l.join(" ");

  TypeVerb verbs;
  while (_proc.state() == QProcess::Running)
  {
    if (_killFlag)
    {
      qDebug() << "Script::run: script terminated";
      rc = _CANCEL;
      break;
    }

    // wait until we have some input from _process
    _proc.waitForReadyRead(-1);
    QByteArray ba = _proc.readAllStandardOutput();
    qDebug() << _file << ba;

    // check for end of script
    QString s(ba);
    s = s.trimmed();
    if (! s.length())
    {
      rc = _OK;
      break;
    }
    
    // parse command
    CommandParse cp;
    if (cp.parse(s))
    {
      qDebug() << "Script::run: verb parse error";
      qDebug() << "Script::run:" << s;
      break;
    }

    // check if verb
    if (verbs.stringToIndex(cp.command()) == -1)
    {
      qDebug() << "Script::run: invalid verb" << cp.command();
      break;
    }

    CommandFactory fac;
    Command *command = fac.command(cp.command());
    if (! command)
    {
      qDebug() << "Script::run: invalid command" << cp.command();
      break;
    }

    QString rc = command->run(cp, this);
    delete command;
    
    if (rc.contains("ERROR"))
    {
      qDebug() << "Script::run: command error" << cp.command() << rc;
      qDebug() << "Script::run:" << s;
      break;
    }

    QByteArray rba;
    rba.append(rc + "\n");
    _proc.write(rba);
    _proc.waitForBytesWritten(-1);
  }

  _proc.kill();
  
  done(rc);
  
//  deleteLater();
}

void Script::done (int rc)
{
  QStringList l;
  l << QDateTime::currentDateTime().toString();
  l << tr("Script");
  l << _file;

  switch ((ReturnCode) rc)
  {
    case _CANCEL:
      l << tr("cancelled");
      break;
    case _TIMEOUT:
      l << tr("timeout");
      break;
    case _ERROR:
      l << tr("error");
      break;
    default:
    {
      l << tr("completed");
      
      ScriptFunctions sf;
      Entity settings;
      settings.setName(_name);
      if (! sf.load(settings))
      {
        KeyScriptDataBase keys;
        Data lastRun;
        settings.toData(keys.indexToString(KeyScriptDataBase::_LAST_RUN), lastRun);
        lastRun.set(QDateTime::currentDateTime());
        settings.set(keys.indexToString(KeyScriptDataBase::_LAST_RUN), lastRun);
        if (sf.save(settings))
          qDebug() << "Script::done: error saving LAST_RUN" << _name;
      }
      
      break;
    }
  }

  qDebug() << l.join(" ");
  
  emit signalDone(_id);
}

void Script::stopScript ()
{
  _killFlag = TRUE;
qDebug() << "Script::stopScript";
}

int Script::count ()
{
  return _data.count();
}

QList<QString> Script::dataKeys ()
{
  return _data.keys();
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
