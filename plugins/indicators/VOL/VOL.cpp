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

#include "VOL.h"
#include "MAFactory.h"
#include "PlotFactory.h"

#include <QtDebug>

VOL::VOL ()
{
  indicator = "VOL";

  settings.setData(UpColor, "green");
  settings.setData(DownColor, "red");
  settings.setData(NeutralColor, "blue");
  settings.setData(MAColor, "yellow");
  settings.setData(Plot, "Histogram Bar");
  settings.setData(MAPlot, "Line");
  settings.setData(Label, indicator);
  settings.setData(MALabel, "VOLMA");
  settings.setData(MAPeriod, 10);
  settings.setData(MAType, "SMA");
}

int VOL::getIndicator (Indicator &ind, BarData *data)
{
  QString s;
  settings.getData(Plot, s);
  PlotFactory fac;
  int lineType = fac.typeFromString(s);

  settings.getData(UpColor, s);
  QColor up(s);

  settings.getData(DownColor, s);
  QColor down(s);

  settings.getData(NeutralColor, s);
  QColor neutral(s);

  PlotLine *line = getVOL(data, lineType, up, down, neutral);
  if (! line)
    return 1;

  settings.getData(Label, s);
  line->setLabel(s);
  ind.addLine(line);

  // vol ma
  int period = settings.getInt(MAPeriod);

  settings.getData(MAType, s);
  MAFactory mau;
  int type = mau.typeFromString(s);

  settings.getData(MAColor, s);
  QColor color(s);

  settings.getData(MAPlot, s);
  lineType = fac.typeFromString(s);

  PlotLine *ma = mau.ma(line, period, type, lineType, color);
  if (ma)
  {
    settings.getData(MALabel, s);
    ma->setLabel(s);
    ind.addLine(ma);
  }

  return 0;
}

int VOL::getCUS (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data)
{
  // INDICATOR,PLUGIN,VOL,<NAME>,<PLOT TYPE>,<COLOR>
  //     0       1     2    3         4         5

  if (set.count() != 6)
  {
    qDebug() << indicator << "::getCUS: invalid parm count" << set.count();
    return 1;
  }

  PlotLine *tl = tlines.value(set[3]);
  if (tl)
  {
    qDebug() << indicator << "::getCUS: duplicate name" << set[3];
    return 1;
  }

  PlotFactory fac;
  int lineType = fac.typeFromString(set[4]);
  if (lineType == -1)
  {
    qDebug() << indicator << "::getCUS: invalid plot type" << set[4];
    return 1;
  }

  QColor color(set[5]);
  if (! color.isValid())
  {
    qDebug() << indicator << "::getCUS: invalid color" << set[5];
    return 1;
  }

  PlotLine *line = getVOL(data, lineType, color, color, color);
  if (! line)
    return 1;

  line->setLabel(set[3]);

  tlines.insert(set[3], line);

  return 0;
}

PlotLine * VOL::getVOL (BarData *data, int lineType, QColor &up, QColor &down, QColor &neutral)
{
  PlotFactory fac;
  PlotLine *vol = fac.plot(lineType);
  if (! vol)
    return 0;

  // set the first bar to neutral
  int loop = 0;
  Bar *bar = data->getBar(loop);
  vol->setData(loop, new PlotLineBar(neutral, bar->getVolume()));

  loop++;
  for (; loop < data->count(); loop++)
  {
    Bar *bar = data->getBar(loop);
    Bar *pbar = data->getBar(loop - 1);
    
    if (bar->getClose() < pbar->getClose())
      vol->setData(loop, new PlotLineBar(down, bar->getVolume()));
    else
    {
      if (bar->getClose() > pbar->getClose())
        vol->setData(loop, new PlotLineBar(up, bar->getVolume()));
      else
        vol->setData(loop, new PlotLineBar(neutral, bar->getVolume()));
    }
  }
  
  return vol;
}

int VOL::dialog (int)
{
  int page = 0;
  QString k, d;
  PrefDialog *dialog = new PrefDialog;
  dialog->setWindowTitle(QObject::tr("Edit Indicator"));

  k = QObject::tr("Settings");
  dialog->addPage(page, k);

  settings.getData(UpColor, d);
  dialog->addColorItem(UpColor, page, QObject::tr("Up Color"), d);

  settings.getData(DownColor, d);
  dialog->addColorItem(DownColor, page, QObject::tr("Down Color"), d);

  settings.getData(NeutralColor, d);
  dialog->addColorItem(NeutralColor, page, QObject::tr("Neutral Color"), d);

  PlotFactory fac;
  QStringList plotList;
  fac.list(plotList, TRUE);

  settings.getData(Plot, d);
  dialog->addComboItem(Plot, page, QObject::tr("Plot"), plotList, d);

  settings.getData(Label, d);
  dialog->addTextItem(Label, page, QObject::tr("Label"), d);

  page++;
  k = QObject::tr("MA");
  dialog->addPage(page, k);

  settings.getData(MAColor, d);
  dialog->addColorItem(MAColor, page, QObject::tr("Color"), d);

  settings.getData(MAPlot, d);
  dialog->addComboItem(MAPlot, page, QObject::tr("Plot"), plotList, d);

  settings.getData(MALabel, d);
  dialog->addTextItem(MALabel, page, QObject::tr("Label"), d);

  dialog->addIntItem(MAPeriod, page, QObject::tr("Period"), settings.getInt(MAPeriod), 1, 100000);

  QStringList maList;
  MAFactory mau;
  mau.list(maList);
  
  settings.getData(MAType, d);
  dialog->addComboItem(MAType, page, QObject::tr("Type"), maList, d);

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
  VOL *o = new VOL;
  return ((IndicatorPlugin *) o);
}


