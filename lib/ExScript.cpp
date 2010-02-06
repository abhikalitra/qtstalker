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
#include "IndicatorFactory.h"
#include "IndicatorBase.h"

#include <QByteArray>
#include <QtDebug>


ExScript::ExScript ()
{
  proc = 0;
  data = 0;
  deleteFlag = 0;

  functionList << "CLEAR";
  functionList << "INDICATOR" << "INDICATOR_GET" << "INDICATOR_GET_INDEX" << "INDICATOR_SET";
  functionList << "GROUP_ADD" << "GROUP_DELETE";
  functionList << "PLOT";
  functionList << "SYMBOL_LIST";
}

ExScript::~ExScript ()
{
  clear();
}

void ExScript::clear ()
{
  if (proc)
  {
    proc->kill();
    delete proc;
    proc = 0;
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

  proc = new QProcess(this);
  connect(proc, SIGNAL(readyReadStandardOutput()), this, SLOT(readFromStdout()));
  connect(proc, SIGNAL(readyReadStandardError()), this, SLOT(readFromStderr()));
  connect(proc, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(done(int, QProcess::ExitStatus)));

  // start the process
  proc->start(command, QIODevice::ReadWrite);

  // make sure process starts error free
  if (! proc->waitForStarted(30000))
  {
    qDebug() << "ExScript::calculate: error starting script...timed out";
    clear();
    return 1;
  }

  if (! proc->waitForFinished(30000))
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

  proc = new QProcess(this);
  connect(proc, SIGNAL(readyReadStandardOutput()), this, SLOT(readFromStdout()));
  connect(proc, SIGNAL(readyReadStandardError()), this, SLOT(readFromStderr()));
  connect(proc, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(done(int, QProcess::ExitStatus)));

  // start the process
  proc->start(command, QIODevice::ReadWrite);

  // make sure process starts error free
  if (! proc->waitForStarted(30000))
  {
    qDebug() << "ExScript::calculate: error starting script...timed out";
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
  QByteArray ba = proc->readAllStandardOutput();
  QString s(ba);
  QStringList l =  s.split(",");
  if (! l.count())
  {
    qDebug() << "ExScript::readFromStdout: invalid request string " << l;
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
      QByteArray ba;
      ba.append("0\n");
      proc->write(ba);
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
      QByteArray ba;
      ba.append(QString::number(rc) + '\n');
      proc->write(ba);
      delete ib;
      break;
    }
    case INDICATOR_GET:
    {
      SCIndicator sc;
      QByteArray ba;
      int rc = sc.getIndicator(l, ba, tlines);
      if (rc)
        ba.append("ERROR\n");
      proc->write(ba);
      break;
    }
    case INDICATOR_GET_INDEX:
    {
      SCIndicator sc;
      QByteArray ba;
      int rc = sc.getIndex(l, tlines, ba);
      if (rc)
        ba.append("ERROR\n");
      proc->write(ba);
      break;
    }
    case INDICATOR_SET:
    {
      SCIndicator sc;
      QByteArray ba;
      int rc = sc.setIndicator(l, tlines);
      if (rc)
        ba.append("1\n");
      else
        ba.append("0\n");
      proc->write(ba);
      break;
    }
    case GROUP_ADD:
    {
      SCGroup sc;
      QByteArray ba;
      int rc = sc.addGroup(l, ba);
      if (rc)
        ba.append("1\n");
      else
        ba.append("0\n");
      proc->write(ba);
      break;
    }
    case GROUP_DELETE:
    {
      SCGroup sc;
      QByteArray ba;
      int rc = sc.deleteGroup(l, ba);
      if (rc)
        ba.append("1\n");
      else
        ba.append("0\n");
      proc->write(ba);
      break;
    }
    case PLOT:
    {
      SCPlot sc;
      QByteArray ba;
      int rc = sc.calculate(l, plotOrder, tlines);
      if (rc)
        ba.append("1\n");
      else
        ba.append("0\n");
      proc->write(ba);
      break;
    }
    case SYMBOL_LIST:
    {
      SCSymbolList sc;
      QByteArray ba;
      int rc = sc.calculate(l, ba);
      if (rc)
        ba.append("ERROR\n");
      proc->write(ba);
      break;
    }
    default:
      break;
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

  clear();
}

void ExScript::setDeleteFlag (int d)
{
  deleteFlag = d;
}

