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
#include "Operator.h"

#include <QDebug>

AlertThread::AlertThread (QObject *p, AlertItem alert) : QThread (p)
{
  _alert = alert;

  connect(this, SIGNAL(finished()), this, SLOT(deleteLater()));
}

void AlertThread::run ()
{
  QStringList tl;
  tl << "Quotes" << "Date" << _alert.exchange() << _alert.symbol();
  BarData bd;
  QString s;
  bd.barLengthText((BarData::BarLength) _alert.barLength(), s);
  tl << s << "0" << "0" << QString::number(_alert.bars());
  QString command = tl.join(",") + "\n";
      
  QuoteServerRequest qsr;
  if (qsr.run(command))
  {
    qDebug() << "AlertThread::run: qsr error" << _alert.indicator();
    quit();
  }

  bd.setBarLength((BarData::BarLength) _alert.barLength());
  bd.setBars(qsr.data());
  
  Setting settings;
  settings.parse(_alert.settings());

  Indicator i;
  i.setSettings(settings);

  IndicatorPluginFactory fac;
  IndicatorPlugin *plug = fac.plugin(_alert.indicator());
  if (! plug)
  {
    qDebug() << "AlertThread::run: no plugin" << _alert.indicator();
    quit();
  }

  if (plug->getIndicator(i, bd))
  {
    qDebug() << "AlertThread::run: indicator error" << _alert.indicator();
    quit();
  }

  QStringList plotNames;
  plug->plotNames(i, plotNames);
  
  int loop = 0;
  int count = 0;
  int total = 0;
  for (; loop < plotNames.count(); loop++)
  {
    if (! _alert.enable(plotNames.at(loop)))
      continue;

    Curve *curve = i.line(plotNames.at(loop));
    if (! curve)
      continue;

    total++;
    
    switch ((Operator::Type) _alert.op(plotNames.at(loop)))
    {
      case Operator::_LessThan:
      {
        CurveBar *bar = curve->bar(curve->count() - 1);
        if (bar->data() < _alert.value(plotNames.at(loop)))
          count++;
        break;
      }
      case Operator::_LessThanEqual:
      {
        CurveBar *bar = curve->bar(curve->count() - 1);
        if (bar->data() <= _alert.value(plotNames.at(loop)))
          count++;
        break;
      }
      case Operator::_Equal:
      {
        CurveBar *bar = curve->bar(curve->count() - 1);
        if (bar->data() == _alert.value(plotNames.at(loop)))
          count++;
        break;
      }
      case Operator::_GreaterThanEqual:
      {
        CurveBar *bar = curve->bar(curve->count() - 1);
        if (bar->data() >= _alert.value(plotNames.at(loop)))
          count++;
        break;
      }
      case Operator::_GreaterThan:
      {
        CurveBar *bar = curve->bar(curve->count() - 1);
        if (bar->data() > _alert.value(plotNames.at(loop)))
          count++;
        break;
      }
      default:
        break;
    }
  }

  if (count == total && total != 0)
  {
    _alert.setStatus(AlertItem::_Notify);
    emit signalDone(_alert);
  }

  quit();
}

