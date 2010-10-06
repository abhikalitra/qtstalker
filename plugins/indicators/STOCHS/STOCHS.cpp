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

#include "STOCHS.h"
#include "FunctionMA.h"
#include "STOCHSDialog.h"
#include "Curve.h"
#include "ta_libc.h"

#include <QtDebug>

STOCHS::STOCHS ()
{
  _indicator = "STOCHS";
}

int STOCHS::getIndicator (Indicator &ind, BarData &data)
{
  Setting settings = ind.settings();

  // create first ref line
  QString s;
  Curve *line = new Curve;
  line->setType(Curve::Horizontal);

  settings.getData(_Ref1Color, s);
  QColor color(s);

  line->setBar(0, new CurveBar(color, (double) settings.getInt(_Ref1)));
  
  line->setZ(0);
  ind.setLine(0, line);

  // create second ref line
  line = new Curve;
  line->setType(Curve::Horizontal);

  settings.getData(_Ref2Color, s);
  color.setNamedColor(s);

  line->setBar(0, new CurveBar(color, (double) settings.getInt(_Ref2)));
  
  line->setZ(1);
  ind.setLine(1, line);

  int fkperiod = settings.getInt(_FastKPeriod);
  int skperiod = settings.getInt(_SlowKPeriod);

  FunctionMA mau;
  settings.getData(_SlowKMA, s);
  int kmaType = mau.typeFromString(s);

  int dperiod = settings.getInt(_SlowDPeriod);

  settings.getData(_SlowDMA, s);
  int dmaType = mau.typeFromString(s);

  QList<Curve *> pl;
  if (calculate(fkperiod, skperiod, dperiod, kmaType, dmaType, pl, data))
    return 1;

  line = pl.at(0);

  settings.getData(_SlowKPlot, s);
  line->setType((Curve::Type) line->typeFromString(s));

  settings.getData(_SlowKColor, s);
  color.setNamedColor(s);
  line->setColor(color);

  settings.getData(_SlowKLabel, s);
  line->setLabel(s);
  
  line->setZ(2);
  ind.setLine(s, line);

  line = pl.at(1);

  settings.getData(_SlowDPlot, s);
  line->setType((Curve::Type) line->typeFromString(s));

  settings.getData(_SlowDColor, s);
  color.setNamedColor(s);
  line->setColor(color);

  settings.getData(_SlowDLabel, s);
  line->setLabel(s);
  
  line->setZ(3);
  ind.setLine(s, line);

  return 0;
}

int STOCHS::getCUS (QStringList &set, Indicator &ind, BarData &data)
{
  // INDICATOR,PLUGIN,STOCHS,<NAME SLOWK>,<NAME SLOWD>,<FASTK PERIOD>,<SLOWK PERIOD>,<SLOWK MA TYPE>,<SLOWD PERIOD>,<SLOWD MA TYPE>
  //     0       1      2         3            4             5              6               7              8              9

  if (set.count() != 10)
  {
    qDebug() << "STOCHS::getCUS: invalid settings count" << set.count();
    return 1;
  }

  Curve *tl = ind.line(set[3]);
  if (tl)
  {
    qDebug() << "STOCHS::getCUS: duplicate name" << set[3];
    return 1;
  }

  tl = ind.line(set[4]);
  if (tl)
  {
    qDebug() << "STOCHS::getCUS: duplicate name" << set[4];
    return 1;
  }

  bool ok;
  int fkp = set[5].toInt(&ok);
  if (! ok)
  {
    qDebug() << "STOCHS::getCUS: invalid fastk period" << set[5];
    return 1;
  }

  int skp = set[6].toInt(&ok);
  if (! ok)
  {
    qDebug() << "STOCHS::getCUS: invalid slowk period" << set[6];
    return 1;
  }

  FunctionMA mau;
  int kma = mau.typeFromString(set[7]);
  if (kma == -1)
  {
    qDebug() << "STOCHS::getCUS: invalid slowk ma" << set[7];
    return 1;
  }

  int sdp = set[8].toInt(&ok);
  if (! ok)
  {
    qDebug() << "STOCHS::getCUS: invalid slowd period" << set[8];
    return 1;
  }

  int dma = mau.typeFromString(set[9]);
  if (dma == -1)
  {
    qDebug() << "STOCHS::getCUS: invalid slowd ma" << set[9];
    return 1;
  }

  QList<Curve *> pl;
  if (calculate(fkp, skp, sdp, kma, dma, pl, data))
    return 1;

  Curve *line = pl.at(0);
  line->setLabel(set[3]);
  ind.setLine(set[3], line);

  line = pl.at(1);
  line->setLabel(set[4]);
  ind.setLine(set[4], line);

  return 0;
}

IndicatorPluginDialog * STOCHS::dialog (Indicator &i)
{
  return new STOCHSDialog(i);
}

void STOCHS::defaults (Indicator &i)
{
  Setting set;
  set.setData(_SlowKColor, "red");
  set.setData(_SlowDColor, "yellow");
  set.setData(_Ref1Color, "white");
  set.setData(_Ref2Color, "white");
  set.setData(_SlowKPlot, "Line");
  set.setData(_SlowDPlot, "Dash");
  set.setData(_SlowKLabel, "SLOWK");
  set.setData(_SlowDLabel, "SLOWD");
  set.setData(_FastKPeriod, 5);
  set.setData(_SlowKPeriod, 3);
  set.setData(_SlowDPeriod, 3);
  set.setData(_SlowKMA, "SMA");
  set.setData(_SlowDMA, "SMA");
  set.setData(_Ref1, 20);
  set.setData(_Ref2, 80);
  i.setSettings(set);
}

void STOCHS::plotNames (Indicator &i, QStringList &l)
{
  l.clear();

  Setting settings = i.settings();
  QString s;

  settings.getData(_SlowKLabel, s);
  l.append(s);

  settings.getData(_SlowDLabel, s);
  l.append(s);
}

int STOCHS::calculate (int fkperiod, int skperiod, int sdperiod, int kma, int dma, QList<Curve *> &pl, BarData &data)
{
  int size = data.count();

  if (size < fkperiod || size < skperiod)
    return 1;

  TA_Real out[size];
  TA_Real out2[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  TA_RetCode rc = TA_STOCH(0,
                           size - 1,
                           data.getTAData(BarData::High),
                           data.getTAData(BarData::Low),
                           data.getTAData(BarData::Close),
                           fkperiod,
                           skperiod,
                           (TA_MAType) kma,
                           sdperiod,
                           (TA_MAType) dma,
                           &outBeg,
                           &outNb,
                           &out[0],
                           &out2[0]);

  if (rc != TA_SUCCESS)
  {
    qDebug() << "STOCHS::calculate: TA-Lib error" << rc;
    return 1;
  }

  Curve *kline = new Curve;
  Curve *dline = new Curve;

  int dataLoop = size - 1;
  int outLoop = outNb - 1;
  while (outLoop > -1 && dataLoop > -1)
  {
    kline->setBar(dataLoop, new CurveBar(out[outLoop]));
    dline->setBar(dataLoop, new CurveBar(out2[outLoop]));

    dataLoop--;
    outLoop--;
  }

  pl.append(kline);
  pl.append(dline);

  return 0;
}


//*************************************************************
//*************************************************************
//*************************************************************

IndicatorPlugin * createIndicatorPlugin ()
{
  STOCHS *o = new STOCHS;
  return ((IndicatorPlugin *) o);
}


