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
#include "ChartObjectData.h"
#include "DataFactory.h"
#include "CommandFactory.h"
#include "CommandParse.h"

#include <QDebug>
#include <QUuid>

Script::Script (QObject *p) : QObject (p)
{
  clear();

  _proc = new QProcess(this);
  connect(_proc, SIGNAL(readyReadStandardOutput()), this, SLOT(readFromStdout()));
  connect(_proc, SIGNAL(readyReadStandardError()), this, SLOT(readFromStderr()));
  connect(_proc, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(done(int, QProcess::ExitStatus)));
  connect(_proc, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(deleteLater()));
  connect(_proc, SIGNAL(error(QProcess::ProcessError)), this, SLOT(deleteLater()));
//  connect(_proc, SIGNAL(error(QProcess::ProcessError)), this, SLOT(error(QProcess::ProcessError)));

  _id = QUuid::createUuid().toString();
}

Script::~Script ()
{
  clear();
  _proc->terminate();
  _proc->waitForFinished();
  emit signalDeleted(_id);
//qDebug() << "Script::~Script:" << _file << "deleted";
}

void Script::clear ()
{
  _killFlag = 0;
  _symbol = 0;

  deleteData();
  _data.clear();

  _dialog.clear();

  _name.clear();
  _command.clear();
}

int Script::run ()
{
  _killFlag = 0;
  QString command = _command + " " + _file;

  // start the process
  _proc->start(command, QIODevice::ReadWrite);

  // make sure process starts error free
  if (! _proc->waitForStarted())
  {
    qDebug() << "Script::run: error timed out" << _file;
    clear();
    emit signalDone(_file);
    return 1;
  }

  QStringList l;
  l << QDateTime::currentDateTime().toString();
  l << _file;
  l << tr("started");
  qDebug() << l.join(" ");

  return 0;
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

void Script::readFromStdout ()
{
  if (_killFlag)
  {
    qDebug() << "Script::readFromStdout: script terminated";
    clear();
    return;
  }

  QByteArray ba = _proc->readAllStandardOutput();
qDebug() << _file << ba;

  // parse command
  QString s(ba);
  CommandParse cp;
  if (cp.parse(s))
  {
    qDebug() << "Script::readFromStdout: command parse error";
    clear();
    return;
  }

  // check if command
  CommandFactory fac;
  Command *command = fac.command(this, cp.command());
  if (! command)
  {
    qDebug() << "Script::readFromStdout: command parse error" << s << cp.command();
    clear();
    return;
  }

  connect(command, SIGNAL(signalDone(QString)), this, SLOT(resume(QString)));

  switch ((Command::Type)command->type())
  {
    case Command::_THREAD:
    {
      CommandThread *ct = new CommandThread(this, this, cp, command);
      ct->start();
      break;
    }
    default: // _WAIT
    {
      command->runScript(cp, this);
      break;
    }
  }
}

void Script::readFromStderr ()
{
  qDebug() << "Script::readFromStderr:" << _proc->readAllStandardError();
}

void Script::stopScript ()
{
  if (_proc->state() == QProcess::NotRunning)
    return;

  _killFlag = TRUE;
  emit signalKill();
//qDebug() << "Script::stopScript";
}

void Script::resume (QString d)
{
  QByteArray ba;
  ba.append(d);
  _proc->write(ba);
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
  int low = 0;
  QHashIterator<QString, Data *> it(_data);
  while (it.hasNext())
  {
    it.next();
    Data *dg = it.value();
    if (dg->type() != DataFactory::_CHART_OBJECT)
      continue;

    int t = dg->toData(ChartObjectData::_ID)->toInteger();
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

void Script::setData (QString key, Data *d)
{
  Data *sg = _data.value(key);
  if (sg)
    delete sg;

  _data.insert(key, d);
}

Data * Script::data (QString d)
{
  return _data.value(d);
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

void Script::setSymbol (Symbol *d)
{
  _symbol = d;

//  Symbol *symbol = new Symbol;
//  d->copy(symbol);
//  _symbol = symbol;
}

Symbol * Script::symbol ()
{
  return _symbol;
}

void Script::deleteData ()
{
  QHashIterator<QString, Data *> it(_data);
  while (it.hasNext())
  {
    it.next();
    Data *d = it.value();
    if (it.value()->deleteFlag())
      delete d;
  }
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

void Script::setDialog (QString key, DataDialog *d)
{
  DataDialog *dialog = _dialog.value(key);
  if (dialog)
    delete dialog;

  _dialog.insert(key, d);
}

DataDialog * Script::dialog (QString d)
{
  return _dialog.value(d);
}
