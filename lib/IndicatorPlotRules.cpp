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

