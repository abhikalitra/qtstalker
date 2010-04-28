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

#ifndef CANDLES_HPP
#define CANDLES_HPP

#include "IndicatorPlugin.h"

#include <QColor>

class CANDLES : public IndicatorPlugin
{
  public:
    enum Function
    {
      _NONE,
      _2CROWS,
      _3BLACKCROWS,
      _3INSIDE,
      _3LINESTRIKE,
      _3OUTSIDE,
      _3STARSINSOUTH,
      _3WHITESOLDIERS,
      _ABANDONEDBABY,
      _ADVANCEBLOCK,
      _BELTHOLD,
      _BREAKAWAY,
      _CLOSINGMARUBOZU,
      _CONCEALBABYSWALL,
      _COUNTERATTACK,
      _DARKCLOUDCOVER,
      _DOJI,
      _DOJISTAR,
      _DRAGONFLYDOJI,
      _ENGULFING,
      _EVENINGDOJISTAR,
      _EVENINGSTAR,
      _GAPSIDESIDEWHITE,
      _GRAVESTONEDOJI,
      _HAMMER,
      _HANGINGMAN,
      _HARAMI,
      _HARAMICROSS,
      _HIGHWAVE,
      _HIKKAKE,
      _HIKKAKEMOD,
      _HOMINGPIGEON,
      _IDENTICAL3CROWS,
      _INNECK,
      _INVERTEDHAMMER,
      _KICKING,
      _KICKINGBYLENGTH,
      _LADDERBOTTOM,
      _LONGLEGGEDDOJI,
      _LONGLINE,
      _MARUBOZU,
      _MATCHINGLOW,
      _MATHOLD,
      _MORNINGDOJISTAR,
      _MORNINGSTAR,
      _ONNECK,
      _PIERCING,
      _RICKSHAWMAN,
      _RISEFALL3METHODS,
      _SEPARATINGLINES,
      _SHOOTINGSTAR,
      _SHORTLINE,
      _SPINNINGTOP,
      _STALLEDPATTERN,
      _STICKSANDWICH,
      _TAKURI,
      _TASUKIGAP,
      _THRUSTING,
      _TRISTAR,
      _UNIQUE3RIVER,
      _UPSIDEGAP2CROWS,
      _XSIDEGAP3METHODS
    };

    enum Parm
    {
      Method,
      Penetration,
      MethodColor,
      Color,
      Label,
      MAColor,
      MA2Color,
      MA3Color,
      MAPlot,
      MA2Plot,
      MA3Plot,
      MALabel,
      MA2Label,
      MA3Label,
      MAPeriod,
      MA2Period,
      MA3Period,
      MAType,
      MA2Type,
      MA3Type
    };

    CANDLES ();
    int getIndicator (Indicator &, BarData *);
    int getCUS (QStringList &, QHash<QString, PlotLine *> &, BarData *);
    int dialog (int);
    PlotLine * getMethod (BarData *, int, double);
    PlotLine * getCANDLES (BarData *, QColor &);

  protected:
    QStringList methodList;
};

extern "C"
{
  IndicatorPlugin * createIndicatorPlugin ();
}

#endif
