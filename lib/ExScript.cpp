/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2007 Stefan S. Stratigakos
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
#include "DataBase.h"
#include "SCIndicator.h"
#include "SCPlot.h"
#include "SCGroup.h"
#include "SCSymbolList.h"
#include "SCTest.h"
#include "IndicatorFactory.h"
#include "IndicatorBase.h"

#include <QByteArray>
#include <QtDebug>


ExScript::ExScript ()
{
//  proc = 0;
  data = 0;
  deleteFlag = 0;
  killFlag = 0;

  functionList << "CLEAR";
  functionList << "INDICATOR" << "INDICATOR_GET" << "INDICATOR_GET_INDEX" << "INDICATOR_GET_SIZE";
  functionList << "INDICATOR_SET";
  functionList << "GROUP_ADD" << "GROUP_DELETE" << "GROUP_GET";
  functionList << "PLOT";
  functionList << "SYMBOL_LIST";
  functionList << "TEST_ENTER_LONG" << "TEST_EXIT_LONG" << "TEST_ENTER_SHORT" << "TEST_EXIT_SHORT";

  proc = new QProcess(this);
  connect(proc, SIGNAL(readyReadStandardOutput()), this, SLOT(readFromStdout()));
  connect(proc, SIGNAL(readyReadStandardError()), this, SLOT(readFromStderr()));
  connect(proc, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(done(int, QProcess::ExitStatus)));
}

ExScript::~ExScript ()
{
  clear();
  delete proc;
}

void ExScript::clear ()
{
//  killFlag = 0;

  if (proc)
  {
    proc->kill();
//    delete proc;
//    proc = 0;
  }

  plotOrder.clear();

  if (deleteFlag)
  {
    QHashIterator<QString, PlotLine *> it(tlines);
    while (it.hasNext())
    {
      it.next();
      delete it.value();
    }
  }

  tlines.clear();
}

void ExScript::setBarData (BarData *d)
{
  data = d;
}

int ExScript::calculate (QString &command)
{
  // clean up if needed
  clear();

//  proc = new QProcess(this);
//  connect(proc, SIGNAL(readyReadStandardOutput()), this, SLOT(readFromStdout()));
//  connect(proc, SIGNAL(readyReadStandardError()), this, SLOT(readFromStderr()));
//  connect(proc, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(done(int, QProcess::ExitStatus)));

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

//  proc = new QProcess(this);
//  connect(proc, SIGNAL(readyReadStandardOutput()), this, SLOT(readFromStdout()));
//  connect(proc, SIGNAL(readyReadStandardError()), this, SLOT(readFromStderr()));
//  connect(proc, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(done(int, QProcess::ExitStatus)));

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
      QHashIterator<QString, PlotLine *> it(tlines);
      while (it.hasNext())
      {
        it.next();
        delete it.value();
      }
      tlines.clear();
      plotOrder.clear();
      proc->write("0\n");
      break;
    }
    case INDICATOR:
    {
      IndicatorFactory fac;
      IndicatorBase *ib = fac.getFunction(l[1]);
      if (! ib)
      {
        proc->write("1\n");
        break;
      }

      int rc = ib->getCUS(l, tlines, data);
      ba.append(QString::number(rc) + '\n');
      proc->write(ba);
      delete ib;
      break;
    }
    case INDICATOR_GET:
    {
      SCIndicator sc;
      sc.getIndicator(l, ba, tlines);
      proc->write(ba);
      break;
    }
    case INDICATOR_GET_INDEX:
    {
      SCIndicator sc;
      sc.getIndex(l, tlines, ba);
      proc->write(ba);
      break;
    }
    case INDICATOR_GET_SIZE:
    {
      SCIndicator sc;
      sc.getSize(l, tlines, ba);
      proc->write(ba);
      break;
    }
    case INDICATOR_SET:
    {
      SCIndicator sc;
      sc.setIndicator(l, tlines, ba);
      proc->write(ba);
      break;
    }
    case GROUP_ADD:
    {
      SCGroup sc;
      sc.addGroup(l, ba);
      proc->write(ba);
      break;
    }
    case GROUP_DELETE:
    {
      SCGroup sc;
      sc.deleteGroup(l, ba);
      proc->write(ba);
      break;
    }
    case GROUP_GET:
    {
      SCGroup sc;
      sc.getGroup(l, ba);
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
    case SYMBOL_LIST:
    {
      SCSymbolList sc;
      sc.calculate(l, ba);
      proc->write(ba);
      break;
    }
    case TEST_ENTER_LONG:
    case TEST_EXIT_LONG:
    case TEST_ENTER_SHORT:
    case TEST_EXIT_SHORT:
    {
      SCTest sc;
      sc.getSig(l, data, tlines, ba, signalList);
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
  QByteArray ba = proc->readAllStandardError();
  QString s(ba);
  qDebug() << "ExScript::readFromStderr:" << s;
}

void ExScript::getLines (QList<PlotLine *> &lines)
{
  int loop;
  for (loop = 0; loop < plotOrder.count(); loop++)
  {
    lines.append(tlines.value(plotOrder[loop]));
//    qDebug() << "plot order" << loop << plotOrder[loop];
  }

  QHashIterator<QString, PlotLine *> it(tlines);
  while (it.hasNext())
  {
    it.next();
    if (! it.value()->getPlotFlag())
    {
//      qDebug() << "deleted" << it.key();
      delete it.value();
    }
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

void ExScript::getSignalList (QHash<int, int> &l)
{
  l = signalList;
  signalList.clear();
}

