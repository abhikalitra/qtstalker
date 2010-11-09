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

#include "PO.h"
#include "FunctionMA.h"
#include "Curve.h"
#include "ta_libc.h"

#include <QtDebug>

PO::PO ()
{
  _indicator = "PO";
  _methodList << "APO" << "PPO";
}

int PO::getIndicator (Indicator &ind, BarData &data)
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

  int fast = settings.getInt(_FastPeriod);
  int slow = settings.getInt(_SlowPeriod);

  FunctionMA mau;
  settings.getData(_MAType, s);
  int ma = mau.typeFromString(s);

  settings.getData(_Method, s);
  int method = _methodList.indexOf(s);

  Curve *line = calculate(in, fast, slow, ma, method);
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
  
  line->setZ(0);
  ind.setLine(s, line);

  delete in;

  return 0;
}

int PO::getCUS (QStringList &set, Indicator &ind, BarData &data)
{
  // INDICATOR,PLUGIN,PO,<METHOD>,<NAME>,<INPUT>,<FAST_PERIOD>,<SLOW_PERIOD>,<MA_TYPE>
  //     0       1     2    3       4       5         6             7           8

  if (set.count() != 9)
  {
    qDebug() << "PO::getCUS: invalid parm count" << set.count();
    return 1;
  }

  int method = _methodList.indexOf(set[3]);
  if (method == -1)
  {
    qDebug() << "PO::getCUS: invalid method" << set[3];
    return 1;
  }

  Curve *tl = ind.line(set[4]);
  if (tl)
  {
    qDebug() << "PO::getCUS: duplicate name" << set[4];
    return 1;
  }

  Curve *in = ind.line(set[5]);
  if (! in)
  {
    in = data.getInput(data.getInputType(set[5]));
    if (! in)
    {
      qDebug() << "PO::getCUS: input not found" << set[5];
      return 1;
    }

    ind.setLine(set[5], in);
  }

  bool ok;
  int fast = set[6].toInt(&ok);
  if (! ok)
  {
    qDebug() << "PO::getCUS: invalid fast period" << set[6];
    return 1;
  }

  int slow = set[7].toInt(&ok);
  if (! ok)
  {
    qDebug() << "PO::getCUS: invalid slow period" << set[7];
    return 1;
  }

  FunctionMA mau;
  int ma = mau.typeFromString(set[8]);
  if (ma == -1)
  {
    qDebug() << "PO::getCUS: invalid ma" << set[8];
    return 1;
  }

  Curve *line = calculate(in, fast, slow, ma, method);
  if (! line)
    return 1;

  line->setLabel(set[4]);

  ind.setLine(set[4], line);

  return 0;
}

IndicatorPluginDialog * PO::dialog (Indicator &i)
{
  IndicatorPluginDialog *dialog = new IndicatorPluginDialog(i);

  dialog->setHelpFile("PO.html");

  Setting _settings = i.settings();

  // general tab
  int tab = dialog->addTab(tr("General"));

  QString d;
  _settings.getData(_Method, d);
  dialog->addCombo(tab, _Method, tr("Method"), list(), d);

  _settings.getData(_Input, d);
  dialog->addInput(tab, _Input, tr("Input"), d);

  dialog->addInt(tab, _FastPeriod, tr("Fast Period"), _settings.getInt(_FastPeriod), 100000, 2);

  dialog->addInt(tab, _SlowPeriod, tr("Slow Period"), _settings.getInt(_SlowPeriod), 100000, 2);

  _settings.getData(_MAType, d);
  dialog->addMA(tab, _MAType, tr("MA Type"), d);

  _settings.getData(_Color, d);
  dialog->addColor(tab, _Color, tr("Color"), d);

  _settings.getData(_Plot, d);
  dialog->addPlot(tab, _Plot, tr("Plot"), d);

  _settings.getData(_Label, d);
  dialog->addText(tab, _Label, tr("Label"), d);

  return dialog;
}

void PO::defaults (Indicator &i)
{
  Setting set;
  set.setData(_Color, "red");
  set.setData(_Plot, "Histogram Bar");
  set.setData(_Label, _indicator);
  set.setData(_Input, "Close");
  set.setData(_FastPeriod, 12);
  set.setData(_SlowPeriod, 26);
  set.setData(_MAType, "SMA");
  set.setData(_Method, "APO");
  i.setSettings(set);
}

void PO::plotNames (Indicator &i, QStringList &l)
{
  l.clear();

  Setting settings = i.settings();
  QString s;
  settings.getData(_Label, s);
  l.append(s);
}

Curve * PO::calculate (Curve *in, int fast, int slow, int ma, int method)
{
  if (in->count() < fast || in->count() < slow)
    return 0;

  QList<int> keys;
  in->keys(keys);
  int size = keys.count();

  TA_Real input[size];
  TA_Real out[size];
  TA_Integer outBeg;
  TA_Integer outNb;

  int loop = 0;
  for (; loop < size; loop++)
  {
    CurveBar *bar = in->bar(keys.at(loop));
    input[loop] = (TA_Real) bar->data();
  }

  TA_RetCode rc = TA_SUCCESS;

  switch ((Method) method)
  {
    case _APO:
      rc = TA_APO(0, size - 1, &input[0], fast, slow, (TA_MAType) ma, &outBeg, &outNb, &out[0]);
      break;
    case _PPO:
      rc = TA_PPO(0, size - 1, &input[0], fast, slow, (TA_MAType) ma, &outBeg, &outNb, &out[0]);
      break;
    default:
      break;
  }

  if (rc != TA_SUCCESS)
  {
    qDebug() << "PO::calculate: TA-Lib error" << rc;
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

QStringList & PO::list ()
{
  return _methodList;
}

//*************************************************************
//*************************************************************
//*************************************************************

IndicatorPlugin * createIndicatorPlugin ()
{
  PO *o = new PO;
  return ((IndicatorPlugin *) o);
}


