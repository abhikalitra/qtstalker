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

#include <QByteArray>
#include <QtDebug>

ExScript::ExScript (QString &ipp, QString &dbpp)
{
  indicatorPluginPath = ipp;
  dbPluginPath = dbpp;
  data = 0;
  deleteFlag = 0;
  killFlag = 0;
  enterLong = 0;
  exitLong = 0;
  enterShort = 0;
  exitShort = 0;

  functionList << "CLEAR";
  functionList << "INDICATOR";
  functionList << "GROUP";
  functionList << "PLOT";
  functionList << "QUOTE";
  functionList << "SYMBOL";
  functionList << "TEST_ENTER_LONG" << "TEST_EXIT_LONG" << "TEST_ENTER_SHORT" << "TEST_EXIT_SHORT";
  
  proc = new QProcess(this);
  connect(proc, SIGNAL(readyReadStandardOutput()), this, SLOT(readFromStdout()));
  connect(proc, SIGNAL(readyReadStandardError()), this, SLOT(readFromStderr()));
  connect(proc, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(done(int, QProcess::ExitStatus)));
}

ExScript::~ExScript ()
{
  clear();
}

void ExScript::clear ()
{
  if (proc)
    proc->kill();

  plotOrder.clear();

  if (deleteFlag)
    qDeleteAll(tlines);
  tlines.clear();

  enterLong = 0;
  exitLong = 0;
  enterShort = 0;
  exitShort = 0;
}

void ExScript::setBarData (BarData *d)
{
  data = d;
}

int ExScript::calculate (QString &command)
{
  // clean up if needed
  clear();

  // start the process
  proc->start(command, QIODevice::ReadWrite);

  // make sure process starts error free
  if (! proc->waitForStarted(10000))
  {
    qDebug() << "ExScript::calculate: error starting script...timed out";
    clear();
    return 1;
  }

  // wait until script is finished, this will block gui until done.
  if (! proc->waitForFinished(10000))
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
  proc->start(command, QIODevice::ReadWrite);

  // make sure process starts error free
  if (! proc->waitForStarted(10000))
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
  QByteArray ba = proc->readAllStandardOutput();
  QString s(ba);
  QStringList l =  s.split(",");
  if (! l.count())
  {
    qDebug() << "ExScript::readFromStdout: invalid request string " << l;
    return;
  }

  ba.clear();
  if (killFlag)
  {
    qDebug() << "ExScript::readFromStdout: script terminated";
    clear();
    return;
  }

  int i = functionList.indexOf(l[0]);
  switch ((Function) i)
  {
    case CLEAR:
    {
      // we actually have to delete the lines if we call this function in scripts
      // not from qtstalker or we will delete actual plots
      qDeleteAll(tlines);
      tlines.clear();
      plotOrder.clear();
      proc->write("0\n");
      break;
    }
    case INDICATOR:
    {
      SCIndicator sc;
      sc.calculate(l, ba, tlines, data, indicatorPluginPath);
      proc->write(ba);
      break;
    }
    case GROUP:
    {
      SCGroup sc;
      sc.calculate(l, ba);
      proc->write(ba);
      break;
    }
    case PLOT:
    {
      SCPlot sc;
      sc.calculate(l, plotOrder, tlines, ba);
      proc->write(ba);
      break;
    }
    case QUOTE:
    {
      quotes.calculate(l, ba, dbPluginPath, tlines);
      proc->write(ba);
      break;
    }
    case _SYMBOL:
    {
      SCSymbol sc;
      sc.calculate(l, ba);
      proc->write(ba);
      break;
    }
    case TEST_ENTER_LONG:
    {
      SCTest sc;
      enterLong = sc.getSig(l, tlines, ba);
      if (! enterLong)
	qDebug() << "ExScript::readFromStddout: no enterLong input";
      proc->write(ba);
      break;
    }
    case TEST_EXIT_LONG:
    {
      SCTest sc;
      exitLong = sc.getSig(l, tlines, ba);
      if (! exitLong)
	qDebug() << "ExScript::readFromStddout: no exitLong input";
      proc->write(ba);
      break;
    }
    case TEST_ENTER_SHORT:
    {
      SCTest sc;
      enterShort = sc.getSig(l, tlines, ba);
      if (! enterShort)
	qDebug() << "ExScript::readFromStddout: no enterShort input";
      proc->write(ba);
      break;
    }
    case TEST_EXIT_SHORT:
    {
      SCTest sc;
      exitShort = sc.getSig(l, tlines, ba);
      if (! exitShort)
	qDebug() << "ExScript::readFromStddout: no exitShort input";
      proc->write(ba);
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
//  QByteArray ba = proc->readAllStandardError();
//  QString s(ba);
//  qDebug() << "ExScript::readFromStderr:" << s;
  qDebug() << "ExScript::readFromStderr:" << proc->readAllStandardError();
}

void ExScript::getLines (QList<PlotLine *> &lines)
{
  int loop;
  for (loop = 0; loop < plotOrder.count(); loop++)
    lines.append(tlines.value(plotOrder[loop]));

  QHashIterator<QString, PlotLine *> it(tlines);
  while (it.hasNext())
  {
    it.next();
    if (! it.value()->plotFlag())
      delete it.value();
  }

  clear();
}

int ExScript::getState ()
{
  int rc = 0;

  if (! proc)
    return rc;

  if (proc->state() != QProcess::NotRunning)
    rc = 1;

  return rc;
}

void ExScript::stop ()
{
  if (! proc)
    return;

  if (proc->state() == QProcess::NotRunning)
    return;

  killFlag = TRUE;
}

void ExScript::setDeleteFlag (int d)
{
  deleteFlag = d;
}

PlotLine * ExScript::getEnterLong ()
{
  return enterLong;
}

PlotLine * ExScript::getExitLong ()
{
  return exitLong;
}

PlotLine * ExScript::getEnterShort ()
{
  return enterShort;
}

PlotLine * ExScript::getExitShort ()
{
  return exitShort;
}

