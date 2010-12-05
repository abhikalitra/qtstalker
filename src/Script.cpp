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
#include "Command.h"
#include "CommandThread.h"

#include <QByteArray>
#include <QtDebug>

Script::Script ()
{
  _killFlag = 0;
  _indicator = 0;
  _barData = 0;

  connect(&_proc, SIGNAL(readyReadStandardOutput()), this, SLOT(readFromStdout()));
  connect(&_proc, SIGNAL(readyReadStandardError()), this, SLOT(readFromStderr()));
  connect(&_proc, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(done(int, QProcess::ExitStatus)));
  connect(&_proc, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(deleteLater()));
  connect(&_proc, SIGNAL(error(QProcess::ProcessError)), this, SLOT(deleteLater()));
}

Script::~Script ()
{
  clear();
//qDebug() << "Script::~Script:" << _name << "deleted";
}

void Script::clear ()
{
  _proc.terminate();
  _killFlag = 0;

  if (_plugins.count())
    qDeleteAll(_plugins);
}

void Script::setIndicator (Indicator *d)
{
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
  QString command = _command + " " + _file;

  QStringList l;
  l << QDateTime::currentDateTime().toString();
  l << tr("Script");
  l << _name;
  l << tr("started");
  emit signalMessage(l.join(" "));

  // start the process
  _proc.start(command, QIODevice::ReadWrite);

  // make sure process starts error free
  if (! _proc.waitForStarted())
  {
    qDebug("Script::startScript: %s : error starting script...timed out", qPrintable(command));
    clear();
    emit signalDone(_name);
    return 1;
  }

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

//  QCoreApplication::exit (0);
}

void Script::readFromStdout ()
{
  QByteArray ba = _proc.readAllStandardOutput();
  QString s(ba);
  
  Command command(s);
  command.setScriptFlag(1);
  if (! command.count())
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

  command.setBarData(_barData);
  command.setIndicator(_indicator);

  ScriptPlugin *plug = _plugins.value(command.plugin());
  if (! plug)
  {
    plug = _factory.plugin(command.plugin());
    if (! plug)
    {
      qDebug() << "Script::readFromStdout: syntax error, script abend" << s;
      clear();
      return;
    }

    _plugins.insert(command.plugin(), plug);
  }

  if (! plug->isThreadSafe())
  {
    connect(plug, SIGNAL(signalResume()), this, SLOT(resume()));
    connect(plug, SIGNAL(signalDone(QString)), this, SLOT(returnData(QString)));
    _stringData = command.arrayData();
    plug->command(command);
  }
  else
  {
    CommandThread ct(this, command);
    ct.start();
    ct.wait();
    _proc.write(ct.arrayData());
  }
}

void Script::readFromStderr ()
{
  qDebug() << "Script::readFromStderr:" << _proc.readAllStandardError();
}

void Script::stopScript (QString d)
{
  if (_proc.state() == QProcess::NotRunning)
    return;

  if (_name != d)
    return;

  _killFlag = TRUE;
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
  _command = d;
}

QString & Script::command ()
{
  return _command;
}

void Script::setFile (QString d)
{
  _file = d;
}

QString & Script::file ()
{
  return _file;
}

void Script::resume ()
{
  QByteArray ba;
  ba.append(_stringData);
  ba.append('\n');
  _proc.write(ba);
}

void Script::returnData (QString d)
{
  QByteArray ba;
  ba.append(d);
  ba.append('\n');
  _proc.write(ba);
}
