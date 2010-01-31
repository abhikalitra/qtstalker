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

#include "IndicatorBase.h"

#include <QtDebug>
#include <QObject>


IndicatorBase::IndicatorBase ()
{
  colorKey = QObject::tr("Color");
  plotKey = QObject::tr("Plot");
  labelKey = QObject::tr("Label");
  inputKey = QObject::tr("Input");
  periodKey = QObject::tr("Period");
  maKey = QObject::tr("MA Type");

  PlotLine pl;
  pl.getLineTypes(plotList);

  BarData bd;
  bd.getInputFields(inputList);

  maList << "SMA" << "EMA" << "WMA" << "DEMA" << "TEMA" << "TRIMA" << "KAMA" << "MAMA" << "T3";

  opList << "=" << "<" << "<=" << ">" << ">=" << "AND" << "OR";

  indicatorList << "AD";
  indicatorList << "ADOSC";
  indicatorList << "ADX";
  indicatorList << "APO";
  indicatorList << "AROON";
  indicatorList << "AROONOSC";
  indicatorList << "ATR";
  indicatorList << "BARS";
  indicatorList << "BBANDS";
  indicatorList << "BETA";
  indicatorList << "BOP";
  indicatorList << "CANDLES";
  indicatorList << "CCI";
  indicatorList << "CDL";
  indicatorList << "CMO";
  indicatorList << "COLOR";
  indicatorList << "COMPARE";
  indicatorList << "COMPARE2";
  indicatorList << "CORREL";
  indicatorList << "CUS";
  indicatorList << "DEMA";
  indicatorList << "DX";
  indicatorList << "FI";
  indicatorList << "HT_DCPERIOD";
  indicatorList << "HT_DCPHASE";
  indicatorList << "HT_PHASOR";
  indicatorList << "HT_SINE";
  indicatorList << "HT_TRENDLINE";
  indicatorList << "HT_TRENDMODE";
  indicatorList << "LINEARREG";
  indicatorList << "LINEARREG_ANGLE";
  indicatorList << "LINEARREG_INTERCEPT";
  indicatorList << "LINEARREG_SLOPE";
  indicatorList << "LMS";
  indicatorList << "MA";
  indicatorList << "MACD";
  indicatorList << "MACDEXT";
  indicatorList << "MACDFIX";
  indicatorList << "MAMA";
  indicatorList << "MATH";
  indicatorList << "MATH2";
  indicatorList << "MAVP";
  indicatorList << "MAX";
  indicatorList << "MAXINDEX";
  indicatorList << "MFI";
  indicatorList << "MIDPOINT";
  indicatorList << "MIDPRICE";
  indicatorList << "MIN";
  indicatorList << "MININDEX";
  indicatorList << "MINMAX";
  indicatorList << "MINMAXINDEX";
  indicatorList << "MINUS_DI";
  indicatorList << "MINUS_DM";
  indicatorList << "MOM";
  indicatorList << "NATR";
  indicatorList << "PLUS_DI";
  indicatorList << "PLUS_DM";
  indicatorList << "PP";
  indicatorList << "PPO";
  indicatorList << "PRICE";
  indicatorList << "REF";
  indicatorList << "ROC";
  indicatorList << "RSI";
  indicatorList << "SAR";
  indicatorList << "SAREXT";
  indicatorList << "SINWAV";
  indicatorList << "STDDEV";
  indicatorList << "STOCH";
  indicatorList << "STOCHF";
  indicatorList << "STOCHRSI";
  indicatorList << "SUM";
  indicatorList << "SYMBOL";
  indicatorList << "SZ";
  indicatorList << "T3";
  indicatorList << "THERM";
  indicatorList << "TRANGE";
  indicatorList << "TRIG";
  indicatorList << "TRIX";
  indicatorList << "TSF";
  indicatorList << "ULTOSC";
  indicatorList << "VAR";
  indicatorList << "VFI";
  indicatorList << "VIDYA";
  indicatorList << "VOL";
  indicatorList << "WILLR";
}

IndicatorBase::~IndicatorBase ()
{
}

// virtual function
int IndicatorBase::getIndicator (Indicator &, BarData *)
{
  return 0;
}

// virtual function
int IndicatorBase::getCUS (QStringList &, QHash<QString, PlotLine *> &, BarData *)
{
  return 0;
}

// virtual function
int IndicatorBase::dialog ()
{
  return 0;
}

PlotLine * IndicatorBase::getMA (PlotLine *_in)
{
  TA_Integer outBeg;
  TA_Integer outNb;
  TA_Real input[_in->getSize()];
  TA_Real out[_in->getSize()];
  int loop;
  for (loop = 0; loop < _in->getSize(); loop++)
    input[loop] = (TA_Real) _in->getData(loop);

  TA_RetCode rc = TA_MA(0, _in->getSize() - 1, &input[0], maPeriod, (TA_MAType) maType, &outBeg, &outNb, &out[0]);
  if (rc != TA_SUCCESS)
  {
    qDebug() << "IndicatorBase::getMA: TA-Lib error" << rc;
    return 0;
  }

  PlotLine *line = new PlotLine;
  for (loop = 0; loop < outNb; loop++)
    line->append(out[loop]);

  return line;
}

void IndicatorBase::setSettings (Indicator &_ind)
{
  _ind.getSettings(settings);
  _ind.getIndicator(indicator);
}

void IndicatorBase::getSettings (Indicator &_ind)
{
  _ind.setIndicator(indicator);
  _ind.setSettings(settings);
}

void IndicatorBase::getIndicatorList (QStringList &l)
{
  l = indicatorList;
  l.removeAll("COLOR");
  l.removeAll("COMPARE");
  l.removeAll("COMPARE2");
  l.removeAll("MATH");
  l.removeAll("MATH2");
  l.removeAll("REF");
  l.removeAll("SYMBOL");
  l.removeAll("TRIG");
}

void IndicatorBase::getMAList (QStringList &l)
{
  l = maList;
}


