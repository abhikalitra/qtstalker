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

#include "CMO.h"
#include "Curve.h"
#include "ta_libc.h"

#include <QtDebug>

CMO::CMO ()
{
  _indicator = "CMO";
}

int CMO::getIndicator (Indicator &ind, BarData &data)
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

  Curve *line = calculate(in, period);
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

int CMO::getCUS (QStringList &set, Indicator &ind, BarData &data)
{
  // INDICATOR,PLUGIN,CMO,<NAME>,<INPUT>,<PERIOD>
  //     0       1     2    3       4       5

  if (set.count() != 6)
  {
    qDebug() << "CMO::getCUS: invalid parm count" << set.count();
    return 1;
  }

  Curve *tl = ind.line(set[3]);
  if (tl)
  {
    qDebug() << "CMO::getCUS: duplicate name" << set[3];
    return 1;
  }

  Curve *in = ind.line(set[4]);
  if (! in)
  {
    in = data.getInput(data.getInputType(set[4]));
    if (! in)
    {
      qDebug() << "CMO::getCUS: input not found" << set[4];
      return 1;
    }

    ind.setLine(set[4], in);
  }

  bool ok;
  int period = set[5].toInt(&ok);
  if (! ok)
  {
    qDebug() << "CMO::getCUS: invalid period parm" << set[5];
    return 1;
  }

  Curve *line = calculate(in, period);
  if (! line)
    return 1;

  line->setLabel(set[3]);

  ind.setLine(set[3], line);

  return 0;
}

IndicatorPluginDialog * CMO::dialog (Indicator &i)
{
  IndicatorPluginDialog *dialog = new IndicatorPluginDialog(i);

  dialog->setHelpFile("CMO.html");

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

  dialog->addInt(tab, _Period, tr("Period"), _settings.getInt(_Period), 100000, 2);

  _settings.getData(_Input, d);
  dialog->addInput(tab, _Input, tr("Input"), d);

  return dialog;
}

void CMO::defaults (Indicator &i)
{
  Setting set;
  set.setData(_Color, "red");
  set.setData(_Plot, "Histogram Bar");
  set.setData(_Label, _indicator);
  set.setData(_Input, "Close");
  set.setData(_Period, 14);
  i.setSettings(set);
}

void CMO::plotNames (Indicator &i, QStringList &l)
{
  l.clear();

  Setting settings = i.settings();
  QString s;
  settings.getData(_Label, s);
  l.append(s);
}

Curve * CMO::calculate (Curve *in, int period)
{
  if (in->count() < period)
    return 0;

  TA_Real input[in->count()];
  TA_Real out[in->count()];
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

  TA_RetCode rc = TA_CMO(0,
                         keys.count() - 1,
                         &input[0],
                         period,
                         &outBeg,
                         &outNb,
                         &out[0]);
  if (rc != TA_SUCCESS)
  {
    qDebug() << "CMO::calculate: TA-Lib error" << rc;
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

//*************************************************************
//*************************************************************
//*************************************************************

IndicatorPlugin * createIndicatorPlugin ()
{
  CMO *o = new CMO;
  return ((IndicatorPlugin *) o);
}
