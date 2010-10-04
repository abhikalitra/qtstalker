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

#include "TesterThread.h"
#include "QuoteServerRequest.h"
#include "BarData.h"
#include "IndicatorPluginFactory.h"
#include "TesterDataBase.h"

#include <QDebug>

TesterThread::TesterThread (QObject *p, Indicator &i, QStringList &l, Setting &set) : QThread (p)
{
  _indicator = i;
  _symbols = l;
  _settings = set;
  _stopFlag = 0;
}

void TesterThread::run ()
{
  _stopFlag = 0;

  int loop = 0;
  for (; loop < _symbols.count(); loop++)
  {
    if (_stopFlag)
    {
      qDebug() << "TesterThread::run: stopped";
//      emit signalStopped();
      quit();
      return;
    }

    BarData bd;
    bd.setKey(_symbols.at(loop));

    QStringList tl;
    tl << "Quotes" << "Date" << bd.getExchange() << bd.getSymbol();
    QString s;
    bd.barLengthText((BarData::BarLength) _settings.getInt(TesterDataBase::_Period), s);
    tl << s << "0" << "0" << QString::number(_settings.getInt(TesterDataBase::_DateRange));
    QString command = tl.join(",") + "\n";

    QuoteServerRequest qsr;
    if (qsr.run(command))
    {
      qDebug() << "TesterThread::run: qsr error";
      continue;
    }

    bd.setBarLength((BarData::BarLength) _settings.getInt(TesterDataBase::_Period));
    bd.setBars(qsr.data());

    IndicatorPluginFactory fac;
    _settings.getData(TesterDataBase::_Indicator, s);
    IndicatorPlugin *plug = fac.plugin(s);
    if (! plug)
    {
      qDebug() << "TesterThread::run: no plugin" << s;
      continue;
    }

    if (plug->getIndicator(_indicator, bd))
    {
      qDebug() << "TesterThread::run: indicator error" << s;
      continue;
    }

    QStringList plotNames;
    plug->plotNames(_indicator, plotNames);

    int loop = 0;
    int count = 0;
    int total = 0;
/*    
    for (; loop < plotNames.count(); loop++)
    {
      if (! _scanner.enable(plotNames.at(loop)))
        continue;

      Curve *curve = i.line(plotNames.at(loop));
      if (! curve)
        continue;

      total++;

      switch ((Operator::Type) _scanner.op(plotNames.at(loop)))
      {
        case Operator::_LessThan:
        {
          CurveBar *bar = curve->bar(curve->count() - 1);
          if (bar->data() < _scanner.value(plotNames.at(loop)))
            count++;
          break;
        }
        case Operator::_LessThanEqual:
        {
          CurveBar *bar = curve->bar(curve->count() - 1);
          if (bar->data() <= _scanner.value(plotNames.at(loop)))
            count++;
          break;
        }
        case Operator::_Equal:
        {
          CurveBar *bar = curve->bar(curve->count() - 1);
          if (bar->data() == _scanner.value(plotNames.at(loop)))
            count++;
          break;
        }
        case Operator::_GreaterThanEqual:
        {
          CurveBar *bar = curve->bar(curve->count() - 1);
          if (bar->data() >= _scanner.value(plotNames.at(loop)))
            count++;
          break;
        }
        case Operator::_GreaterThan:
        {
          CurveBar *bar = curve->bar(curve->count() - 1);
          if (bar->data() > _scanner.value(plotNames.at(loop)))
            count++;
          break;
        }
        default:
          break;
      }
    }
*/
    if (count == total && total != 0)
    {
    }
  }
  
  emit signalDone(QString());
  
  quit();
}

void TesterThread::stop ()
{
  _stopFlag = 1;
}

/*
void TesterThread::stop ()
{
  int loop = 0;
  
}
*/

