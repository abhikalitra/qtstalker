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

#include "YahooThread.h"
#include "QuoteServerRequest.h"

#include <QDebug>
#include <QTcpSocket>

YahooThread::YahooThread (QObject *p, QString &type, QStringList &symbols, QDateTime sd, QDateTime ed) : QThread (p)
{
  _stopFlag = 0;
  
  _type = type;
  _symbols = symbols;
  _startDate = sd;
  _endDate = ed;
}

void YahooThread::run ()
{
  QStringList l;
  l << "YahooHistorical";
  if (_type == "H")
    l << "H" << _startDate.toString("yyyyMMdd") << _endDate.toString("yyyyMMdd");
  else
    l << "D" << "n" << "Name";
  QString command = l.join(",");

  _stopFlag = 0;
  QuoteServerRequest qsr;
  int loop = 0;
  for (; loop < _symbols.count(); loop++)
  {
    if (_stopFlag)
    {
      emit signalMessage(QString("Download cancelled."));
      break;
    }
    
    QString s = command + "," + _symbols.at(loop) + "\n";

    QString msg = _symbols.at(loop);
    
    if (qsr.run(s))
      msg.append("...ERROR");
    else
      msg.append("...OK");
    
    emit signalMessage(msg);
  }
}

void YahooThread::stop ()
{
  _stopFlag = TRUE;
}
