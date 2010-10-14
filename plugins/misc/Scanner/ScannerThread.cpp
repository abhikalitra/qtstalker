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

#include "ScannerThread.h"
#include "Indicator.h"
#include "QuoteServerRequest.h"
#include "BarData.h"
#include "IndicatorPluginFactory.h"
#include "IndicatorPlugin.h"
#include "Operator.h"
#include "GroupDataBase.h"
#include "IndicatorPlotRules.h"

#include <QDebug>

ScannerThread::ScannerThread (QObject *p, ScannerItem scanner) : QThread (p)
{
  _scanner = scanner;
  _stop = 0;

  connect(this, SIGNAL(finished()), this, SLOT(deleteLater()));
}

void ScannerThread::run ()
{
  QStringList keys, symbols;
  Group group = _scanner.group();
  group.getStringList(keys);

  Group results;
  results.setName(_scanner.groupName());

  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
    if (_stop)
    {
      qDebug() << "ScannerThread::run:" << _scanner.name() << "stopped";
      emit signalStopped(_scanner.name());
      quit();
      return;
    }
    
    BarData bd;
    group.getSymbol(keys.at(loop), bd);
    
    QStringList tl;
    tl << "Quotes" << "Date" << bd.getExchange() << bd.getSymbol();
    QString s;
    bd.barLengthText((BarData::BarLength) _scanner.barLength(), s);
    tl << s << "0" << "0" << QString::number(_scanner.dateRange());
    QString command = tl.join(",") + "\n";
      
    QuoteServerRequest qsr;
    if (qsr.run(command))
    {
      qDebug() << "ScannerThread::run: qsr error" << _scanner.name();
      continue;
    }

    bd.setBarLength((BarData::BarLength) _scanner.barLength());
    bd.setBars(qsr.data());
  
    Indicator i;
    i.setSettings(_scanner.settings());

    IndicatorPluginFactory fac;
    IndicatorPlugin *plug = fac.plugin(_scanner.indicator());
    if (! plug)
    {
      qDebug() << "ScannerThread::run: no plugin" << _scanner.indicator();
      continue;
    }

    if (plug->getIndicator(i, bd))
    {
      qDebug() << "ScannerThread::run: indicator error" << _scanner.indicator();
      continue;
    }

    IndicatorPlotRules rules;
    if (rules.createRules(i, _scanner.plots(), bd))
      continue;

    if (! rules.count())
      continue;

    int loop = 0;
    int count = 0;
    for (; loop < rules.count(); loop++)
    {
      IndicatorPlotRule *rule = rules.getRule(loop);

      Curve *curve = i.line(rule->name());
      if (! curve)
      {
        qDebug() << "ScannerThread::run: no" << rule->name();
        continue;
      }

      double value = 0;
      int sindex, eindex;
      curve->keyRange(sindex, eindex);
      CurveBar *bar = curve->bar(eindex);
      if (! bar)
      {
        qDebug() << "ScannerThread::run: no bar" << rule->name();
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
          qDebug() << "ScannerThread::run: no bar" << rule->value();
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
          qDebug() << "ScannerThread::run: invalid value" << rule->name();
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

    if (count == rules.count())
    {
      BarData tbd;
      tbd.setExchange(bd.getExchange());
      tbd.setSymbol(bd.getSymbol());
      results.setSymbol(tbd);
    }
  }

  GroupDataBase db;
  db.transaction();
  db.setGroup(results);
  db.commit();
  
  emit signalDone(_scanner.name());

  quit();
}

void ScannerThread::stop ()
{
  _stop = 1;
}

