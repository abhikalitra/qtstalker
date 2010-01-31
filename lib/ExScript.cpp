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
#include "SCGetIndicator.h"
#include "SCSetIndicator.h"
#include "SCPlot.h"
#include "SCSymbolList.h"
#include "IndicatorFactory.h"
#include "IndicatorBase.h"

#include <QByteArray>
#include <QtDebug>


ExScript::ExScript ()
{
  proc = 0;
  data = 0;

//  TA_RetCode rc = TA_Initialize();
//  if (rc != TA_SUCCESS)
//    qDebug("TALIB::setDefaults:error on TA_Initialize");

  functionList << "INDICATOR" << "GET_INDICATOR" << "SET_INDICATOR" << "PLOT";
  functionList << "SYMBOL_LIST";
}

ExScript::~ExScript ()
{
  clear();
}

void ExScript::clear ()
{
  tlines.clear();
  plotOrder.clear();

  if (proc)
  {
    delete proc;
    proc = 0;
  }
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
    qDebug() << "ExScript::calculate: script error";
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
    case INDICATOR:
    {
      if (! data)
        break;

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
    case GET_INDICATOR:
    {
      if (! data)
        break;

      SCGetIndicator sc;
      QByteArray ba;
      int rc = sc.calculate(l, ba, tlines);
      if (rc)
        ba.append("ERROR\n");
      proc->write(ba);
      break;
    }
    case SET_INDICATOR:
    {
      if (! data)
        break;

      SCSetIndicator sc;
      QByteArray ba;
      int rc = sc.calculate(l, tlines);
      if (rc)
        ba.append("1\n");
      else
        ba.append("0\n");
      proc->write(ba);
      break;
    }
    case PLOT:
    {
      if (! data)
        break;

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

