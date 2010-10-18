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

#include "AlertThread.h"
#include "Indicator.h"
#include "QuoteServerRequest.h"
#include "BarData.h"
#include "IndicatorPluginFactory.h"
#include "IndicatorPlugin.h"
#include "IndicatorPlotRules.h"

#include <QDebug>

AlertThread::AlertThread (QObject *p, AlertItem alert) : QThread (p)
{
  _alert = alert;

  connect(this, SIGNAL(finished()), this, SLOT(deleteLater()));
}

void AlertThread::run ()
{
  BarData bd;
  bd.setKey(_alert.symbol());
    
  QStringList tl;
  tl << "Quotes" << "Date" << bd.getExchange() << bd.getSymbol();
  QString s;
  bd.barLengthText((BarData::BarLength) _alert.barLength(), s);
  tl << s << "0" << "0" << QString::number(_alert.bars());
  QString command = tl.join(",") + "\n";
      
  QuoteServerRequest qsr;
  if (qsr.run(command))
  {
    qDebug() << "AlertThread::run: qsr error" << _alert.indicator();
    quit();
    return;
  }

  bd.setBarLength((BarData::BarLength) _alert.barLength());
  bd.setBars(qsr.data());
 
  Indicator i;
  i.setSettings(_alert.settings());

  IndicatorPluginFactory fac;
  IndicatorPlugin *plug = fac.plugin(_alert.indicator());
  if (! plug)
  {
    qDebug() << "AlertThread::run: no plugin" << _alert.indicator();
    quit();
    return;
  }

  if (plug->getIndicator(i, bd))
  {
    qDebug() << "AlertThread::run: indicator error" << _alert.indicator();
    quit();
    return;
  }

  IndicatorPlotRules rules;
  if (rules.createRules(i, _alert.plots(), bd))
  {
    quit();
    return;
  }

  if (! rules.test(i, -1))
  {
    _alert.setLastUpdate(QDateTime::currentDateTime());
    quit();
    return;
  }

  _alert.setStatus(AlertItem::_Notify);
  _alert.setLastUpdate(QDateTime::currentDateTime());
  _alert.setHitDate(QDateTime::currentDateTime());
  emit signalDone(_alert);
}

