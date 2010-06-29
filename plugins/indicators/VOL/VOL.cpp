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
  _indicator = "VOL";

  _settings.setData(UpColor, "green");
  _settings.setData(DownColor, "red");
  _settings.setData(NeutralColor, "blue");
  _settings.setData(MAColor, "yellow");
  _settings.setData(Plot, "Histogram Bar");
  _settings.setData(MAPlot, "Line");
  _settings.setData(Label, _indicator);
  _settings.setData(MALabel, "VOLMA");
  _settings.setData(MAPeriod, 10);
  _settings.setData(MAType, "SMA");
}

int VOL::getIndicator (Indicator &ind, BarData &data)
{
  QString s;
  _settings.getData(Plot, s);
  PlotFactory fac;
  int lineType = fac.typeFromString(s);

  _settings.getData(UpColor, s);
  QColor up(s);

  _settings.getData(DownColor, s);
  QColor down(s);

  _settings.getData(NeutralColor, s);
  QColor neutral(s);

  PlotLine *line = getVOL(data, lineType, up, down, neutral);
  if (! line)
    return 1;

  _settings.getData(Label, s);
  line->setLabel(s);

  s = "0";
  ind.setLine(s, line);
  ind.addPlotOrder(s);

  // vol ma
  int period = _settings.getInt(MAPeriod);

  _settings.getData(MAType, s);
  MAFactory mau;
  int type = mau.typeFromString(s);

  _settings.getData(MAColor, s);
  QColor color(s);

  _settings.getData(MAPlot, s);
  lineType = fac.typeFromString(s);

  PlotLine *ma = mau.ma(line, period, type, lineType, color);
  if (ma)
  {
    _settings.getData(MALabel, s);
    ma->setLabel(s);
    
    s = "1";
    ind.setLine(s, ma);
    ind.addPlotOrder(s);
  }

  return 0;
}

int VOL::getCUS (QStringList &set, Indicator &ind, BarData &data)
{
  // INDICATOR,PLUGIN,VOL,<NAME>,<PLOT TYPE>,<COLOR>
  //     0       1     2    3         4         5

  if (set.count() != 6)
  {
    qDebug() << _indicator << "::getCUS: invalid parm count" << set.count();
    return 1;
  }

  PlotLine *tl = ind.line(set[3]);
  if (tl)
  {
    qDebug() << _indicator << "::getCUS: duplicate name" << set[3];
    return 1;
  }

  PlotFactory fac;
  int lineType = fac.typeFromString(set[4]);
  if (lineType == -1)
  {
    qDebug() << _indicator << "::getCUS: invalid plot type" << set[4];
    return 1;
  }

  QColor color(set[5]);
  if (! color.isValid())
  {
    qDebug() << _indicator << "::getCUS: invalid color" << set[5];
    return 1;
  }

  PlotLine *line = getVOL(data, lineType, color, color, color);
  if (! line)
    return 1;

  line->setLabel(set[3]);

  ind.setLine(set[3], line);

  return 0;
}

PlotLine * VOL::getVOL (BarData &data, int lineType, QColor &up, QColor &down, QColor &neutral)
{
  if (data.count() < 2)
    return 0;
  
  PlotFactory fac;
  PlotLine *vol = fac.plot(lineType);
  if (! vol)
    return 0;

  // set the first bar to neutral
  int loop = 0;
  Bar bar = data.getBar(loop);
  vol->setData(loop, new PlotLineBar(neutral, bar.getVolume()));

  loop++;
  for (; loop < data.count(); loop++)
  {
    Bar bar = data.getBar(loop);
    Bar pbar = data.getBar(loop - 1);
    
    if (bar.getClose() < pbar.getClose())
      vol->setData(loop, new PlotLineBar(down, bar.getVolume()));
    else
    {
      if (bar.getClose() > pbar.getClose())
        vol->setData(loop, new PlotLineBar(up, bar.getVolume()));
      else
        vol->setData(loop, new PlotLineBar(neutral, bar.getVolume()));
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

  _settings.getData(UpColor, d);
  dialog->addColorItem(UpColor, page, QObject::tr("Up Color"), d);

  _settings.getData(DownColor, d);
  dialog->addColorItem(DownColor, page, QObject::tr("Down Color"), d);

  _settings.getData(NeutralColor, d);
  dialog->addColorItem(NeutralColor, page, QObject::tr("Neutral Color"), d);

  PlotFactory fac;
  QStringList plotList;
  fac.list(plotList, TRUE);

  _settings.getData(Plot, d);
  dialog->addComboItem(Plot, page, QObject::tr("Plot"), plotList, d);

  _settings.getData(Label, d);
  dialog->addTextItem(Label, page, QObject::tr("Label"), d, QString());

  page++;
  k = QObject::tr("MA");
  dialog->addPage(page, k);

  _settings.getData(MAColor, d);
  dialog->addColorItem(MAColor, page, QObject::tr("Color"), d);

  _settings.getData(MAPlot, d);
  dialog->addComboItem(MAPlot, page, QObject::tr("Plot"), plotList, d);

  _settings.getData(MALabel, d);
  dialog->addTextItem(MALabel, page, QObject::tr("Label"), d, QString());

  dialog->addIntItem(MAPeriod, page, QObject::tr("Period"), _settings.getInt(MAPeriod), 1, 100000);

  MAFactory mau;
  QStringList maList = mau.list();
  
  _settings.getData(MAType, d);
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


