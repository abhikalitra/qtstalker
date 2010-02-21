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
  PlotLine pl;
  pl.getLineTypes(plotList);

  BarData bd;
  bd.getInputFields(inputList);

  maList << "SMA" << "EMA" << "WMA" << "DEMA" << "TEMA" << "TRIMA" << "KAMA" << "MAMA" << "T3";

  opList << "=" << "<" << "<=" << ">" << ">=" << "AND" << "OR";

  indicatorList << "AD";
  indicatorList << "ADX";
  indicatorList << "AROON";
  indicatorList << "ATR";
  indicatorList << "BARS";
  indicatorList << "BBANDS";
  indicatorList << "BETA";
  indicatorList << "BOP";
  indicatorList << "CANDLES";
  indicatorList << "CCI";
  indicatorList << "CMO";
  indicatorList << "COLOR";
  indicatorList << "COMPARE";
  indicatorList << "CORREL";
  indicatorList << "CUS";
  indicatorList << "FI";
  indicatorList << "HT";
  indicatorList << "LINEARREG";
  indicatorList << "LMS";
  indicatorList << "MA";
  indicatorList << "MACD";
  indicatorList << "MAMA";
  indicatorList << "MATH1";
  indicatorList << "MAVP";
  indicatorList << "MFI";
  indicatorList << "MOM";
  indicatorList << "PP";
  indicatorList << "PO";
  indicatorList << "RANGE";
  indicatorList << "REF";
  indicatorList << "ROC";
  indicatorList << "RSI";
  indicatorList << "SAR";
  indicatorList << "STOCH";
  indicatorList << "STOCHRSI";
  indicatorList << "SYMBOL";
  indicatorList << "SZ";
  indicatorList << "T3";
  indicatorList << "THERM";
  indicatorList << "TRIX";
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
int IndicatorBase::dialog (int)
{
  return 0;
}

// virtual function
int IndicatorBase::test (BarData *, TestSignal &)
{
  return 0;
}


PlotLine * IndicatorBase::getMA (PlotLine *_in, int period, int type)
{
  TA_Integer outBeg;
  TA_Integer outNb;
  TA_Real input[_in->getSize()];
  TA_Real out[_in->getSize()];
  int loop;
  for (loop = 0; loop < _in->getSize(); loop++)
    input[loop] = (TA_Real) _in->getData(loop);

  TA_RetCode rc = TA_MA(0, _in->getSize() - 1, &input[0], period, (TA_MAType) type, &outBeg, &outNb, &out[0]);
  if (rc != TA_SUCCESS)
  {
    qDebug() << "IndicatorBase::getMA: TA-Lib error" << rc;
    qDebug() << period << type;
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

void IndicatorBase::getIndicatorList (QStringList &l, int flag)
{
  l = indicatorList;

  if (flag)
  {
    l.removeAll("COLOR");
    l.removeAll("COMPARE");
    l.removeAll("MATH1");
    l.removeAll("RANGE");
    l.removeAll("REF");
    l.removeAll("SYMBOL");
  }
}

void IndicatorBase::getMAList (QStringList &l)
{
  l = maList;
}

void IndicatorBase::getDialogSettings (PrefDialog *dialog)
{
  QList<int> keys;
  dialog->getKeyList(keys);

  int loop;
  for (loop = 0; loop < keys.count(); loop++)
  {
    QString d;
    dialog->getItem(keys[loop], d);
    if (! d.isEmpty())
      settings.setData(keys[loop], d);
  }
}

void IndicatorBase::getIndicator (QString &d)
{
  d = indicator;
}

