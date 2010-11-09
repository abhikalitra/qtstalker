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

#include "LINEARREG.h"
#include "FunctionBARS.h"
#include "Curve.h"
#include "ta_libc.h"

#include <QtDebug>

LINEARREG::LINEARREG ()
{
  _indicator = "LINEARREG";
  _methodList << "LINEARREG" << "ANGLE" << "INTERCEPT" "SLOPE" << "TSF";
}

int LINEARREG::getIndicator (Indicator &ind, BarData &data)
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

  int period = settings.getInt(_Period);

  settings.getData(_Method, s);
  int method = _methodList.indexOf(s);

  Curve *line = calculate(in, period, method);
  if (! line)
  {
    delete in;
    return 1;
  }

  switch ((Method) method)
  {
    case _LINEARREG:
    case _INTERCEPT:
    {
      QColor up("green");
      QColor down("red");
      QColor neutral("blue");
      FunctionBARS b;
      Curve *bars = b.getBARS(up, down, neutral, data);
      if (bars)
      {
        bars->setZ(0);
	ind.setLine(0, bars);
      }
      break;
    }
    default:
      break;
  }

  settings.getData(_Plot, s);
  line->setType((Curve::Type) line->typeFromString(s));

  settings.getData(_Color, s);
  QColor c(s);
  line->setColor(c);

  settings.getData(_Label, s);
  line->setLabel(s);
  
  line->setZ(1);
  ind.setLine(s, line);

  delete in;

  return 0;
}

int LINEARREG::getCUS (QStringList &set, Indicator &ind, BarData &data)
{
  // INDICATOR,PLUGIN,LINEARREG,<METHOD>,<NAME>,<INPUT>,<PERIOD>
  //     0       1        2        3       4       5        6

  if (set.count() != 7)
  {
    qDebug() << "LINEARREG::getCUS: invalid parm count" << set.count();
    return 1;
  }

  int method = _methodList.indexOf(set[3]);
  if (method == -1)
  {
    qDebug() << "LINEARREG::getCUS: invalid method" << set[3];
    return 1;
  }

  Curve *tl = ind.line(set[4]);
  if (tl)
  {
    qDebug() << "LINEARREG::getCUS: duplicate name" << set[4];
    return 1;
  }

  Curve *in = ind.line(set[5]);
  if (! in)
  {
    in = data.getInput(data.getInputType(set[5]));
    if (! in)
    {
      qDebug() << "LINEARREG::getCUS: input not found" << set[5];
      return 1;
    }

    ind.setLine(set[5], in);
  }

  bool ok;
  int period = set[6].toInt(&ok);
  if (! ok)
  {
    qDebug() << "LINEARREG::getCUS: invalid period" << set[6];
    return 1;
  }

  Curve *line = calculate(in, period, method);
  if (! line)
    return 1;

  line->setLabel(set[4]);

  ind.setLine(set[4], line);

  return 0;
}

IndicatorPluginDialog * LINEARREG::dialog (Indicator &i)
{
  IndicatorPluginDialog *dialog = new IndicatorPluginDialog(i);

  dialog->setHelpFile("LINEARREG.html");

  Setting _settings = i.settings();

  // general tab
  int tab = dialog->addTab(tr("General"));

  QString d;
  _settings.getData(_Color, d);
  dialog->addColor(tab, _Color, tr("Color"), d);

  _settings.getData(_Plot, d);
  dialog->addPlot(tab, _Plot, tr("Plot"), d);

  _settings.getData(_Label, d);
  dialog->addText(tab, _Label, tr("Label"), d);

  _settings.getData(_Method, d);
  dialog->addCombo(tab, _Method, tr("Method"), list(), d);

  _settings.getData(_Input, d);
  dialog->addInput(tab, _Input, tr("Input"), d);

  dialog->addInt(tab, _Period, tr("Period"), _settings.getInt(_Period), 100000, 2);

  return dialog;
}

void LINEARREG::defaults (Indicator &i)
{
  Setting set;
  set.setData(_Method, "LINEARREG");
  set.setData(_Color, "red");
  set.setData(_Plot, "Line");
  set.setData(_Label, _indicator);
  set.setData(_Input, "Close");
  set.setData(_Period, 14);
  i.setSettings(set);
}

void LINEARREG::plotNames (Indicator &i, QStringList &l)
{
  l.clear();

  Setting settings = i.settings();
  QString s;
  settings.getData(_Label, s);
  l.append(s);
}

Curve * LINEARREG::calculate (Curve *in, int period, int method)
{
  if (in->count() < period)
    return 0;

  int size = in->count();
  TA_Real input[size];
  TA_Real out[size];
  TA_Integer outBeg;
  TA_Integer outNb;

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
    case _LINEARREG:
      rc = TA_LINEARREG(0, size - 1, &input[0], period, &outBeg, &outNb, &out[0]);
      break;
    case _ANGLE:
      rc = TA_LINEARREG_ANGLE(0, size - 1, &input[0], period, &outBeg, &outNb, &out[0]);
      break;
    case _INTERCEPT:
      rc = TA_LINEARREG_INTERCEPT(0, size - 1, &input[0], period, &outBeg, &outNb, &out[0]);
      break;
    case _SLOPE:
      rc = TA_LINEARREG_SLOPE(0, size - 1, &input[0], period, &outBeg, &outNb, &out[0]);
      break;
    case _TSF:
      rc = TA_TSF(0, size - 1, &input[0], period, &outBeg, &outNb, &out[0]);
      break;
    default:
      break;
  }

  if (rc != TA_SUCCESS)
  {
    qDebug() << "LINEARREG::calculate: TA-Lib error" << rc;
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

QStringList & LINEARREG::list ()
{
  return _methodList;
}

//*************************************************************
//*************************************************************
//*************************************************************

IndicatorPlugin * createIndicatorPlugin ()
{
  LINEARREG *o = new LINEARREG;
  return ((IndicatorPlugin *) o);
}


