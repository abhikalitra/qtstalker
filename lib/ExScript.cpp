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
#include "SCIndicator.h"
#include "SCPlot.h"
#include "SCGroup.h"
#include "SCTest.h"
#include "SCSymbol.h"
#include "SCChartObject.h"

#include <QByteArray>
#include <QtDebug>

ExScript::ExScript (QString &ipp, QString &dbpp)
{
  _indicatorPluginPath = ipp;
  _dbPluginPath = dbpp;
  _data = 0;
  _killFlag = 0;

  _functionList << "CO";
  _functionList << "DELETE";
  _functionList << "INDICATOR";
  _functionList << "GROUP";
  _functionList << "PLOT";
  _functionList << "QUOTE";
  _functionList << "SYMBOL";
  _functionList << "TEST";
  
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
  if (_proc)
    _proc->kill();

  _indicator.weedPlots();
  _indicator.cleanClear();
}

void ExScript::setBarData (BarData *d)
{
  _data = d;
}

int ExScript::calculate (QString &command)
{
  // clean up if needed
  clear();

  // start the process
  _proc->start(command, QIODevice::ReadWrite);

  // make sure process starts error free
  if (! _proc->waitForStarted(10000))
  {
    qDebug() << "ExScript::calculate: error starting script...timed out";
    clear();
    return 1;
  }

  // wait until script is finished, this will block gui until done.
  if (! _proc->waitForFinished(10000))
  {
    qDebug() << "ExScript::calculate: script error, timed out";
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
  if (! _proc->waitForStarted(10000))
  {
    qDebug() << "ExScript::calculate: error starting script...timed out";
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

  int i = _functionList.indexOf(l[0]);
  switch ((Function) i)
  {
    case CO:
    {
      SCChartObject sc;
      sc.calculate(l, ba, _indicator, _data);
      _proc->write(ba);
      break;
    }
    case DELETE:
    {
      // we actually have to delete the lines if we call this function in scripts
      // not from qtstalker or we will delete actual plots
//      SCDelete sc;
//      sc.calculate(l, ba, _indicator);
//      proc->write(ba);
      break;
    }
    case INDICATOR:
    {
      SCIndicator sc;
      sc.calculate(l, ba, _indicator, _data, _indicatorPluginPath);
      _proc->write(ba);
      break;
    }
    case GROUP:
    {
      SCGroup sc;
      sc.calculate(l, ba);
      _proc->write(ba);
      break;
    }
    case PLOT:
    {
      SCPlot sc;
      sc.calculate(l, _indicator, ba);
      _proc->write(ba);
      break;
    }
    case QUOTE:
    {
      _quotes.calculate(l, ba, _dbPluginPath, _indicator);
      _proc->write(ba);
      break;
    }
    case _SYMBOL:
    {
      SCSymbol sc;
      sc.calculate(l, ba);
      _proc->write(ba);
      break;
    }
    case TEST:
    {
//      SCTest sc;
//      sc.calculate(l, ba, _tlines);
//      _proc->write(ba);
      break;
    }
    default:
    {
      // if we are here it means there was a command syntax error, abort script
      clear();
      qDebug() << "ExScript::readFromStdout: syntax error, script abend" << l;
      break;
    }
  }
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
}

