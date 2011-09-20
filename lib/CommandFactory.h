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

#ifndef COMMAND_FACTORY_HPP
#define COMMAND_FACTORY_HPP

#include "Command.h"

#include <QStringList>

class CommandFactory
{
  public:
    enum Type
    {
      _AD,
      _ADX,
      _ARITHMETIC,
      _AROON,
      _ATR,
      _AVERAGE_PRICE,
      _BBANDS,
      _BETA,
      _BOP,
      _BREAKOUT,
      _CANDLE_PATTERN,
      _CCI,
      _CHART,
      _CHART_OBJECT_BUY,
      _CHART_OBJECT_HLINE,
      _CHART_OBJECT_RETRACEMENT,
      _CHART_OBJECT_SELL,
      _CHART_OBJECT_TEXT,
      _CHART_OBJECT_TLINE,
      _CHART_OBJECT_VLINE,
      _CHART_PANEL_REFRESH,
      _CHART_UPDATE,
      _CMO,
      _COLOR,
      _COMPARE,
      _CORREL,
      _CSV,
      _DIALOG,
      _EXCHANGE_SEARCH_DIALOG,
      _FI,
      _FILE_DIALOG,
      _GROUP_PANEL_REFRESH,
      _GROUP_SAVE,
      _HT,
      _HT_PHASOR,
      _HT_SINE,
      _LINEARREG,
      _MA,
      _MACD,
      _MAMA,
      _MEDIAN_PRICE,
      _MINMAX,
      _MFI,
      _MOM,
      _NEW_HIGH_LOW,
      _NORMALIZE,
      _PO,
      _PLOT_LINE,
      _PLOT_HISTOGRAM,
      _PLOT_OHLC,
      _ROC,
      _RSI,
      _SAR,
      _SCRIPT_DONE,
      _SCRIPT_START,
      _SELECT_DIALOG,
      _SETTING,
      _SETTING_GET,
      _SHIFT,
      _STDDEV,
      _STOCH_FAST,
      _STOCH_RSI,
      _STOCH_SLOW,
      _SZ,
      _SYMBOL,
      _SYMBOL_CURRENT,
      _SYMBOL_DIALOG,
      _T3,
      _THERM,
      _TRIX,
      _TYPICAL_PRICE,
      _ULTOSC,
      _VAR,
      _VBP,
      _VFI,
      _VIDYA,
      _WEIGHTED_CLOSE,
      _WILLR,
      _YAHOO_HISTORY
    };

    CommandFactory ();
    Command * command (QObject *, QString);
    QStringList list ();
    int stringToType (QString);

  protected:
    QStringList _types;
};

#endif
