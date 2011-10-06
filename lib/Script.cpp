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
#include "CommandThread.h"

#include <QDebug>
#include <QUuid>

Script::Script (QObject *p) : QObject (p)
{
  _symbol = 0;

  clear();

  _proc = new QProcess(this);
  connect(_proc, SIGNAL(readyReadStandardOutput()), this, SLOT(readFromStdout()));
  connect(_proc, SIGNAL(readyReadStandardError()), this, SLOT(readFromStderr()));
  connect(_proc, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(done(int, QProcess::ExitStatus)));
//  connect(_proc, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(deleteLater()));
//  connect(_proc, SIGNAL(error(QProcess::ProcessError)), this, SLOT(deleteLater()));
  connect(_proc, SIGNAL(error(QProcess::ProcessError)), this, SLOT(error(QProcess::ProcessError)));

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

  if (_symbol)
    delete _symbol;
  _symbol = 0;

  deleteData();
  _data.clear();

  _name.clear();
//  _file.clear();
  _command.clear();

  qDeleteAll(_tsettings);
  _tsettings.clear();

  qDeleteAll(_tdata);
  _tdata.clear();
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

  deleteLater();
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
//qDebug() << ba;

  Message tsg;
  QString s(ba);
  QStringList l = s.split(";");
  int loop = 0;
  for (; loop < l.count(); loop++)
  {
    QStringList tl = l.at(loop).split("=");
    if (tl.count() != 2)
      continue;

    tsg.insert(tl.at(0).trimmed(), tl.at(1).trimmed());
  }

  CommandFactory fac;
  Command *command = fac.command(this, tsg.value("COMMAND"));
  if (! command)
  {
    qDebug() << "QtStalkerScript::run: command not found" << tsg.value("COMMAND");
    clear();
    return;
  }

//  command->setWidgetParent(g_parent);

  connect(command, SIGNAL(signalResume(void *)), this, SLOT(resume(void *)));

  switch ((Command::Type)command->type())
  {
    case Command::_DIALOG:
      command->runScript(&tsg, this);
      break;
    case Command::_THREAD:
    {
      CommandThread *ct = new CommandThread(this, tsg, command, this);
//      connect(ct, SIGNAL(finished()), this, SLOT(resume()), Qt::QueuedConnection);
      ct->start();
      break;
    }
    default: // _NORMAL
      command->runScript(&tsg, this);
      break;
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

void Script::resume (void *d)
{
  Command *command = (Command *) d;
  QString s = command->returnString() + "\n";

  QByteArray ba;
  ba.append(s);
  _proc->write(ba);

  command->deleteLater();
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

    int t = dg->get(ChartObjectData::_ID)->toInteger();
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

void Script::setTSetting (Setting *d)
{
  _tsettings << d;
}

void Script::setTData (Data *d)
{
  _tdata << d;
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
//  _symbol = d;

  Symbol *symbol = new Symbol;
  d->copy(symbol);
  _symbol = symbol;
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
