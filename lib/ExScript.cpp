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

#include <QByteArray>
#include <QtDebug>


ExScript::ExScript ()
{
  proc = 0;
  data = 0;
  is = 0;
  
  functionList << "INDICATOR" << "GET_INDICATOR" << "SET_INDICATOR" << "PLOT";
}

ExScript::~ExScript ()
{
  clear();
}

void ExScript::clear ()
{
  if (proc)
  {
    delete proc;
    proc = 0;
  }
    
  if (is)
  {
    delete is;
    is = 0;
  }
}

void ExScript::setInput (BarData *bd)
{
  data = bd;
}

void ExScript::calculate (QString &command)
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
    return;
  }
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
      if (! is)
        is = new IndicatorScript(data);
      
      int rc = is->parseIndicator(l);
      
      QByteArray ba;
      ba.append(QString::number(rc) + '\n');
      proc->write(ba);
      break;
    }
    case GET_INDICATOR:
    {
      if (! is)
        is = new IndicatorScript(data);
      QByteArray ba;
      is->getIndicator(l, ba);
      proc->write(ba);
      break;
    }
    case SET_INDICATOR:
    {
      if (! is)
        is = new IndicatorScript(data);
      QByteArray ba;
      is->setIndicator(l, ba);
      proc->write(ba);
      break;
    }
    case PLOT:
    {
      if (! is)
      {
        proc->write("1\n");
        break;
      }
      
      int rc = is->plot(l);
      
      QByteArray ba;
      ba.append(QString::number(rc) + '\n');
      proc->write(ba);
      break;
    }
    default: // if we are here we attempt to parse an indicator rerquest
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
  if (! is)
    return;
  
  is->getLines(lines);
  clear();
}

