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

  if (! rules.count())
  {
    quit();
    return;
  }

  int loop = 0;
  int count = 0;
  for (; loop < rules.count(); loop++)
  {
    IndicatorPlotRule *rule = rules.getRule(loop);
    Curve *curve = i.line(rule->name());
    if (! curve)
    {
      qDebug() << "AlertThread::run: no" << rule->name();
      continue;
    }

    double value = 0;
    int sindex, eindex;
    curve->keyRange(sindex, eindex);
    CurveBar *bar = curve->bar(eindex);
    if (! bar)
    {
      qDebug() << "AlertThread::run: no bar" << rule->name();
      continue;
    }
    value = bar->data();

    double value2 = 0;
    curve = i.line(rule->value());
    if (curve)
    {
      curve->keyRange(sindex, eindex);
      bar = curve->bar(eindex);
      if (! bar)
      {
        qDebug() << "AlertThread::run: no bar" << rule->value();
        continue;
      }
      value2 = bar->data();
    }
    else
    {
      bool ok;
      value2 = rule->value().toDouble(&ok);
      if (! ok)
      {
        qDebug() << "AlertThread::run: invalid value" << rule->name();
        continue;
      }
    }

    switch ((Operator::Type) rule->op())
    {
      case Operator::_LessThan:
        if (value < value2)
          count++;
        break;
      case Operator::_LessThanEqual:
        if (value <= value2)
          count++;
        break;
      case Operator::_Equal:
        if (value == value2)
          count++;
        break;
      case Operator::_GreaterThanEqual:
        if (value >= value2)
          count++;
        break;
      case Operator::_GreaterThan:
        if (value > value2)
          count++;
        break;
      default:
        break;
    }
  }

  _alert.setLastUpdate(QDateTime::currentDateTime());

  if (count == rules.count())
  {
    _alert.setStatus(AlertItem::_Notify);
    _alert.setHitDate(QDateTime::currentDateTime());
  }

  emit signalDone(_alert);
}

