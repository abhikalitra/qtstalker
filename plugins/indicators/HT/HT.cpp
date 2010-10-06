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

#include "HT.h"
#include "FunctionBARS.h"
#include "HTDialog.h"
#include "Curve.h"
#include "ta_libc.h"

#include <QtDebug>

HT::HT ()
{
  _indicator = "HT";
  _methodList << "DCPERIOD" << "DCPHASE" << "TRENDLINE" << "TRENDMODE" << "PHASOR" << "SINE";
}

int HT::getIndicator (Indicator &ind, BarData &data)
{
  Setting settings = ind.settings();

  QString s;
  settings.getData(_Input, s);
  Curve *in = data.getInput(data.getInputType(s));
  if (! in)
  {
    qDebug() << _indicator << "::getIndicator: input not found" << s;
    return 1;
  }

  settings.getData(_Method, s);
  int method = _methodList.indexOf(s);

  switch ((Method) method)
  {
    case _PHASOR:
    {
      QList<Curve *> pl;
      if (getPHASE(in, pl))
      {
        delete in;
        return 1;
      }

      Curve *line = pl.at(0);

      settings.getData(_PhasePlot, s);
      line->setType((Curve::Type) line->typeFromString(s));

      settings.getData(_PhaseColor, s);
      QColor c(s);
      line->setColor(c);

      settings.getData(_PhaseLabel, s);
      line->setLabel(s);

      line->setZ(0);
      ind.setLine(s, line);

      line = pl.at(1);

      settings.getData(_QuadPlot, s);
      line->setType((Curve::Type) line->typeFromString(s));

      settings.getData(_QuadColor, s);
      c.setNamedColor(s);
      line->setColor(c);

      settings.getData(_QuadLabel, s);
      line->setLabel(s);
      
      line->setZ(1);
      ind.setLine(s, line);
      break;
    }
    case _SINE:
    {
      QList<Curve *> pl;
      if (getSINE(in, pl))
      {
        delete in;
        return 1;
      }
      
      Curve *line = pl.at(0);

      settings.getData(_SinePlot, s);
      line->setType((Curve::Type) line->typeFromString(s));

      settings.getData(_SineColor, s);
      QColor c(s);
      line->setColor(c);

      settings.getData(_SineLabel, s);
      line->setLabel(s);
      
      line->setZ(2);
      ind.setLine(s, line);

      line = pl.at(1);

      settings.getData(_LeadPlot, s);
      line->setType((Curve::Type) line->typeFromString(s));

      settings.getData(_LeadColor, s);
      c.setNamedColor(s);
      line->setColor(c);

      settings.getData(_LeadLabel, s);
      line->setLabel(s);
      
      line->setZ(3);
      ind.setLine(s, line);
      break;
    }
    default:
    {
      QColor up("green");
      QColor down("red");
      QColor neutral("blue");
      FunctionBARS b;
      Curve *bars = b.getBARS(up, down, neutral, data);
      if (bars)
      {
        bars->setZ(4);
        ind.setLine(4, bars);
      }

      Curve *line = getHT(in, method);
      if (! line)
      {
        delete in;
        return 1;
      }

      settings.getData(_Plot, s);
      line->setType((Curve::Type) line->typeFromString(s));

      settings.getData(_Color, s);
      QColor c(s);
      line->setColor(c);

      settings.getData(_Label, s);
      line->setLabel(s);
      
      line->setZ(5);
      ind.setLine(s, line);
      break;
    }
  }

  delete in;
  return 0;
}

int HT::getCUS (QStringList &set, Indicator &ind, BarData &data)
{
  // INDICATOR,PLUGIN,HT,<METHOD>,*
  //     0       1     2    3

  if (set.count() < 4)
  {
    qDebug() << "HT::getCUS: invalid settings count" << set.count();
    return 1;
  }

  int rc = 1;
  int method = _methodList.indexOf(set[3]);

  switch ((Method) method)
  {
    case _DCPERIOD:
    case _DCPHASE:
    case _TRENDLINE:
    case _TRENDMODE:
      rc = scriptHT(set, ind, data);
      break;
    case _PHASOR:
      rc = scriptPHASE(set, ind, data);
      break;
    case _SINE:
      rc = scriptSINE(set, ind, data);
      break;
    default:
      break;
  }

  return rc;
}

IndicatorPluginDialog * HT::dialog (Indicator &i)
{
  return new HTDialog(i);
}

void HT::defaults (Indicator &i)
{
  Setting set;
  set.setData(_Method, "TRENDLINE");
  set.setData(_Color, "red");
  set.setData(_PhaseColor, "red");
  set.setData(_QuadColor, "yellow");
  set.setData(_SineColor, "red");
  set.setData(_LeadColor, "yellow");
  set.setData(_Plot, "Line");
  set.setData(_PhasePlot, "Line");
  set.setData(_QuadPlot, "Line");
  set.setData(_SinePlot, "Line");
  set.setData(_LeadPlot, "Line");
  set.setData(_Label, _indicator);
  set.setData(_PhaseLabel, "PHASE");
  set.setData(_QuadLabel, "QUAD");
  set.setData(_SineLabel, "SINE");
  set.setData(_LeadLabel, "LEAD");
  set.setData(_Input, "Close");
  i.setSettings(set);
}

void HT::plotNames (Indicator &i, QStringList &l)
{
  l.clear();

  Setting settings = i.settings();
  QString s;
  
  settings.getData(_Method, s);
  int method = _methodList.indexOf(s);

  switch ((Method) method)
  {
    case _PHASOR:
      settings.getData(_PhaseLabel, s);
      l.append(s);

      settings.getData(_QuadLabel, s);
      l.append(s);
      break;
    case _SINE:
      settings.getData(_SineLabel, s);
      l.append(s);

      settings.getData(_LeadLabel, s);
      l.append(s);
      break;
    default:
      settings.getData(_Label, s);
      l.append(s);
      break;
  }
}

int HT::scriptPHASE (QStringList &set, Indicator &ind, BarData &data)
{
  // INDICATOR,PLUGIN,HT,<METHOD>,<INPUT>,<PHASE NAME>,<QUAD NAME>
  //     0       1     2     3       4          5           6

  if (set.count() != 7)
  {
    qDebug() << "HT::scriptPHASE: invalid settings count" << set.count();
    return 1;
  }

  Curve *in = ind.line(set[4]);
  if (! in)
  {
    in = data.getInput(data.getInputType(set[4]));
    if (! in)
    {
      qDebug() << "HT::scriptPHASE: input not found" << set[4];
      return 1;
    }

    ind.setLine(set[4], in);
  }

  Curve *tl = ind.line(set[5]);
  if (tl)
  {
    qDebug() << "HT::scriptPHASE: duplicate phase name" << set[5];
    return 1;
  }

  tl = ind.line(set[6]);
  if (tl)
  {
    qDebug() << "HT::scriptPHASE: duplicate quad name" << set[6];
    return 1;
  }

  QList<Curve *> pl;
  if (getPHASE(in, pl))
    return 1;

  pl.at(0)->setLabel(set[5]);
  pl.at(1)->setLabel(set[6]);

  ind.setLine(set[5], pl.at(0));
  ind.setLine(set[6], pl.at(1));

  return 0;
}

int HT::scriptSINE (QStringList &set, Indicator &ind, BarData &data)
{
  // INDICATOR,PLUGIN,HT,<METHOD>,<INPUT>,<SINE NAME>,<LEAD NAME>
  //     0       1     2     3       4          5           6

  if (set.count() != 7)
  {
    qDebug() << "HT::scriptSINE: invalid settings count" << set.count();
    return 1;
  }

  Curve *in = ind.line(set[4]);
  if (! in)
  {
    in = data.getInput(data.getInputType(set[4]));
    if (! in)
    {
      qDebug() << "HT::scriptSINE: input not found" << set[4];
      return 1;
    }

    ind.setLine(set[4], in);
  }

  Curve *tl = ind.line(set[5]);
  if (tl)
  {
    qDebug() << "HT::scriptSINE: duplicate sine name" << set[5];
    return 1;
  }

  tl = ind.line(set[6]);
  if (tl)
  {
    qDebug() << "HT::scriptSINE: duplicate lead name" << set[6];
    return 1;
  }

  QList<Curve *> pl;
  if (getSINE(in, pl))
    return 1;

  pl.at(0)->setLabel(set[5]);
  pl.at(1)->setLabel(set[6]);

  ind.setLine(set[5], pl.at(0));
  ind.setLine(set[6], pl.at(1));

  return 0;
}

int HT::scriptHT (QStringList &set, Indicator &ind, BarData &data)
{
  // INDICATOR,PLUGIN,HT,<METHOD>,<NAME>,<INPUT>
  //     0       1     2    3       4       5

  if (set.count() != 6)
  {
    qDebug() << "HT::scriptHT: invalid settings count" << set.count();
    return 1;
  }

  int method = _methodList.indexOf(set[3]);
  if (method == -1)
  {
    qDebug() << "HT::scriptHT: invalid method" << set[3];
    return 1;
  }

  Curve *tl = ind.line(set[4]);
  if (tl)
  {
    qDebug() << "HT::scriptHT: duplicate name" << set[4];
    return 1;
  }

  Curve *in = ind.line(set[5]);
  if (! in)
  {
    in = data.getInput(data.getInputType(set[5]));
    if (! in)
    {
      qDebug() << "HT::scriptHT: input not found" << set[5];
      return 1;
    }

    ind.setLine(set[5], in);
  }

  Curve *line = getHT(in, method);
  if (! line)
    return 1;

  line->setLabel(set[4]);

  ind.setLine(set[4], line);

  return 0;
}

Curve * HT::getHT (Curve *in, int method)
{
  if (in->count() < 1)
    return 0;

  int size = in->count();
  TA_Integer outBeg;
  TA_Integer outNb;
  TA_Real input[size];
  TA_Real out[size];

  QList<int> keys;
  in->keys(keys);

  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
    CurveBar *bar = in->bar(keys.at(loop));
    input[loop] = (TA_Real) bar->data();
  }

  TA_RetCode rc = TA_SUCCESS;
  switch ((Method) method)
  {
    case _DCPERIOD:
      rc = TA_HT_DCPERIOD (0, size - 1, &input[0], &outBeg, &outNb, &out[0]);
      break;
    case _DCPHASE:
      rc = TA_HT_DCPHASE (0, size - 1, &input[0], &outBeg, &outNb, &out[0]);
      break;
    case _TRENDLINE:
      rc = TA_HT_TRENDLINE (0, size - 1, &input[0], &outBeg, &outNb, &out[0]);
      break;
    case _TRENDMODE:
    {
      TA_Integer iout[size];
      rc = TA_HT_TRENDMODE (0, size - 1, &input[0], &outBeg, &outNb, &iout[0]);
      if (rc != TA_SUCCESS)
      {
        qDebug() << "HT::getHT TA-Lib error" << rc;
        return 0;
      }

      Curve *line = new Curve;
      int keyLoop = keys.count() - 1;
      int outLoop = outNb - 1;
      while (keyLoop > -1 && outLoop > -1)
      {
        line->setBar(keys.at(keyLoop), new CurveBar((double) iout[outLoop]));
        keyLoop--;
        outLoop--;
      }

      return line;
      break;
    }
    default:
      break;
  }

  if (rc != TA_SUCCESS)
  {
    qDebug() << "HT::getHT TA-Lib error" << rc;
    return 0;
  }

  Curve *line = new Curve;

  int keyLoop = keys.count() - 1;
  int outLoop = outNb - 1;
  while (keyLoop > -1 && outLoop > -1)
  {
    line->setBar(keys.at(keyLoop), new CurveBar(out[outLoop]));
    keyLoop--;
    outLoop--;
  }

  return line;
}

int HT::getPHASE (Curve *in, QList<Curve *> &pl)
{
  if (in->count() < 1)
    return 1;

  int size = in->count();
  TA_Integer outBeg;
  TA_Integer outNb;
  TA_Real input[size];
  TA_Real out[size];
  TA_Real out2[size];

  QList<int> keys;
  in->keys(keys);

  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
    CurveBar *bar = in->bar(keys.at(loop));
    input[loop] = (TA_Real) bar->data();
  }

  TA_RetCode rc = TA_HT_PHASOR (0,
                                size - 1,
                                &input[0],
                                &outBeg,
                                &outNb,
                                &out[0],
                                &out2[0]);

  if (rc != TA_SUCCESS)
  {
    qDebug() << "HT::getPHASE: TA-Lib error" << rc;
    return 1;
  }

  Curve *pline = new Curve;
  Curve *qline = new Curve;

  int keyLoop = keys.count() - 1;
  int outLoop = outNb - 1;
  while (keyLoop > -1 && outLoop > -1)
  {
    pline->setBar(keys.at(keyLoop), new CurveBar(out[outLoop]));
    qline->setBar(keys.at(keyLoop), new CurveBar(out2[outLoop]));

    keyLoop--;
    outLoop--;
  }

  pl.append(pline);
  pl.append(qline);

  return 0;
}

int HT::getSINE (Curve *in, QList<Curve *> &pl)
{
  if (in->count() < 1)
    return 1;

  int size = in->count();
  TA_Integer outBeg;
  TA_Integer outNb;
  TA_Real input[size];
  TA_Real out[size];
  TA_Real out2[size];

  QList<int> keys;
  in->keys(keys);

  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
    CurveBar *bar = in->bar(keys.at(loop));
    input[loop] = (TA_Real) bar->data();
  }

  TA_RetCode rc = TA_HT_SINE (0,
                              size - 1,
                              &input[0],
                              &outBeg,
                              &outNb,
                              &out[0],
                              &out2[0]);

  if (rc != TA_SUCCESS)
  {
    qDebug() << "HT::getSINE: TA-Lib error" << rc;
    return 1;
  }

  Curve *sline = new Curve;
  Curve *lline = new Curve;

  int keyLoop = keys.count() - 1;
  int outLoop = outNb - 1;
  while (keyLoop > -1 && outLoop > -1)
  {
    sline->setBar(keys.at(keyLoop), new CurveBar(out[outLoop]));
    lline->setBar(keys.at(keyLoop), new CurveBar(out2[outLoop]));

    keyLoop--;
    outLoop--;
  }

  pl.append(sline);
  pl.append(lline);

  return 0;
}

QStringList & HT::list ()
{
  return _methodList;
}

//*************************************************************
//*************************************************************
//*************************************************************

IndicatorPlugin * createIndicatorPlugin ()
{
  HT *o = new HT;
  return ((IndicatorPlugin *) o);
}


