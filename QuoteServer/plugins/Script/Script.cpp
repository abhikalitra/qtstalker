/*
 *  QuoteServer
 *
 *  Copyright (C) 2010 Stefan S. Stratigakos
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
#include "QSLog.h"

#include <QtDebug>
#include <QProcess>

Script::Script ()
{
}

int Script::command (QStringList &input, QString &, QString &output)
{
  // input format = Script,<COMMAND>,<RUNS>,<WAIT>,<TIMEOUT>
  //                   0       1       2      3        4

  // return is OK for success and ERROR for an error

  output = "ERROR";

  QSLog log;
  
  if (input.count() != 5)
  {
    log.message(QSLog::Error, QString(" Script: invalid parm count" + QString::number(input.count())));
    return 1;
  }

  bool ok;
  int runs = input.at(2).toInt(&ok);
  if (! ok)
  {
    log.message(QSLog::Error, QString(" Script: invalid runs value" + input.at(2)));
    return 1;
  }

  int wait = input.at(3).toInt(&ok);
  if (! ok)
  {
    log.message(QSLog::Error, QString(" Script: invalid wait value" + input.at(3)));
    return 1;
  }

  int timeout = input.at(4).toInt(&ok);
  if (! ok)
  {
    log.message(QSLog::Error, QString(" Script: invalid timeout value" + input.at(4)));
    return 1;
  }

  while (runs)
  {
    QProcess proc;
    proc.start(input[1], QIODevice::ReadWrite);
    if (! proc.waitForStarted())
      log.message(QSLog::Error, QString(" Script: unable to start script"));
    else
    {
      if (! proc.waitForFinished(timeout * 1000))
        log.message(QSLog::Error, QString(" Script: script error or timeout"));
      else
        log.message(QSLog::Info, QString(" Script: script completed"));
    }

    if (runs != -1)
      runs--;

    sleep(wait);
  }

  output = "OK";
  
  return 0;
}

//**********************************************************
//**********************************************************
//**********************************************************

QSPlugin * createPlugin ()
{
  Script *o = new Script;
  return ((QSPlugin *) o);
}

