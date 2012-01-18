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

#include "CommandType.h"

#include <QDebug>

CommandType::CommandType ()
{
  _list << "AD";
  _list << "ADX";
  _list << "ARITHMETIC";
  _list << "AROON";
  _list << "ATR";
  _list << "AVERAGE_PRICE";
  _list << "BBANDS";
  _list << "BETA";
  _list << "BOP";
  _list << "BREAKOUT";
  _list << "CANDLE_PATTERN";
  _list << "CCI";
  _list << "CHART";
  _list << "CHART_OBJECT_BUY";
  _list << "CHART_OBJECT_HLINE";
  _list << "CHART_OBJECT_RETRACEMENT";
  _list << "CHART_OBJECT_SELL";
  _list << "CHART_OBJECT_TEXT";
  _list << "CHART_OBJECT_TLINE";
  _list << "CHART_OBJECT_VLINE";
  _list << "CHART_PANEL_REFRESH";
  _list << "CHART_UPDATE";
  _list << "CMO";
  _list << "COMPARE";
  _list << "CORREL";
  _list << "CSV";
  _list << "DIALOG";
  _list << "FI";
  _list << "GET";
  _list << "GROUP_PANEL_REFRESH";
  _list << "GROUP_SAVE";
  _list << "HT";
  _list << "HT_PHASOR";
  _list << "HT_SINE";
  _list << "LINEARREG";
  _list << "MA";
  _list << "MACD";
  _list << "MAMA";
  _list << "MEDIAN_PRICE";
  _list << "MINMAX";
  _list << "MFI";
  _list << "MOM";
  _list << "NEW";
  _list << "NEW_HIGH_LOW";
  _list << "NORMALIZE";
  _list << "PO";
  _list << "PLOT_LINE";
  _list << "PLOT_HISTOGRAM";
  _list << "PLOT_OHLC";
  _list << "ROC";
  _list << "RSI";
  _list << "RUN";
  _list << "SAR";
  _list << "SET";
  _list << "SHIFT";
  _list << "STDDEV";
  _list << "STOCH_FAST";
  _list << "STOCH_RSI";
  _list << "STOCH_SLOW";
  _list << "SZ";
  _list << "SYMBOL";
  _list << "SYMBOL_CURRENT";
  _list << "T3";
  _list << "TEST";
  _list << "THERM";
  _list << "TRIX";
  _list << "TYPICAL_PRICE";
  _list << "ULTOSC";
  _list << "VAR";
  _list << "VBP";
  _list << "VFI";
  _list << "VIDYA";
  _list << "WEIGHTED_CLOSE";
  _list << "WILLR";
  _list << "YAHOO_HISTORY";
}

QStringList CommandType::list ()
{
  return _list;
}

int CommandType::stringToType (QString d)
{
  return _list.indexOf(d);
}
