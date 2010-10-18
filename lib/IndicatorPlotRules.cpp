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

#include "IndicatorPlotRules.h"
#include "Operator.h"

#include <QtDebug>

IndicatorPlotRules::IndicatorPlotRules ()
{
}

IndicatorPlotRules::~IndicatorPlotRules ()
{
  qDeleteAll(_rules);
}

int IndicatorPlotRules::createRules (Indicator &indicator, QStringList &l, BarData &bars)
{
  int loop = 0;
  for (; loop < l.count(); loop++)
  {
    QStringList l2 = l.at(loop).split(",");
    if (l2.count() != 3)
      return 1;

    IndicatorPlotRule *r = new IndicatorPlotRule;
    r->setName(l2.at(0));
    r->setOp(l2.at(1).toInt());
    r->setValue(l2.at(2));
    _rules.append(r);

    // add any OHLC inputs to indicator for later usage
    bars.getInputFields(l2);
    if (l2.indexOf(r->value()) != -1)
    {
      Curve *curve = indicator.line(r->value());
      if (! curve)
      {
        curve = bars.getInput(bars.getInputType(r->value()));
        indicator.setLine(r->value(), curve);
      }
    }
  }

  return 0;
}

int IndicatorPlotRules::count ()
{
  return _rules.count();
}

IndicatorPlotRule * IndicatorPlotRules::getRule (int index)
{
  return _rules.at(index);
}

int IndicatorPlotRules::test (Indicator &indicator, int index)
{
  if (! _rules.count())
    return 0;

  int loop = 0;
  int count = 0;
  for (; loop < _rules.count(); loop++)
  {
    IndicatorPlotRule *rule = _rules.at(loop);

    int offset = 0;
    Curve *curve = indicator.line(rule->name());
    if (! curve)
    {
      // check for a lookback syntax
      if (rule->name().contains("."))
      {
        QStringList l = rule->name().split(".");
        if (l.count() != 2)
        {
          qDebug() << "IndicatorPlotRules::test: invalid syntax" << rule->name();
          return 0;
        }

        // get the curve portion
        curve = indicator.line(l.at(0));
        if (! curve)
        {
          qDebug() << "IndicatorPlotRules::test: invalid syntax" << rule->name();
          return 0;
        }

        // get the offset portion
        bool ok;
        offset = l.at(1).toInt(&ok);
        if (! ok)
        {
          qDebug() << "IndicatorPlotRules::test: invalid syntax" << rule->name();
          return 0;
        }
      }
      else
      {
        qDebug() << "IndicatorPlotRules::test: invalid syntax" << rule->name();
        return 0;
      }
    }

    // get the value
    double value = 0;
    if (index == -1)
    {
      int sindex, eindex;
      curve->keyRange(sindex, eindex);
      CurveBar *bar = curve->bar(eindex - offset);
      if (! bar)
        return 0;
      value = bar->data();
    }
    else
    {
      CurveBar *bar = curve->bar(index - offset);
      if (! bar)
        return 0;
      value = bar->data();
    }

    // now get the second curve
    int offset2 = 0;
    double value2 = 0;
    curve = indicator.line(rule->value());
    if (! curve)
    {
      // check for a lookback syntax
      if (rule->value().contains("."))
      {
        QStringList l = rule->value().split(".");
        if (l.count() != 2)
        {
          qDebug() << "IndicatorPlotRules::test: invalid syntax" << rule->value();
          return 0;
        }

        // get the curve portion
        curve = indicator.line(l.at(0));
        if (! curve)
        {
          qDebug() << "IndicatorPlotRules::test: invalid syntax" << rule->value();
          return 0;
        }

        // get the offset portion
        bool ok;
        offset2 = l.at(1).toInt(&ok);
        if (! ok)
        {
          qDebug() << "IndicatorPlotRules::test: invalid syntax" << rule->value();
          return 0;
        }
      }
      else
      {
        // check if its a value
        bool ok;
        value2 = rule->value().toDouble(&ok);
        if (! ok)
        {
          qDebug() << "IndicatorPlotRules::test: invalid syntax" << rule->value();
          return 0;
        }
      }
    }

    // get the value2
    if (index == -1)
    {
      int sindex, eindex;
      curve->keyRange(sindex, eindex);
      CurveBar *bar = curve->bar(eindex - offset2);
      if (! bar)
        return 0;
      value2 = bar->data();
    }
    else
    {
      CurveBar *bar = curve->bar(index - offset2);
      if (! bar)
        return 0;
      value2 = bar->data();
    }

    // now compare the values
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

  if (count == _rules.count())
    return 1; // all rules are true

  return 0; // one or more rules are false
}

