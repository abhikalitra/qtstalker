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

#include "SetQuotes.h"
#include "QSQuoteDataBase.h"
#include "QSBar.h"
#include "QSGlobals.h" // for mutex
#include "QSLog.h"

#include <QtDebug>

SetQuotes::SetQuotes ()
{
}

int SetQuotes::command (QStringList &input, QString &dbPath, QString &output)
{
  // format = SetQuotes,<TYPE>,<EXCHANGE>,<SYMBOL>,<DATE_MASK>,* bar fields depending on type
  //              0       1        2         3          4       *
  
  output = "ERROR";

  QSLog log;
  
  if (input.count() < 5)
  {
    log.message(QSLog::Error, QString(" SetQuotes: invalid parm count " + QString::number(input.count())));
    return 1;
  }

  QSBar tbar;
  QStringList typeList;
  tbar.typeList(typeList);
  int type = typeList.indexOf(input.at(1));
  if (type == -1)
  {
    log.message(QSLog::Error, QString(" SetQuotes: invalid type " + input.at(1)));
    return 1;
  }

  QSSymbol symbol;
  symbol.type = type;
  symbol.exchange = input.at(2);
  symbol.symbol = input.at(3);

  int fields = 0;
  switch ((QSBar::QSBarType) type)
  {
    case QSBar::Futures:
      fields = 7;
      break;
    default:
      fields = 6;
      break;
  }
  
  QList<QSBar> bars;
  int loop = 5;
  for (; loop < input.count(); loop += fields)
  {
    if (loop + fields >= input.count())
    {
      log.message(QSLog::Error, QString(" SetQuotes: not enough bar values starting at pos " + QString::number(loop)));
      break;
    }
    
    QSBar bar;
    bar.setBarType((QSBar::QSBarType) type);
    
    QDateTime dt = QDateTime::fromString(input[loop], input.at(3));
    if (! dt.isValid())
    {
      log.message(QSLog::Error, QString(" SetQuotes: bar date or date mask " + input.at(loop) + " at pos " + QString::number(loop)));
      continue;
    }
    else
      bar.setDate(dt);
    
    if (bar.setOpen(input[loop + 1]))
    {
      log.message(QSLog::Error, QString(" SetQuotes: invalid bar open " + input.at(loop + 1) + " at pos " + QString::number(loop + 1)));
      continue;
    }

    if (bar.setHigh(input[loop + 2]))
    {
      log.message(QSLog::Error, QString(" SetQuotes: invalid bar high " + input.at(loop + 2) + " at pos " + QString::number(loop + 2)));
      continue;
    }

    if (bar.setLow(input[loop + 3]))
    {
      log.message(QSLog::Error, QString(" SetQuotes: invalid bar low " + input.at(loop + 3) + " at pos " + QString::number(loop + 3)));
      continue;
    }

    if (bar.setClose(input[loop + 4]))
    {
      log.message(QSLog::Error, QString(" SetQuotes: invalid bar close " + input.at(loop + 4) + " at pos " + QString::number(loop + 4)));
      continue;
    }

    if (bar.setVolume(input[loop + 5]))
    {
      log.message(QSLog::Error, QString(" SetQuotes: invalid bar volume " + input.at(loop + 5) + " at pos " + QString::number(loop + 5)));
      continue;
    }

    if ((QSBar::QSBarType) type == QSBar::Futures)
    {
      if (bar.setOI(input[loop + 6]))
      {
        log.message(QSLog::Error, QString(" SetQuotes: invalid bar oi " + input.at(loop + 6) + " at pos " + QString::number(loop + 6)));
        continue;
      }
    }

    if (bar.verify())
    {
      log.message(QSLog::Error, QString(" SetQuotes: invalid bar values at pos " + QString::number(loop)));
      continue;
    }

    bars.append(bar);
  }

  // lock the mutex so we stop other threads from writing to the database
  g_mutex.lock();

  QSQuoteDataBase db(dbPath);
  db.transaction();
  db.setBars(symbol, bars);
  db.commit();

  // we are done writing, unlock the mutex so other threads can write
  g_mutex.unlock();
  
  output = "OK";
  
  return 0;
}

//**********************************************************
//**********************************************************
//**********************************************************

QSPlugin * createPlugin ()
{
  SetQuotes *o = new SetQuotes;
  return ((QSPlugin *) o);
}

