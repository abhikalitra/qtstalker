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

#ifndef TESTER_STOPS_HPP
#define TESTER_STOPS_HPP

#include <QStringList>
#include <QList>

#include "TesterSettings.h"
#include "BarData.h"
#include "TesterTrade.h"

class TesterStops
{
  public:
    TesterStops ();
    void setSettings (TesterSettings &);
    int checkStops (QList<TesterTrade *> &, int tindex, BarData &, int);
    int maximumLossStop (QList<TesterTrade *> &, int tindex, BarData &, int index);
    int profitTargetStop (QList<TesterTrade *> &, int tindex, BarData &, int index);
    int trailingStop (QList<TesterTrade *> &, int tindex, BarData &, int index);
    int barsStop (QList<TesterTrade *> &, int tindex, int index);
    int triggered ();

  private:
    TesterSettings _settings;
    int _triggered;
};

#endif
