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
#include "TypeCommand.h"
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
#include "CommandCompare.h"
#include "CommandCORREL.h"
#include "CommandCSV.h"
#include "CommandDataBase.h"
#include "CommandDialog.h"
#include "CommandDialogMessage.h"
#include "CommandDialogNew.h"
#include "CommandDialogUser.h"
#include "CommandFI.h"
#include "CommandGet.h"
#include "CommandGroupDataBase.h"
#include "CommandGroupPanelRefresh.h"
#include "CommandHLine.h"
#include "CommandHT.h"
#include "CommandLINEARREG.h"
#include "CommandLoad.h"
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
#include "CommandRetracement.h"
#include "CommandROC.h"
#include "CommandRSI.h"
#include "CommandRun.h"
#include "CommandSAR.h"
#include "CommandSave.h"
#include "CommandScriptDataBase.h"
#include "CommandSell.h"
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
#include "CommandText.h"
#include "CommandTHERM.h"
#include "CommandTLine.h"
#include "CommandTRIX.h"
#include "CommandTypicalPrice.h"
#include "CommandULTOSC.h"
#include "CommandVAR.h"
//#include "CommandVBP.h"
#include "CommandVFI.h"
#include "CommandVIDYA.h"
#include "CommandVLine.h"
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
  TypeCommand ct;

  switch ((TypeCommand::Type) ct.stringToIndex(type))
  {
    case TypeCommand::_AD:
      c = new CommandAD;
      break;
    case TypeCommand::_ADX:
      c = new CommandADX;
      break;
    case TypeCommand::_ARITHMETIC:
      c = new CommandArithmetic;
      break;
    case TypeCommand::_AROON:
      c = new CommandAROON;
      break;
    case TypeCommand::_ATR:
      c = new CommandATR;
      break;
    case TypeCommand::_AVERAGE_PRICE:
      c = new CommandAveragePrice;
      break;
    case TypeCommand::_BBANDS:
      c = new CommandBBANDS;
      break;
    case TypeCommand::_BETA:
      c = new CommandBETA;
      break;
    case TypeCommand::_BOP:
      c = new CommandBOP;
      break;
    case TypeCommand::_BREAKOUT:
      c = new CommandBreakout;
      break;
    case TypeCommand::_CANDLE_PATTERN:
      c = new CommandCandlePattern;
      break;
    case TypeCommand::_CCI:
      c = new CommandCCI;
      break;
    case TypeCommand::_CHART:
      c = new CommandChart;
      break;
    case TypeCommand::_CHART_OBJECT_BUY:
      c = new CommandBuy;
      break;
    case TypeCommand::_CHART_OBJECT_HLINE:
      c = new CommandHLine;
      break;
    case TypeCommand::_CHART_OBJECT_RETRACEMENT:
      c = new CommandRetracement;
      break;
    case TypeCommand::_CHART_OBJECT_SELL:
      c = new CommandSell;
      break;
    case TypeCommand::_CHART_OBJECT_TEXT:
      c = new CommandText;
      break;
    case TypeCommand::_CHART_OBJECT_TLINE:
      c = new CommandTLine;
      break;
    case TypeCommand::_CHART_OBJECT_VLINE:
      c = new CommandVLine;
      break;
    case TypeCommand::_CHART_PANEL_REFRESH:
      c = new CommandChartPanelRefresh;
      break;
    case TypeCommand::_CHART_UPDATE:
      c = new CommandChartUpdate;
      break;
    case TypeCommand::_CMO:
      c = new CommandCMO;
      break;
    case TypeCommand::_COMPARE:
      c = new CommandCompare;
      break;
    case TypeCommand::_CORREL:
      c = new CommandCORREL;
      break;
    case TypeCommand::_CSV:
      c = new CommandCSV;
      break;
    case TypeCommand::_DATABASE:
      c = new CommandDataBase;
      break;
    case TypeCommand::_DIALOG:
      c = new CommandDialog;
      break;
    case TypeCommand::_DIALOG_MESSAGE:
      c = new CommandDialogMessage;
      break;
    case TypeCommand::_DIALOG_NEW:
      c = new CommandDialogNew;
      break;
    case TypeCommand::_DIALOG_USER:
      c = new CommandDialogUser;
      break;
    case TypeCommand::_FI:
      c = new CommandFI;
      break;
    case TypeCommand::_GET:
      c = new CommandGet;
      break;
    case TypeCommand::_GROUP_DATA_BASE:
      c = new CommandGroupDataBase;
      break;
    case TypeCommand::_GROUP_PANEL_REFRESH:
      c = new CommandGroupPanelRefresh;
      break;
    case TypeCommand::_HT:
      c = new CommandHT;
      break;
    case TypeCommand::_HT_PHASOR:
      c = new CommandPHASOR;
      break;
    case TypeCommand::_HT_SINE:
      c = new CommandSINE;
      break;
    case TypeCommand::_LINEARREG:
      c = new CommandLINEARREG;
      break;
    case TypeCommand::_LOAD:
      c = new CommandLoad;
      break;
    case TypeCommand::_MA:
      c = new CommandMA;
      break;
    case TypeCommand::_MACD:
      c = new CommandMACD;
      break;
    case TypeCommand::_MAMA:
      c = new CommandMAMA;
      break;
    case TypeCommand::_MEDIAN_PRICE:
      c = new CommandMedianPrice;
      break;
    case TypeCommand::_MINMAX:
      c = new CommandMINMAX;
      break;
    case TypeCommand::_MFI:
      c = new CommandMFI;
      break;
    case TypeCommand::_MOM:
      c = new CommandMOM;
      break;
    case TypeCommand::__NEW:
      c = new CommandNew;
      break;
    case TypeCommand::_NEW_HIGH_LOW:
      c = new CommandNewHighLow;
      break;
    case TypeCommand::_NORMALIZE:
      c = new CommandNormalize;
      break;
    case TypeCommand::_PO:
      c = new CommandPO;
      break;
    case TypeCommand::_PLOT_LINE:
      c = new CommandPlotLine;
      break;
    case TypeCommand::_PLOT_HISTOGRAM:
      c = new CommandPlotHistogram;
      break;
    case TypeCommand::_PLOT_OHLC:
      c = new CommandPlotOHLC;
      break;
    case TypeCommand::_ROC:
      c = new CommandROC;
      break;
    case TypeCommand::_RSI:
      c = new CommandRSI;
      break;
    case TypeCommand::_RUN:
      c = new CommandRun;
      break;
    case TypeCommand::_SAR:
      c = new CommandSAR;
      break;
    case TypeCommand::_SAVE:
      c = new CommandSave;
      break;
    case TypeCommand::_SCRIPT_DATA_BASE:
      c = new CommandScriptDataBase;
      break;
    case TypeCommand::_SET:
      c = new CommandSet;
      break;
    case TypeCommand::_SHIFT:
      c = new CommandShift;
      break;
    case TypeCommand::_STDDEV:
      c = new CommandSTDDEV;
      break;
    case TypeCommand::_STOCH_FAST:
      c = new CommandStochFast;
      break;
    case TypeCommand::_STOCH_RSI:
      c = new CommandStochRSI;
      break;
    case TypeCommand::_STOCH_SLOW:
      c = new CommandStochSlow;
      break;
    case TypeCommand::_SZ:
      c = new CommandSZ;
      break;
    case TypeCommand::_SYMBOL:
      c = new CommandSymbol;
      break;
    case TypeCommand::_SYMBOL_CURRENT:
      c = new CommandSymbolCurrent;
      break;
    case TypeCommand::_T3:
      c = new CommandT3;
      break;
    case TypeCommand::_TEST:
//      c = new CommandTest;
      break;
    case TypeCommand::_THERM:
      c = new CommandTHERM;
      break;
    case TypeCommand::_TRIX:
      c = new CommandTRIX;
      break;
    case TypeCommand::_TYPICAL_PRICE:
      c = new CommandTypicalPrice;
      break;
    case TypeCommand::_ULTOSC:
      c = new CommandULTOSC;
      break;
    case TypeCommand::_VAR:
      c = new CommandVAR;
      break;
    case TypeCommand::_VBP:
//      c = new CommandVBP;
      break;
    case TypeCommand::_VFI:
      c = new CommandVFI;
      break;
    case TypeCommand::_VIDYA:
      c = new CommandVIDYA;
      break;
    case TypeCommand::_WEIGHTED_CLOSE:
      c = new CommandWeightedClose;
      break;
    case TypeCommand::_WILLR:
      c = new CommandWILLR;
      break;
    case TypeCommand::_YAHOO_HISTORY:
      c = new CommandYahooHistory;
      break;
    default:
      break;
  }

  return c;
}
