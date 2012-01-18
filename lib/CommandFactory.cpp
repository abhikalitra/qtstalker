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
#include "CommandType.h"

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
//#include "CommandBuy.h"
#include "CommandCandlePattern.h"
#include "CommandCCI.h"
#include "CommandChart.h"
#include "CommandChartPanelRefresh.h"
#include "CommandChartUpdate.h"
#include "CommandCMO.h"
#include "CommandCompare.h"
#include "CommandCORREL.h"
#include "CommandCSV.h"
#include "CommandDialog.h"
#include "CommandFI.h"
#include "CommandGet.h"
#include "CommandGroupPanelRefresh.h"
#include "CommandGroupSave.h"
//#include "CommandHLine.h"
#include "CommandHT.h"
#include "CommandLINEARREG.h"
#include "CommandMA.h"
#include "CommandMACD.h"
#include "CommandMAMA.h"
#include "CommandMedianPrice.h"
#include "CommandMINMAX.h"
#include "CommandMFI.h"
#include "CommandMOM.h"
#include "CommandNew.h"
#include "CommandNewHighLow.h"
#include "CommandNormalize.h"
#include "CommandPHASOR.h"
#include "CommandPO.h"
#include "CommandPlotOHLC.h"
#include "CommandPlotLine.h"
#include "CommandPlotHistogram.h"
//#include "CommandRetracement.h"
#include "CommandROC.h"
#include "CommandRSI.h"
#include "CommandRun.h"
#include "CommandSAR.h"
//#include "CommandSell.h"
#include "CommandSet.h"
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
//#include "CommandTest.h"
//#include "CommandText.h"
#include "CommandTHERM.h"
//#include "CommandTLine.h"
#include "CommandTRIX.h"
#include "CommandTypicalPrice.h"
#include "CommandULTOSC.h"
#include "CommandVAR.h"
//#include "CommandVBP.h"
#include "CommandVFI.h"
#include "CommandVIDYA.h"
//#include "CommandVLine.h"
#include "CommandWeightedClose.h"
#include "CommandWILLR.h"
#include "CommandYahooHistory.h"

#include <QDebug>

CommandFactory::CommandFactory ()
{
}

Command * CommandFactory::command (QString type)
{
  Command *c = 0;
  CommandType ct;

  switch ((CommandType::Type) ct.stringToType(type))
  {
    case CommandType::_AD:
      c = new CommandAD;
      break;
    case CommandType::_ADX:
      c = new CommandADX;
      break;
    case CommandType::_ARITHMETIC:
      c = new CommandArithmetic;
      break;
    case CommandType::_AROON:
      c = new CommandAROON;
      break;
    case CommandType::_ATR:
      c = new CommandATR;
      break;
    case CommandType::_AVERAGE_PRICE:
      c = new CommandAveragePrice;
      break;
    case CommandType::_BBANDS:
      c = new CommandBBANDS;
      break;
    case CommandType::_BETA:
      c = new CommandBETA;
      break;
    case CommandType::_BOP:
      c = new CommandBOP;
      break;
    case CommandType::_BREAKOUT:
      c = new CommandBreakout;
      break;
    case CommandType::_CANDLE_PATTERN:
      c = new CommandCandlePattern;
      break;
    case CommandType::_CCI:
      c = new CommandCCI;
      break;
    case CommandType::_CHART:
      c = new CommandChart;
      break;
    case CommandType::_CHART_OBJECT_BUY:
//      c = new CommandBuy;
      break;
    case CommandType::_CHART_OBJECT_HLINE:
//      c = new CommandHLine;
      break;
    case CommandType::_CHART_OBJECT_RETRACEMENT:
//      c = new CommandRetracement;
      break;
    case CommandType::_CHART_OBJECT_SELL:
//      c = new CommandSell;
      break;
    case CommandType::_CHART_OBJECT_TEXT:
//      c = new CommandText;
      break;
    case CommandType::_CHART_OBJECT_TLINE:
//      c = new CommandTLine;
      break;
    case CommandType::_CHART_OBJECT_VLINE:
//      c = new CommandVLine;
      break;
    case CommandType::_CHART_PANEL_REFRESH:
      c = new CommandChartPanelRefresh;
      break;
    case CommandType::_CHART_UPDATE:
      c = new CommandChartUpdate;
      break;
    case CommandType::_CMO:
      c = new CommandCMO;
      break;
    case CommandType::_COMPARE:
      c = new CommandCompare;
      break;
    case CommandType::_CORREL:
      c = new CommandCORREL;
      break;
    case CommandType::_CSV:
      c = new CommandCSV;
      break;
    case CommandType::_DIALOG:
      c = new CommandDialog;
      break;
    case CommandType::_FI:
      c = new CommandFI;
      break;
    case CommandType::_GET:
      c = new CommandGet;
      break;
    case CommandType::_GROUP_PANEL_REFRESH:
      c = new CommandGroupPanelRefresh;
      break;
    case CommandType::_GROUP_SAVE:
      c = new CommandGroupSave;
      break;
    case CommandType::_HT:
      c = new CommandHT;
      break;
    case CommandType::_HT_PHASOR:
      c = new CommandPHASOR;
      break;
    case CommandType::_HT_SINE:
      c = new CommandSINE;
      break;
    case CommandType::_LINEARREG:
      c = new CommandLINEARREG;
      break;
    case CommandType::_MA:
      c = new CommandMA;
      break;
    case CommandType::_MACD:
      c = new CommandMACD;
      break;
    case CommandType::_MAMA:
      c = new CommandMAMA;
      break;
    case CommandType::_MEDIAN_PRICE:
      c = new CommandMedianPrice;
      break;
    case CommandType::_MINMAX:
      c = new CommandMINMAX;
      break;
    case CommandType::_MFI:
      c = new CommandMFI;
      break;
    case CommandType::_MOM:
      c = new CommandMOM;
      break;
    case CommandType::__NEW:
      c = new CommandNew;
      break;
    case CommandType::_NEW_HIGH_LOW:
      c = new CommandNewHighLow;
      break;
    case CommandType::_NORMALIZE:
      c = new CommandNormalize;
      break;
    case CommandType::_PO:
      c = new CommandPO;
      break;
    case CommandType::_PLOT_LINE:
      c = new CommandPlotLine;
      break;
    case CommandType::_PLOT_HISTOGRAM:
      c = new CommandPlotHistogram;
      break;
    case CommandType::_PLOT_OHLC:
      c = new CommandPlotOHLC;
      break;
    case CommandType::_ROC:
      c = new CommandROC;
      break;
    case CommandType::_RSI:
      c = new CommandRSI;
      break;
    case CommandType::_RUN:
      c = new CommandRun;
      break;
    case CommandType::_SAR:
      c = new CommandSAR;
      break;
    case CommandType::_SET:
      c = new CommandSet;
      break;
    case CommandType::_SHIFT:
      c = new CommandShift;
      break;
    case CommandType::_STDDEV:
      c = new CommandSTDDEV;
      break;
    case CommandType::_STOCH_FAST:
      c = new CommandStochFast;
      break;
    case CommandType::_STOCH_RSI:
      c = new CommandStochRSI;
      break;
    case CommandType::_STOCH_SLOW:
      c = new CommandStochSlow;
      break;
    case CommandType::_SZ:
      c = new CommandSZ;
      break;
    case CommandType::_SYMBOL:
      c = new CommandSymbol;
      break;
    case CommandType::_SYMBOL_CURRENT:
      c = new CommandSymbolCurrent;
      break;
    case CommandType::_T3:
      c = new CommandT3;
      break;
    case CommandType::_TEST:
//      c = new CommandTest;
      break;
    case CommandType::_THERM:
      c = new CommandTHERM;
      break;
    case CommandType::_TRIX:
      c = new CommandTRIX;
      break;
    case CommandType::_TYPICAL_PRICE:
      c = new CommandTypicalPrice;
      break;
    case CommandType::_ULTOSC:
      c = new CommandULTOSC;
      break;
    case CommandType::_VAR:
      c = new CommandVAR;
      break;
    case CommandType::_VBP:
//      c = new CommandVBP;
      break;
    case CommandType::_VFI:
      c = new CommandVFI;
      break;
    case CommandType::_VIDYA:
      c = new CommandVIDYA;
      break;
    case CommandType::_WEIGHTED_CLOSE:
      c = new CommandWeightedClose;
      break;
    case CommandType::_WILLR:
      c = new CommandWILLR;
      break;
    case CommandType::_YAHOO_HISTORY:
      c = new CommandYahooHistory;
      break;
    default:
      break;
  }

  return c;
}
