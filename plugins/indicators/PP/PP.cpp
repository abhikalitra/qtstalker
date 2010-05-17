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
#include "BARSUtils.h"
#include "PlotFactory.h"

#include <QtDebug>

PP::PP ()
{
  indicator = "PP";

  settings.setData(R1Color, "red");
  settings.setData(R2Color, "red");
  settings.setData(R3Color, "red");
  settings.setData(S1Color, "red");
  settings.setData(S2Color, "red");
  settings.setData(S3Color, "red");
  settings.setData(R1Label, "1R");
  settings.setData(R2Label, "2R");
  settings.setData(R3Label, "3R");
  settings.setData(S1Label, "1S");
  settings.setData(S2Label, "2S");
  settings.setData(S3Label, "3S");
  settings.setData(R1Show, 1);
  settings.setData(R2Show, 1);
  settings.setData(R3Show, 1);
  settings.setData(S1Show, 1);
  settings.setData(S2Show, 1);
  settings.setData(S3Show, 1);
}

int PP::getIndicator (Indicator &ind, BarData *data)
{
  QColor up("green");
  QColor down("red");
  QColor neutral("blue");
  BARSUtils b;
  PlotLine *bars = b.getBARS(data, up, down, neutral);
  if (bars)
    ind.addLine(bars);
  
  QString s;
  // 1R line
  if (settings.getInt(R1Show))
  {
    settings.getData(R1Color, s);
    QColor color(s);

    PlotLine *line = getPP(data, 0, color);
    if (line)
    {
      settings.getData(R1Label, s);
      line->setLabel(s);
      ind.addLine(line);
    }
  }

  // 2R line
  if (settings.getInt(R2Show))
  {
    settings.getData(R2Color, s);
    QColor color(s);

    PlotLine *line = getPP(data, 1, color);
    if (line)
    {
      settings.getData(R2Label, s);
      line->setLabel(s);
      ind.addLine(line);
    }
  }

  // 3R line
  if (settings.getInt(R3Show))
  {
    settings.getData(R3Color, s);
    QColor color(s);

    PlotLine *line = getPP(data, 2, color);
    if (line)
    {
      settings.getData(R3Label, s);
      line->setLabel(s);
      ind.addLine(line);
    }
  }

  // 1S line
  if (settings.getInt(S1Show))
  {
    settings.getData(S1Color, s);
    QColor color(s);

    PlotLine *line = getPP(data, 3, color);
    if (line)
    {
      settings.getData(S1Label, s);
      line->setLabel(s);
      ind.addLine(line);
    }
  }

  // 2S line
  if (settings.getInt(S2Show))
  {
    settings.getData(S2Color, s);
    QColor color(s);

    PlotLine *line = getPP(data, 4, color);
    if (line)
    {
      settings.getData(S2Label, s);
      line->setLabel(s);
      ind.addLine(line);
    }
  }

  // 3S line
  if (settings.getInt(S3Show))
  {
    settings.getData(S3Color, s);
    QColor color(s);

    PlotLine *line = getPP(data, 5, color);
    if (line)
    {
      settings.getData(S3Label, s);
      line->setLabel(s);
      ind.addLine(line);
    }
  }

  return 0;
}

int PP::getCUS (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  // INDICATOR,PLUGIN,PP,<NAME>,<POINT>,<COLOR>
  //     0       1     2    3      4       5

  if (set.count() != 6)
  {
    qDebug() << indicator << "::getCUS: invalid settings count" << set.count();
    return 1;
  }

  PlotLine *tl = tlines.value(set[3]);
  if (tl)
  {
    qDebug() << indicator << "::getCUS: duplicate name" << set[3];
    return 1;
  }

  bool ok;
  int point = set[4].toInt(&ok);
  if (! ok)
  {
    qDebug() << indicator << "::getCUS: invalid point" << set[4];
    return 1;
  }
  else
  {
    if (point < 0 || point > 5)
    {
      qDebug() << indicator << "::getCUS: invalid point" << set[4];
      return 1;
    }
  }

  QColor color(set[5]);
  if (! color.isValid())
  {
    qDebug() << indicator << "::getCUS: invalid color" << set[5];
    return 1;
  }

  PlotLine *line = getPP(data, point, color);
  if (! line)
    return 1;

  line->setLabel(set[3]);

  tlines.insert(set[3], line);

  return 0;
}

PlotLine * PP::getPP (BarData *data, int point, QColor &color)
{
  QString s = "Horizontal";
  PlotFactory fac;
  PlotLine *output = fac.plot(s);
  if (! output)
    return 0;

  Bar *bar = data->getBar(data->count() - 1);
  double high = bar->getHigh();
  double low = bar->getLow();
  double close = bar->getClose();
  double pp = 0;
  double t = 0;

  switch (point)
  {
    case 0: // first resistance
      pp = (high + low + close) / 3;
      t = (2 * pp) - low;
      output->setData(0, new PlotLineBar(color, t));
      break;
    case 1: // second resistance
      pp = (high + low + close) / 3;
      t = pp + (high - low);
      output->setData(0, new PlotLineBar(color, t));
      break;
    case 2: // third resistance
      pp = (high + low + close) / 3;
      t = (2 * pp) + (high - (2 * low));
      output->setData(0, new PlotLineBar(color, t));
      break;
    case 3: // first support
      pp = (high + low + close) / 3;
      t = (2 * pp) - high;
      output->setData(0, new PlotLineBar(color, t));
      break;
    case 4: // second support
      pp = (high + low + close) / 3;
      t = pp - (high - low);
      output->setData(0, new PlotLineBar(color, t));
      break;
    case 5: // third support
      pp = (high + low + close) / 3;
      t = (2 * pp) - ((2 * high) - low);
      output->setData(0, new PlotLineBar(color, t));
      break;
    default:
      break;
  }

  return output;
}

int PP::dialog (int)
{
  int page = 0;
  QString k, d;
  PrefDialog *dialog = new PrefDialog;
  dialog->setWindowTitle(QObject::tr("Edit Indicator"));

  k = "R1";
  dialog->addPage(page, k);

  settings.getData(R1Color, d);
  dialog->addColorItem(R1Color, page, QObject::tr("Color"), d);

  settings.getData(R1Label, d);
  dialog->addTextItem(R1Label, page, QObject::tr("Label"), d);

  dialog->addCheckItem(R1Show, page, QObject::tr("Show"), settings.getInt(R1Show));

  page++;
  k = "R2";
  dialog->addPage(page, k);

  settings.getData(R2Color, d);
  dialog->addColorItem(R2Color, page, QObject::tr("Color"), d);

  settings.getData(R2Label, d);
  dialog->addTextItem(R2Label, page, QObject::tr("Label"), d);

  dialog->addCheckItem(R2Show, page, QObject::tr("Show"), settings.getInt(R2Show));

  page++;
  k = "R3";
  dialog->addPage(page, k);

  settings.getData(R3Color, d);
  dialog->addColorItem(R3Color, page, QObject::tr("Color"), d);

  settings.getData(R3Label, d);
  dialog->addTextItem(R3Label, page, QObject::tr("Label"), d);

  dialog->addCheckItem(R3Show, page, QObject::tr("Show"), settings.getInt(R3Show));

  page++;
  k = "S1";
  dialog->addPage(page, k);

  settings.getData(S1Color, d);
  dialog->addColorItem(S1Color, page, QObject::tr("Color"), d);

  settings.getData(S1Label, d);
  dialog->addTextItem(S1Label, page, QObject::tr("Label"), d);

  dialog->addCheckItem(S1Show, page, QObject::tr("Show"), settings.getInt(S1Show));

  page++;
  k = "S2";
  dialog->addPage(page, k);

  settings.getData(S2Color, d);
  dialog->addColorItem(S2Color, page, QObject::tr("Color"), d);

  settings.getData(S2Label, d);
  dialog->addTextItem(S2Label, page, QObject::tr("Label"), d);

  dialog->addCheckItem(S2Show, page, QObject::tr("Show"), settings.getInt(S2Show));

  page++;
  k = "S3";
  dialog->addPage(page, k);

  settings.getData(S3Color, d);
  dialog->addColorItem(S3Color, page, QObject::tr("Color"), d);

  settings.getData(S3Label, d);
  dialog->addTextItem(S3Label, page, QObject::tr("Label"), d);

  dialog->addCheckItem(S3Show, page, QObject::tr("Show"), settings.getInt(S3Show));

  int rc = dialog->exec();
  if (rc == QDialog::Rejected)
  {
    delete dialog;
    return rc;
  }

  getDialogSettings(dialog);

  delete dialog;
  return rc;
}

//*************************************************************
//*************************************************************
//*************************************************************

IndicatorPlugin * createIndicatorPlugin ()
{
  PP *o = new PP;
  return ((IndicatorPlugin *) o);
}


