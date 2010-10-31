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

#include "STOCH.h"
#include "FunctionMA.h"
#include "STOCHDialog.h"
#include "Curve.h"
#include "ta_libc.h"

#include <QtDebug>

STOCH::STOCH ()
{
  _indicator = "STOCH";
}

int STOCH::getIndicator (Indicator &ind, BarData &data)
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

  // create the fastk line
  int kperiod = settings.getInt(_FastKPeriod);
  int dperiod = settings.getInt(_FastDPeriod);

  FunctionMA mau;
  settings.getData(_FastDMA, s);
  int maType = mau.typeFromString(s);

  QList<Curve *> pl;
  if (calculate(kperiod, dperiod, maType, pl, data))
    return 1;

  line = pl.at(0);

  settings.getData(_FastKPlot, s);
  line->setType((Curve::Type) line->typeFromString(s));

  settings.getData(_FastKColor, s);
  color.setNamedColor(s);
  line->setColor(color);

  settings.getData(_FastKLabel, s);
  line->setLabel(s);
  
  line->setZ(2);
  ind.setLine(s, line);
  
  line = pl.at(1);

  settings.getData(_FastDPlot, s);
  line->setType((Curve::Type) line->typeFromString(s));

  settings.getData(_FastDColor, s);
  color.setNamedColor(s);
  line->setColor(color);

  settings.getData(_FastDLabel, s);
  line->setLabel(s);
  
  line->setZ(3);
  ind.setLine(s, line);

  return 0;
}

int STOCH::getCUS (QStringList &set, Indicator &ind, BarData &data)
{
  // INDICATOR,PLUGIN,STOCH,<NAME FASTK>,<NAME FASTD>,<FASTK PERIOD>,<FASTD PERIOD>,<FASTD MA TYPE>
  //     0        1    2         3            4              5             6              7

  if (set.count() != 8)
  {
    qDebug() << "STOCH::script: invalid settings count" << set.count();
    return 1;
  }

  Curve *tl = ind.line(set[3]);
  if (tl)
  {
    qDebug() << "STOCH::script: duplicate fastk name" << set[3];
    return 1;
  }

  tl = ind.line(set[4]);
  if (tl)
  {
    qDebug() << "STOCH::script: duplicate fastd name" << set[4];
    return 1;
  }

  bool ok;
  int fkp = set[5].toInt(&ok);
  if (! ok)
  {
    qDebug() << "STOCH::script: invalid fastk period" << set[5];
    return 1;
  }

  int fdp = set[6].toInt(&ok);
  if (! ok)
  {
    qDebug() << "STOCH::script: invalid fastd period" << set[6];
    return 1;
  }

  FunctionMA mau;
  int ma = mau.typeFromString(set[7]);
  if (ma == -1)
  {
    qDebug() << "STOCH::script: invalid fastd ma" << set[7];
    return 1;
  }

  QList<Curve *> pl;
  if (calculate(fkp, fdp, ma, pl, data))
    return 1;

  Curve *line = pl.at(0);
  line->setLabel(set[3]);
  ind.setLine(set[3], line);

  line = pl.at(1);
  line->setLabel(set[4]);
  ind.setLine(set[4], line);

  return 0;
}

IndicatorPluginDialog * STOCH::dialog (Indicator &i)
{
  return new STOCHDialog(i);
}

void STOCH::defaults (Indicator &i)
{
  Setting set;
  set.setData(_FastKColor, "red");
  set.setData(_FastDColor, "yellow");
  set.setData(_Ref1Color, "white");
  set.setData(_Ref2Color, "white");
  set.setData(_FastKPlot, "Line");
  set.setData(_FastDPlot, "Line");
  set.setData(_FastKLabel, "FASTK");
  set.setData(_FastDLabel, "FASTD");
  set.setData(_FastKPeriod, 5);
  set.setData(_FastDPeriod, 3);
  set.setData(_FastDMA, "SMA");
  set.setData(_Ref1, 20);
  set.setData(_Ref2, 80);
  i.setSettings(set);
}

void STOCH::plotNames (Indicator &i, QStringList &l)
{
  l.clear();

  Setting settings = i.settings();
  QString s;
  
  settings.getData(_FastKLabel, s);
  l.append(s);

  settings.getData(_FastDLabel, s);
  l.append(s);
}

int STOCH::calculate (int kperiod, int dperiod, int ma, QList<Curve *> &pl, BarData &data)
{
  int size = data.count();

  if (size < kperiod || size < dperiod)
    return 1;

  TA_Real out[size];
  TA_Real out2[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  TA_RetCode rc = TA_STOCHF(0,
                            size - 1,
                            data.getTAData(BarData::High),
                            data.getTAData(BarData::Low),
                            data.getTAData(BarData::Close),
                            kperiod,
                            dperiod,
                            (TA_MAType) ma,
                            &outBeg,
                            &outNb,
                            &out[0],
                            &out2[0]);
  if (rc != TA_SUCCESS)
  {
    qDebug() << "STOCH::calculate: TA-Lib error" << rc;
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
  STOCH *o = new STOCH;
  return ((IndicatorPlugin *) o);
}


