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

#include "ExScript.h"
#include "ScriptPlugin.h"

#include <QByteArray>
#include <QtDebug>

ExScript::ExScript ()
{
  _killFlag = 0;

  _proc = new QProcess(this);
  connect(_proc, SIGNAL(readyReadStandardOutput()), this, SLOT(readFromStdout()));
  connect(_proc, SIGNAL(readyReadStandardError()), this, SLOT(readFromStderr()));
  connect(_proc, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(done(int, QProcess::ExitStatus)));
}

ExScript::~ExScript ()
{
  clear();
}

void ExScript::clear ()
{
  _proc->kill();
  _indicator.weedPlots();
  _indicator.cleanClear();
}

void ExScript::setIndicator (Indicator &d)
{
  _indicator = d;
}

int ExScript::calculate (QString &command)
{
  // clean up if needed
  clear();

  // start the process
  _proc->start(command, QIODevice::ReadWrite);

  // make sure process starts error free
  if (! _proc->waitForStarted())
  {
    qDebug("ExScript::calculate: %s : error starting script...timed out", qPrintable(command));
    clear();
    return 1;
  }

  // wait until script is finished, this will block gui until done.
  if (! _proc->waitForFinished())
  {
    qDebug("ExScript::calculate: %s : script error, timed out", qPrintable(command));
    clear();
    return 1;
  }

  return 0;
}

int ExScript::calculate2 (QString &command)
{
  // clean up if needed
  clear();

  // start the process
  _proc->start(command, QIODevice::ReadWrite);

  // make sure process starts error free
  if (! _proc->waitForStarted())
  {
    qDebug("ExScript::calculate2: %s : error starting script...timed out", qPrintable(command));
    clear();
    return 1;
  }

  return 0;
}

void ExScript::done (int, QProcess::ExitStatus)
{
  emit signalDone();
}

void ExScript::readFromStdout ()
{
  QByteArray ba = _proc->readAllStandardOutput();
  QString s(ba);
  QStringList l =  s.split(",");
  if (! l.count())
  {
    qDebug() << "ExScript::readFromStdout: invalid request string " << l;
    return;
  }

  ba.clear();
  if (_killFlag)
  {
    qDebug() << "ExScript::readFromStdout: script terminated";
    clear();
    return;
  }

  ScriptPlugin *plug = _factory.plugin(l[0]);
  if (! plug)
  {
    qDebug() << "ExScript::readFromStdout: syntax error, script abend" << l;
    clear();
    return;
  }
  
  plug->command(l, _indicator, _barData, ba);
  
  _proc->write(ba);
}

void ExScript::readFromStderr ()
{
  qDebug() << "ExScript::readFromStderr:" << _proc->readAllStandardError();
}

Indicator & ExScript::indicator ()
{
  _indicator.weedPlots();
  return _indicator;
  clear();
}

int ExScript::getState ()
{
  int rc = 0;

  if (! _proc)
    return rc;

  if (_proc->state() != QProcess::NotRunning)
    rc = 1;

  return rc;
}

void ExScript::stop ()
{
  if (! _proc)
    return;

  if (_proc->state() == QProcess::NotRunning)
    return;

  _killFlag = TRUE;

  clear();
}

void ExScript::setBarData (BarData &d)
{
  _barData = d;
}
