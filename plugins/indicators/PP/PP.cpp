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

#include "PP.h"
#include "FunctionBARS.h"
#include "Curve.h"

#include <QtDebug>

PP::PP ()
{
  _indicator = "PP";
}

int PP::getIndicator (Indicator &ind, BarData &data)
{
  Setting settings = ind.settings();

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

  QString s;
  // 1R line
  if (settings.getInt(_R1Show))
  {
    Curve *line = calculate(0, data);
    if (line)
    {
      settings.getData(_R1Color, s);
      QColor c(s);
      line->setColor(c);

      settings.getData(_R1Label, s);
      line->setLabel(s);
      
      line->setZ(1);
      ind.setLine(s, line);
    }
  }

  // 2R line
  if (settings.getInt(_R2Show))
  {
    Curve *line = calculate(1, data);
    if (line)
    {
      settings.getData(_R2Color, s);
      QColor c(s);
      line->setColor(c);

      settings.getData(_R2Label, s);
      line->setLabel(s);
      
      line->setZ(2);
      ind.setLine(s, line);
    }
  }

  // 3R line
  if (settings.getInt(_R3Show))
  {
    Curve *line = calculate(2, data);
    if (line)
    {
      settings.getData(_R3Color, s);
      QColor c(s);
      line->setColor(c);

      settings.getData(_R3Label, s);
      line->setLabel(s);
      
      line->setZ(3);
      ind.setLine(s, line);
    }
  }

  // 1S line
  if (settings.getInt(_S1Show))
  {
    Curve *line = calculate(3, data);
    if (line)
    {
      settings.getData(_S1Color, s);
      QColor c(s);
      line->setColor(c);

      settings.getData(_S1Label, s);
      line->setLabel(s);
      
      line->setZ(4);
      ind.setLine(s, line);
    }
  }

  // 2S line
  if (settings.getInt(_S2Show))
  {
    Curve *line = calculate(4, data);
    if (line)
    {
      settings.getData(_S2Color, s);
      QColor c(s);
      line->setColor(c);

      settings.getData(_S2Label, s);
      line->setLabel(s);
      
      line->setZ(5);
      ind.setLine(s, line);
    }
  }

  // 3S line
  if (settings.getInt(_S3Show))
  {
    Curve *line = calculate(5, data);
    if (line)
    {
      settings.getData(_S3Color, s);
      QColor c(s);
      line->setColor(c);

      settings.getData(_S3Label, s);
      line->setLabel(s);
      
      line->setZ(6);
      ind.setLine(s, line);
    }
  }

  return 0;
}

int PP::getCUS (QStringList &set, Indicator &ind, BarData &data)
{
  // INDICATOR,PLUGIN,PP,<NAME>,<POINT>
  //     0       1     2    3      4

  if (set.count() != 5)
  {
    qDebug() << "PP::getCUS: invalid settings count" << set.count();
    return 1;
  }

  Curve *tl = ind.line(set[3]);
  if (tl)
  {
    qDebug() << "PP::getCUS: duplicate name" << set[3];
    return 1;
  }

  bool ok;
  int point = set[4].toInt(&ok);
  if (! ok)
  {
    qDebug() << "PP::getCUS: invalid point" << set[4];
    return 1;
  }
  else
  {
    if (point < 0 || point > 5)
    {
      qDebug() << "PP::getCUS: invalid point" << set[4];
      return 1;
    }
  }

  Curve *line = calculate(point, data);
  if (! line)
    return 1;

  line->setLabel(set[3]);

  ind.setLine(set[3], line);

  return 0;
}

IndicatorPluginDialog * PP::dialog (Indicator &i)
{
  IndicatorPluginDialog *dialog = new IndicatorPluginDialog(i);

  Setting _settings = i.settings();

  // r1 tab
  int tab = dialog->addTab(tr("R1"));

  QString d;
  _settings.getData(_R1Color, d);
  dialog->addColor(tab, _R1Color, tr("Color"), d);

  _settings.getData(_R1Label, d);
  dialog->addText(tab, _R1Label, tr("Label"), d);

  dialog->addCheck(tab, _R1Show, tr("Show"), _settings.getInt(_R1Show));

  // r2 tab
  tab = dialog->addTab(tr("R2"));

  _settings.getData(_R2Color, d);
  dialog->addColor(tab, _R2Color, tr("Color"), d);

  _settings.getData(_R2Label, d);
  dialog->addText(tab, _R2Label, tr("Label"), d);

  dialog->addCheck(tab, _R2Show, tr("Show"), _settings.getInt(_R2Show));

  // r3 tab
  tab = dialog->addTab(tr("R3"));

  _settings.getData(_R3Color, d);
  dialog->addColor(tab, _R3Color, tr("Color"), d);

  _settings.getData(_R3Label, d);
  dialog->addText(tab, _R3Label, tr("Label"), d);

  dialog->addCheck(tab, _R3Show, tr("Show"), _settings.getInt(_R3Show));

  // s1 tab
  tab = dialog->addTab(tr("S1"));

  _settings.getData(_S1Color, d);
  dialog->addColor(tab, _S1Color, tr("Color"), d);

  _settings.getData(_S1Label, d);
  dialog->addText(tab, _S1Label, tr("Label"), d);

  dialog->addCheck(tab, _S1Show, tr("Show"), _settings.getInt(_S1Show));

  // s2 tab
  tab = dialog->addTab(tr("S2"));

  _settings.getData(_S2Color, d);
  dialog->addColor(tab, _S2Color, tr("Color"), d);

  _settings.getData(_S2Label, d);
  dialog->addText(tab, _S2Label, tr("Label"), d);

  dialog->addCheck(tab, _S2Show, tr("Show"), _settings.getInt(_S2Show));

  // s3 tab
  tab = dialog->addTab(tr("S3"));

  _settings.getData(_S3Color, d);
  dialog->addColor(tab, _S3Color, tr("Color"), d);

  _settings.getData(_S3Label, d);
  dialog->addText(tab, _S3Label, tr("Label"), d);

  dialog->addCheck(tab, _S3Show, tr("Show"), _settings.getInt(_S3Show));

  return dialog;
}

void PP::defaults (Indicator &i)
{
  Setting set;
  set.setData(_R1Color, "red");
  set.setData(_R2Color, "red");
  set.setData(_R3Color, "red");
  set.setData(_S1Color, "red");
  set.setData(_S2Color, "red");
  set.setData(_S3Color, "red");
  set.setData(_R1Label, "1R");
  set.setData(_R2Label, "2R");
  set.setData(_R3Label, "3R");
  set.setData(_S1Label, "1S");
  set.setData(_S2Label, "2S");
  set.setData(_S3Label, "3S");
  set.setData(_R1Show, 1);
  set.setData(_R2Show, 1);
  set.setData(_R3Show, 1);
  set.setData(_S1Show, 1);
  set.setData(_S2Show, 1);
  set.setData(_S3Show, 1);
  i.setSettings(set);
}

void PP::plotNames (Indicator &i, QStringList &l)
{
  l.clear();

  Setting settings = i.settings();
  QString s;
  
  settings.getData(_R1Label, s);
  l.append(s);

  settings.getData(_R2Label, s);
  l.append(s);

  settings.getData(_R3Label, s);
  l.append(s);

  settings.getData(_S1Label, s);
  l.append(s);
  
  settings.getData(_S2Label, s);
  l.append(s);

  settings.getData(_S3Label, s);
  l.append(s);
}

Curve * PP::calculate (int point, BarData &data)
{
  if (data.count() < 1)
    return 0;

  Curve *output = new Curve(Curve::Horizontal);

  Bar bar = data.getBar(data.count() - 1);
  double high = bar.getHigh();
  double low = bar.getLow();
  double close = bar.getClose();
  double pp = 0;
  double t = 0;

  switch (point)
  {
    case 0: // first resistance
    {
      pp = (high + low + close) / 3;
      t = (2 * pp) - low;
      output->setBar(0, new CurveBar(t));
      break;
    }
    case 1: // second resistance
    {
      pp = (high + low + close) / 3;
      t = pp + (high - low);
      output->setBar(0, new CurveBar(t));
      break;
    }
    case 2: // third resistance
    {
      pp = (high + low + close) / 3;
      t = (2 * pp) + (high - (2 * low));
      output->setBar(0, new CurveBar(t));
      break;
    }
    case 3: // first support
    {
      pp = (high + low + close) / 3;
      t = (2 * pp) - high;
      output->setBar(0, new CurveBar(t));
      break;
    }
    case 4: // second support
    {
      pp = (high + low + close) / 3;
      t = pp - (high - low);
      output->setBar(0, new CurveBar(t));
      break;
    }
    case 5: // third support
    {
      pp = (high + low + close) / 3;
      t = (2 * pp) - ((2 * high) - low);
      output->setBar(0, new CurveBar(t));
      break;
    }
    default:
      break;
  }

  return output;
}

//*************************************************************
//*************************************************************
//*************************************************************

IndicatorPlugin * createIndicatorPlugin ()
{
  PP *o = new PP;
  return ((IndicatorPlugin *) o);
}


