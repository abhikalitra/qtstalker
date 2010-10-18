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

// ******************************************************
// ******************************************************

#ifndef INDICATOR_PLOT_RULES_HPP
#define INDICATOR_PLOT_RULES_HPP

#include <QString>
#include <QStringList>

#include "Indicator.h"
#include "BarData.h"
#include "IndicatorPlotRule.h"

class IndicatorPlotRules
{
  public:
    IndicatorPlotRules ();
    ~IndicatorPlotRules ();
    int createRules (Indicator &indicator, QStringList &l, BarData &bars);
    int count ();
    IndicatorPlotRule * getRule (int);
    int test (Indicator &, int index);

  private:
    QList<IndicatorPlotRule *> _rules;
};

#endif
