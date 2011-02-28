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
#include "CommandThread.h"

#include <QByteArray>
#include <QtDebug>

Script::Script (QWidget *p) : QObject(p)
{
  _parent = p;
  init();
}

Script::Script (QObject *p) : QObject(p)
{
  _parent = 0;
  init();
}

Script::Script () : QObject(0)
{
  _parent = 0;
  init();
}

Script::~Script ()
{
  clear();
  
  delete _command;

  if (_indicatorFlag)
    delete _indicator;

//qDebug() << "Script::~Script:" << _name << "deleted";
}

void Script::init ()
{
  _killFlag = 0;
  _barData = 0;
  _minutes = 0;

  _command = new Command;

  _indicator = new Indicator;
  _indicatorFlag = 1;

  _proc = new QProcess(this);
  connect(_proc, SIGNAL(readyReadStandardOutput()), this, SLOT(readFromStdout()));
  connect(_proc, SIGNAL(readyReadStandardError()), this, SLOT(readFromStderr()));
  connect(_proc, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(done(int, QProcess::ExitStatus)));
  connect(_proc, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(deleteLater()));
  connect(_proc, SIGNAL(error(QProcess::ProcessError)), this, SLOT(deleteLater()));
}

void Script::clear ()
{
  _proc->terminate();

  if (_plugins.count())
    qDeleteAll(_plugins);
  _plugins.clear();

  _command->clear();

  _killFlag = 0;
}

void Script::setIndicator (Indicator *d)
{
  delete _indicator;
  _indicatorFlag = 0;
  
  _indicator = d;
}

Indicator * Script::indicator ()
{
  return _indicator;
}

int Script::startScript ()
{
  // clean up if needed
  clear();

  _killFlag = 0;
  QString command = _com + " " + _file;

  QStringList l;
  l << QDateTime::currentDateTime().toString();
  l << tr("Script");
  l << _name;
  l << tr("started");
  emit signalMessage(l.join(" "));

  // start the process
  _proc->start(command, QIODevice::ReadWrite);

  // make sure process starts error free
  if (! _proc->waitForStarted())
  {
    qDebug("Script::startScript: %s : error starting script...timed out", qPrintable(command));
    clear();
    emit signalDone(_name);
    return 1;
  }

  _command->setName(_name);
  _command->setBarData(_barData);
  _command->setIndicator(_indicator);

//qDebug() << "Script::startScript:" << _name << "started";
  
  return 0;
}

void Script::done (int, QProcess::ExitStatus)
{
  QStringList l;
  l << QDateTime::currentDateTime().toString();
  l << tr("Script");
  l << _name;

  if (_killFlag)
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

void Script::readFromStdout ()
{
  QByteArray ba = _proc->readAllStandardOutput();
  QString s(ba);

  _command->parse(s);
  if (! _command->count())
  {
    qDebug() << "Script::readFromStdout: invalid request string " << s;
    clear();
    return;
  }

  if (_killFlag)
  {
    qDebug() << "Script::readFromStdout: script terminated";
    clear();
    return;
  }

  ScriptPlugin *plug = _plugins.value(_command->plugin());
  if (! plug)
  {
    plug = _factory.plugin(_command->plugin());
    if (! plug)
    {
      qDebug() << "Script::readFromStdout: syntax error, script abend" << s;
      clear();
      return;
    }

    _plugins.insert(_command->plugin(), plug);
    connect(plug, SIGNAL(signalResume()), this, SLOT(resume()));
    connect(this, SIGNAL(signalKill()), plug, SIGNAL(signalKill()));
    plug->setParent(_parent);
  }

  switch ((ScriptPlugin::Type) plug->type())
  {
    case ScriptPlugin::_DIALOG:
      plug->command(_command);
      break;
    default:
    {
      CommandThread *ct = new CommandThread(this, plug, _command);
      connect(ct, SIGNAL(finished()), ct, SLOT(deleteLater()));
      connect(ct, SIGNAL(finished()), this, SLOT(resume()), Qt::QueuedConnection);
      ct->start();
      break;
    }
  }
}

void Script::readFromStderr ()
{
  qDebug() << "Script::readFromStderr:" << _proc->readAllStandardError();
}

void Script::stopScript (QString d)
{
  if (_proc->state() == QProcess::NotRunning)
    return;

  if (_name == d || d.isEmpty())
  {
    _killFlag = TRUE;
    emit signalKill();
//qDebug() << "Script::stopScript";    
  }
}

void Script::setBarData (BarData *d)
{
  _barData = d;
}

void Script::setName (QString d)
{
  _name = d;
}

QString & Script::name ()
{
  return _name;
}

void Script::setCommand (QString d)
{
  _com = d;
}

QString & Script::command ()
{
  return _com;
}

void Script::setFile (QString d)
{
  _file = d;
}

QString & Script::file ()
{
  return _file;
}

void Script::setMinutes (int d)
{
  _minutes = d;
}

int Script::minutes ()
{
  return _minutes;
}

void Script::setLastRun (QString d)
{
  _lastRun = d;
}

QString & Script::lastRun ()
{
  return _lastRun;
}

void Script::resume ()
{
  _proc->write(_command->returnCode());
}

void Script::setType (QString d)
{
  _type = d;
}

QString & Script::type ()
{
  return _type;
}

void Script::setComment (QString d)
{
  _comment = d;
}

QString & Script::comment ()
{
  return _comment;
}
