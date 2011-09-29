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

#include "CommandFactory.h"
#include "CommandAD.h"
#include "CommandADX.h"
#include "CommandArithmetic.h"
#include "CommandAROON.h"
#include "CommandATR.h"
#include "CommandAveragePrice.h"
#include "CommandBBANDS.h"
#include "CommandBETA.h"
#include "CommandBOP.h"
#include "CommandBreakout.h"
#include "CommandBuy.h"
#include "CommandCandlePattern.h"
#include "CommandCCI.h"
#include "CommandChart.h"
#include "CommandChartPanelRefresh.h"
#include "CommandChartUpdate.h"
#include "CommandCMO.h"
#include "CommandColor.h"
#include "CommandCompare.h"
#include "CommandCORREL.h"
#include "CommandCSV.h"
#include "CommandDialog.h"
#include "CommandFI.h"
#include "CommandGroupPanelRefresh.h"
#include "CommandGroupSave.h"
#include "CommandHLine.h"
#include "CommandHT.h"
#include "CommandLINEARREG.h"
#include "CommandMA.h"
#include "CommandMACD.h"
#include "CommandMAMA.h"
#include "CommandMedianPrice.h"
#include "CommandMINMAX.h"
#include "CommandMFI.h"
#include "CommandMOM.h"
#include "CommandNewHighLow.h"
#include "CommandNormalize.h"
#include "CommandPHASOR.h"
#include "CommandPO.h"
#include "CommandPlotOHLC.h"
#include "CommandPlotLine.h"
#include "CommandPlotHistogram.h"
#include "CommandRetracement.h"
#include "CommandROC.h"
#include "CommandRSI.h"
#include "CommandSAR.h"
#include "CommandSell.h"
#include "CommandSetting.h"
#include "CommandSettingGet.h"
#include "CommandShift.h"
#include "CommandSINE.h"
#include "CommandSTDDEV.h"
#include "CommandStochFast.h"
#include "CommandStochRSI.h"
#include "CommandStochSlow.h"
#include "CommandSZ.h"
#include "CommandSymbol.h"
#include "CommandSymbolCurrent.h"
#include "CommandT3.h"
#include "CommandText.h"
#include "CommandTHERM.h"
#include "CommandTLine.h"
#include "CommandTRIX.h"
#include "CommandTypicalPrice.h"
#include "CommandULTOSC.h"
#include "CommandVAR.h"
#include "CommandVBP.h"
#include "CommandVFI.h"
#include "CommandVIDYA.h"
#include "CommandVLine.h"
#include "CommandWeightedClose.h"
#include "CommandWILLR.h"
#include "CommandYahooHistory.h"

#include <QDebug>

CommandFactory::CommandFactory ()
{
  _types << "AD" << "ADX" << "ARITHMETIC" << "AROON" << "ATR" << "AVERAGE_PRICE";
  _types << "BBANDS" << "BETA" << "BOP" << "BREAKOUT";
  _types << "CANDLE_PATTERN" << "CCI" << "CHART";
  _types << "CHART_OBJECT_BUY" << "CHART_OBJECT_HLINE" << "CHART_OBJECT_RETRACEMENT" << "CHART_OBJECT_SELL";
  _types << "CHART_OBJECT_TEXT" << "CHART_OBJECT_TLINE" << "CHART_OBJECT_VLINE";
  _types << "CHART_PANEL_REFRESH" << "CHART_UPDATE";
  _types << "CMO" << "COLOR" << "COMPARE" << "CORREL" << "CSV";
  _types << "DIALOG";
  _types << "FI";
  _types << "GROUP_PANEL_REFRESH" << "GROUP_SAVE";
  _types << "HT" << "HT_PHASOR" << "HT_SINE";
  _types << "LINEARREG";
  _types << "MA" << "MACD" << "MAMA" << "MEDIAN_PRICE" << "MINMAX" << "MFI" << "MOM";
  _types << "NEW_HIGH_LOW" << "NORMALIZE";
  _types << "PO" << "PLOT_LINE" << "PLOT_HISTOGRAM" << "PLOT_OHLC";
  _types << "ROC" << "RSI";
  _types << "SAR" << "SETTING" << "SETTING_GET";
  _types << "SHIFT" << "STDDEV";
  _types << "STOCH_FAST" << "STOCH_RSI" << "STOCH_SLOW" << "SZ";
  _types << "SYMBOL" << "SYMBOL_CURRENT";
  _types << "T3" << "THERM" << "TRIX" << "TYPICAL_PRICE";
  _types << "ULTOSC";
  _types << "VAR" << "VBP" << "VFI" << "VIDYA";
  _types << "WEIGHTED_CLOSE" << "WILLR";
  _types << "YAHOO_HISTORY";
}

Command * CommandFactory::command (QObject *p, QString type)
{
  Command *c = 0;

  switch ((Type) _types.indexOf(type))
  {
    case _AD:
      c = new CommandAD(p);
      break;
    case _ADX:
      c = new CommandADX(p);
      break;
    case _ARITHMETIC:
      c = new CommandArithmetic(p);
      break;
    case _AROON:
      c = new CommandAROON(p);
      break;
    case _ATR:
      c = new CommandATR(p);
      break;
    case _AVERAGE_PRICE:
      c = new CommandAveragePrice(p);
      break;
    case _BBANDS:
      c = new CommandBBANDS(p);
      break;
    case _BETA:
      c = new CommandBETA(p);
      break;
    case _BOP:
      c = new CommandBOP(p);
      break;
    case _BREAKOUT:
      c = new CommandBreakout(p);
      break;
    case _CANDLE_PATTERN:
      c = new CommandCandlePattern(p);
      break;
    case _CCI:
      c = new CommandCCI(p);
      break;
    case _CHART:
      c = new CommandChart(p);
      break;
    case _CHART_OBJECT_BUY:
      c = new CommandBuy(p);
      break;
    case _CHART_OBJECT_HLINE:
      c = new CommandHLine(p);
      break;
    case _CHART_OBJECT_RETRACEMENT:
      c = new CommandRetracement(p);
      break;
    case _CHART_OBJECT_SELL:
      c = new CommandSell(p);
      break;
    case _CHART_OBJECT_TEXT:
      c = new CommandText(p);
      break;
    case _CHART_OBJECT_TLINE:
      c = new CommandTLine(p);
      break;
    case _CHART_OBJECT_VLINE:
      c = new CommandVLine(p);
      break;
    case _CHART_PANEL_REFRESH:
      c = new CommandChartPanelRefresh(p);
      break;
    case _CHART_UPDATE:
      c = new CommandChartUpdate(p);
      break;
    case _CMO:
      c = new CommandCMO(p);
      break;
    case _COLOR:
      c = new CommandColor(p);
      break;
    case _COMPARE:
      c = new CommandCompare(p);
      break;
    case _CORREL:
      c = new CommandCORREL(p);
      break;
    case _CSV:
      c = new CommandCSV(p);
      break;
    case _DIALOG:
      c = new CommandDialog(p);
      break;
    case _FI:
      c = new CommandFI(p);
      break;
    case _GROUP_PANEL_REFRESH:
      c = new CommandGroupPanelRefresh(p);
      break;
    case _GROUP_SAVE:
      c = new CommandGroupSave(p);
      break;
    case _HT:
      c = new CommandHT(p);
      break;
    case _HT_PHASOR:
      c = new CommandPHASOR(p);
      break;
    case _HT_SINE:
      c = new CommandSINE(p);
      break;
    case _LINEARREG:
      c = new CommandLINEARREG(p);
      break;
    case _MA:
      c = new CommandMA(p);
      break;
    case _MACD:
      c = new CommandMACD(p);
      break;
    case _MAMA:
      c = new CommandMAMA(p);
      break;
    case _MEDIAN_PRICE:
      c = new CommandMedianPrice(p);
      break;
    case _MINMAX:
      c = new CommandMINMAX(p);
      break;
    case _MFI:
      c = new CommandMFI(p);
      break;
    case _MOM:
      c = new CommandMOM(p);
      break;
    case _NEW_HIGH_LOW:
      c = new CommandNewHighLow(p);
      break;
    case _NORMALIZE:
      c = new CommandNormalize(p);
      break;
    case _PO:
      c = new CommandPO(p);
      break;
    case _PLOT_LINE:
      c = new CommandPlotLine(p);
      break;
    case _PLOT_HISTOGRAM:
      c = new CommandPlotHistogram(p);
      break;
    case _PLOT_OHLC:
      c = new CommandPlotOHLC(p);
      break;
    case _ROC:
      c = new CommandROC(p);
      break;
    case _RSI:
      c = new CommandRSI(p);
      break;
    case _SAR:
      c = new CommandSAR(p);
      break;
    case _SETTING:
      c = new CommandSetting(p);
      break;
    case _SETTING_GET:
      c = new CommandSettingGet(p);
      break;
    case _SHIFT:
      c = new CommandShift(p);
      break;
    case _STDDEV:
      c = new CommandSTDDEV(p);
      break;
    case _STOCH_FAST:
      c = new CommandStochFast(p);
      break;
    case _STOCH_RSI:
      c = new CommandStochRSI(p);
      break;
    case _STOCH_SLOW:
      c = new CommandStochSlow(p);
      break;
    case _SZ:
      c = new CommandSZ(p);
      break;
    case _SYMBOL:
      c = new CommandSymbol(p);
      break;
    case _SYMBOL_CURRENT:
      c = new CommandSymbolCurrent(p);
      break;
    case _T3:
      c = new CommandT3(p);
      break;
    case _THERM:
      c = new CommandTHERM(p);
      break;
    case _TRIX:
      c = new CommandTRIX(p);
      break;
    case _TYPICAL_PRICE:
      c = new CommandTypicalPrice(p);
      break;
    case _ULTOSC:
      c = new CommandULTOSC(p);
      break;
    case _VAR:
      c = new CommandVAR(p);
      break;
    case _VBP:
      c = new CommandVBP(p);
      break;
    case _VFI:
      c = new CommandVFI(p);
      break;
    case _VIDYA:
      c = new CommandVIDYA(p);
      break;
    case _WEIGHTED_CLOSE:
      c = new CommandWeightedClose(p);
      break;
    case _WILLR:
      c = new CommandWILLR(p);
      break;
    case _YAHOO_HISTORY:
      c = new CommandYahooHistory(p);
      break;
    default:
      break;
  }

  return c;
}

QStringList CommandFactory::list ()
{
  return _types;
}

int CommandFactory::stringToType (QString d)
{
  return _types.indexOf(d);
}
