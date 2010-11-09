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

#include "MAMA.h"
#include "FunctionBARS.h"
#include "Curve.h"
#include "ta_libc.h"

#include <QtDebug>

MAMA::MAMA ()
{
  _indicator = "MAMA";
}

int MAMA::getIndicator (Indicator &ind, BarData &data)
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

  double fast = settings.getDouble(_FastLimit);
  double slow = settings.getDouble(_SlowLimit);

  QList<Curve *> l;
  if (calculate(in, fast, slow, l))
  {
    delete in;
    return 1;
  }

  Curve *mama = l.at(0);
  Curve *fama = l.at(1);

  int osc = settings.getInt(_OSC);
  if (osc)
  {
    Curve *line = new Curve;
    line->setType(Curve::HistogramBar);

    QList<int> keys;
    mama->keys(keys);

    int loop = 0;
    for (; loop < keys.count(); loop++)
    {
      CurveBar *mbar = mama->bar(keys.at(loop));
      if (! mbar)
        continue;

      CurveBar *fbar = fama->bar(keys.at(loop));
      if (! fbar)
        continue;

      line->setBar(keys.at(loop), new CurveBar(mbar->data() - fbar->data()));
    }

    settings.getData(_OSCColor, s);
    QColor c(s);
    line->setColor(c);

    settings.getData(_OSCLabel, s);
    line->setLabel(s);

    line->setZ(0);
    ind.setLine(s, line);
  }
  else
  {
    QColor up("green");
    QColor down("red");
    QColor neutral("blue");
    FunctionBARS b;
    Curve *bars = b.getBARS(up, down, neutral, data);
    if (bars)
    {
      bars->setZ(1);
      ind.setLine(1, bars);
    }

    settings.getData(_MAMAPlot, s);
    mama->setType((Curve::Type) mama->typeFromString(s));

    settings.getData(_MAMAColor, s);
    QColor c(s);
    mama->setColor(c);

    settings.getData(_MAMALabel, s);
    mama->setLabel(s);
    
    mama->setZ(2);
    ind.setLine(s, mama);

    // fama line
    settings.getData(_FAMAPlot, s);
    fama->setType((Curve::Type) fama->typeFromString(s));

    settings.getData(_FAMAColor, s);
    c.setNamedColor(s);
    fama->setColor(c);

    settings.getData(_FAMALabel, s);
    fama->setLabel(s);
    
    fama->setZ(3);
    ind.setLine(s, fama);
  }

  delete in;

  return 0;
}

int MAMA::getCUS (QStringList &set, Indicator &ind, BarData &data)
{
  // INDICATOR,PLUGIN,MAMA,<INPUT>,<NAME_MAMA>,<NAME_FAMA>,<FAST_LIMIT>,<SLOW_LIMIT>
  //      0       1     2     3         4           5           6            7

  if (set.count() != 8)
  {
    qDebug() << "MAMA::getCUS: invalid settings count" << set.count();
    return 1;
  }

  Curve *in = ind.line(set[3]);
  if (! in)
  {
    in = data.getInput(data.getInputType(set[3]));
    if (! in)
    {
      qDebug() << "MAMA::getCUS: input not found" << set[3];
      return 1;
    }

    ind.setLine(set[3], in);
  }

  Curve *tl = ind.line(set[4]);
  if (tl)
  {
    qDebug() << "MAMA::getCUS: mama duplicate name" << set[4];
    return 1;
  }

  tl = ind.line(set[5]);
  if (tl)
  {
    qDebug() << "MAMA::getCUS: fama duplicate name" << set[5];
    return 1;
  }

  bool ok;
  double fast = set[6].toDouble(&ok);
  if (! ok)
  {
    qDebug() << "MAMA::getCUS: invalid fast limit" << set[6];
    return 1;
  }

  double slow = set[7].toDouble(&ok);
  if (! ok)
  {
    qDebug() << "MAMA::getCUS: invalid slow limit" << set[7];
    return 1;
  }

  QList<Curve *> l;
  if (calculate(in, fast, slow, l))
    return 1;

  l.at(0)->setLabel(set[4]);
  l.at(1)->setLabel(set[5]);

  ind.setLine(set[4], l.at(0));
  ind.setLine(set[5], l.at(1));

  return 0;
}

IndicatorPluginDialog * MAMA::dialog (Indicator &i)
{
  IndicatorPluginDialog *dialog = new IndicatorPluginDialog(i);

  dialog->setHelpFile("MAMA.html");

  Setting _settings = i.settings();

  // general tab
  int tab = dialog->addTab(tr("General"));

  QString d;
  _settings.getData(_Input, d);
  dialog->addInput(tab, _Input, tr("Input"), d);

  dialog->addDouble(tab, _FastLimit, tr("Fast Limit"), _settings.getDouble(_FastLimit), 0.99, 0.01);

  dialog->addDouble(tab, _SlowLimit, tr("Slow Limit"), _settings.getDouble(_SlowLimit), 0.99, 0.01);

  dialog->addCheck(tab, _OSC, tr("Oscillator"), _settings.getInt(_OSC));
  
  // mama tab
  tab = dialog->addTab(tr("MAMA"));

  _settings.getData(_MAMAColor, d);
  dialog->addColor(tab, _MAMAColor, tr("Color"), d);

  _settings.getData(_MAMAPlot, d);
  dialog->addPlot(tab, _MAMAPlot, tr("Plot"), d);

  _settings.getData(_MAMALabel, d);
  dialog->addText(tab, _MAMALabel, tr("Label"), d);

  // fama tab
  tab = dialog->addTab(tr("FAMA"));

  _settings.getData(_FAMAColor, d);
  dialog->addColor(tab, _FAMAColor, tr("Color"), d);

  _settings.getData(_FAMAPlot, d);
  dialog->addPlot(tab, _FAMAPlot, tr("Plot"), d);

  _settings.getData(_FAMALabel, d);
  dialog->addText(tab, _FAMALabel, tr("Label"), d);

  // osc tab
  tab = dialog->addTab(tr("OSC"));

  _settings.getData(_OSCColor, d);
  dialog->addColor(tab, _OSCColor, tr("Color"), d);

  _settings.getData(_OSCPlot, d);
  dialog->addPlot(tab, _OSCPlot, tr("Plot"), d);

  _settings.getData(_OSCLabel, d);
  dialog->addText(tab, _OSCLabel, tr("Label"), d);

  return dialog;
}

void MAMA::defaults (Indicator &i)
{
  Setting set;
  set.setData(_OSC, 1);
  set.setData(_OSCColor, "red");
  set.setData(_MAMAColor, "red");
  set.setData(_FAMAColor, "yellow");
  set.setData(_MAMAPlot, "Line");
  set.setData(_FAMAPlot, "Line");
  set.setData(_OSCPlot, "Histogram Bar");
  set.setData(_MAMALabel, "MAMA");
  set.setData(_FAMALabel, "FAMA");
  set.setData(_OSCLabel, "MAMAOSC");
  set.setData(_FastLimit, 0.5);
  set.setData(_SlowLimit, 0.05);
  set.setData(_Input, "Close");
  i.setSettings(set);
}

void MAMA::plotNames (Indicator &i, QStringList &l)
{
  l.clear();

  Setting settings = i.settings();
  QString s;
  
  int osc = settings.getInt(_OSC);
  if (osc)
  {
    settings.getData(_OSCLabel, s);
    l.append(s);
  }
  else
  {
    settings.getData(_MAMALabel, s);
    l.append(s);

    settings.getData(_FAMALabel, s);
    l.append(s);
  }
}

int MAMA::calculate (Curve *in, double fast, double slow, QList<Curve *> &l)
{
  if (in->count() < fast || in->count() < slow)
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

  TA_RetCode rc = TA_MAMA(0,
                          size - 1,
                          &input[0],
                          fast,
                          slow,
                          &outBeg,
                          &outNb,
                          &out[0],
                          &out2[0]);

  if (rc != TA_SUCCESS)
  {
    qDebug() << "MAMA::calculate: TA-Lib error" << rc;
    return 1;
  }

  Curve *mama = new Curve;
  Curve *fama = new Curve;

  int keyLoop = keys.count() - 1;
  int outLoop = outNb - 1;
  while (keyLoop > -1 && outLoop > -1)
  {
    mama->setBar(keys.at(keyLoop), new CurveBar(out[outLoop]));
    fama->setBar(keys.at(keyLoop), new CurveBar(out2[outLoop]));

    keyLoop--;
    outLoop--;
  }

  l.append(mama);
  l.append(fama);

  return 0;
}

//*************************************************************
//*************************************************************
//*************************************************************

IndicatorPlugin * createIndicatorPlugin ()
{
  MAMA *o = new MAMA;
  return ((IndicatorPlugin *) o);
}


