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

#ifndef TYPE_COMMAND_HPP
#define TYPE_COMMAND_HPP

#include "Type.h"

class TypeCommand : public Type
{
  public:
    enum Type
    {
      _AD,
      _ADX,
      _ARITHMETIC,
      _AROON,
      _ATR,
      _BBANDS,
      _BETA,
      _BOP,
      _BREAKOUT,
      _CANDLE_PATTERN,
      _CCI,
      _CHART,
      _CHART_OBJECT_BUY,
      _CHART_OBJECT_HLINE,
      _CHART_OBJECT_REMOVE,
      _CHART_OBJECT_RETRACEMENT,
      _CHART_OBJECT_SELL,
      _CHART_OBJECT_TEXT,
      _CHART_OBJECT_TLINE,
      _CHART_OBJECT_VLINE,
      _CHART_PANEL,
      _CHART_REMOVE,
      _CHART_UPDATE,
      _CMO,
      _COMPARE,
      _CORREL,
      _CSV,
      _DATABASE,
      _DIALOG,
      _DIALOG_CONFIRM,
      _DIALOG_MESSAGE,
      _DIALOG_NEW,
      _DIALOG_SCRIPT,
      _DIALOG_SELECT,
      _DIALOG_USER,
      _FI,
      _GET,
      _GROUP_DATA_BASE,
      _GROUP_PANEL,
      _HT,
      _HT_PHASOR,
      _HT_SINE,
      _INDICATOR,
      _LINEARREG,
      _LOAD,
      _MA,
      _MACD,
      _MAMA,
      _MEDIAN_PRICE,
      _MINMAX,
      _MFI,
      _MOM,
      __NEW,
      _NEW_HIGH_LOW,
      _NORMALIZE,
      _PLOT_LINE,
      _PLOT_HISTOGRAM,
      _PLOT_OHLC,
      _PO,
      _PRICE,
      _ROC,
      _RSI,
      _RUN,
      _SAR,
      _SAVE,
      _SCRIPT_DATA_BASE,
      _SCRIPT_PANEL,
      _SET,
      _SHIFT,
      _STDDEV,
      _STOCH_FAST,
      _STOCH_RSI,
      _STOCH_SLOW,
      _SZ,
      _SYMBOL,
      _SYMBOL_CURRENT,
      _SYMBOL_DATA_BASE,
      _T3,
      _TEST,
      _THERM,
      _TRIX,
      _ULTOSC,
      _VAR,
      _VBP,
      _VFI,
      _VIDYA,
      _WILLR,
      _YAHOO_HISTORY
    };

    TypeCommand ();
};

#endif
